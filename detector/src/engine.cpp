#include "../include/engine.h"
#include "../include/workflow.h"
#include "../include/detector.h"
#include <functional>
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <fstream>
#include "../../common/include/version.h"


class Context {
public:

    cv::Mat execute(const cv::Mat& frame, Workflow& workflow) {
        auto flags = ControlFlags(opFlags);
        if (flags.any())
            workflow.detectFaces(frame, static_cast<float>(boxSize) * 0.01f);
        cv::Mat result = frame;
        if (flags[ImageOperation::Detection])
            result = workflow.drawFaceRects(result, flags[ImageOperation::Stablization]);
        if (flags[ImageOperation::Recognition]){
            auto offset = flags[ImageOperation::Detection] ? -0.5 : 0.0;
            result = workflow.drawFaceNames(result, offset);
        }
        if (flags[ImageOperation::Segmentation])
            result = workflow.segment(result);
        return result;
    }

    std::atomic<unsigned> opFlags;
    std::atomic<unsigned> boxSize = 100;
    std::queue<Engine::Event> eventQueue;
};

class Dispatcher {
public:
    static void createDbTask(const CmdMessage& message, std::queue<Engine::Task>& taskQueue){
        taskQueue.push(Engine::Task(getDbFunc(message)));
    }

    static void createPresetTask(const ControlMessage& message, std::queue<Engine::Task>& taskQueue) {
        auto func = [message](Workflow&, Context& context, cv::Mat&){
            context.opFlags = static_cast<unsigned>(message.opFlags.to_ulong());
            context.boxSize = message.boxSize;
            return true;
        };
        taskQueue.push(Engine::Task(func));
    }

    using TaskFunction = std::function<Engine::TaskResult(Workflow&, Context&, cv::Mat&)>;

    static TaskFunction getDbFunc(const CmdMessage& message){
        if (message.dbFlags[DbOperation::SaveFaceToDb] && message.filename.has_value()){
            return [message](Workflow& workflow, Context& context, cv::Mat& frame){
                auto face = workflow.getLargestFace(frame);
                if (!face.has_value())
                    return false;
                auto result = workflow.saveFace(face.value(), message.filename.value());
                if (!result)
                    return result;
                Engine::SaveEventData data;
                face.value().copyTo(data.image);
                data.filename = message.filename.value();
                context.eventQueue.push(Engine::Event(Engine::EventType::FaceSaved, data));
                return result;
            };
        }
        else if (message.dbFlags[DbOperation::RemoveFaceFromDb] && message.filename.has_value()){
            return [message](Workflow& workflow, Context& context, cv::Mat&){
                auto result = workflow.removeFace(message.filename.value());
                if (!result)
                    return result;
                Engine::RemoveEventData data;
                data.filename = message.filename.value();
                context.eventQueue.push(Engine::Event(Engine::EventType::FaceRemoved, data));
                return result;
            };
        }
        return [](Workflow&, Context&, cv::Mat&){
            return true;
        };
    }
};


Engine::Engine():
    workflow(std::make_unique<Workflow>()),
    finished(true),
    readWaitTime(std::chrono::milliseconds(1000/60)) {
    profilers[ProfileType::CamFps] = std::make_unique<Profiler>(0, 80, 80);
    profilers[ProfileType::ReadFps] = std::make_unique<Profiler>(0, 80, 80);
    context = std::make_unique<Context>();
}


void Engine::start() {
    finished = false;
    cameraThread = std::thread([&](){
        camera.start();
        while (camera.isRunning() && !finished) {
            camera.next();
            if (BuildInfo::isDebug())
                profilers.at(ProfileType::CamFps)->measure();
            if (finished)
                camera.stop();
        }
    });
    producerThread = std::thread([&](){
        while (!finished) {
            auto frame = camera.read();
            if (frame.empty())
                continue;
            inputRail.write(frame);
        }
    });
    consumerThread = std::thread([&](){
        while (!finished) {
            auto frame = inputRail.readMove();
            execCommands(frame);
            frame = context->execute(frame, *workflow);
            outputBuffer.write(frame);
        }
    });
}

void Engine::execCommands(cv::Mat frame) {
    std::lock_guard<std::mutex> lock(mutex);
    while (!taskQueue.empty()){
        auto task = std::move(taskQueue.front());
        task(*workflow, *context, frame);
        taskQueue.pop();
        auto result = task.get_future();
        auto errorCode = result.get();
    }
}

cv::Mat Engine::read() {
    if (!camera.isRunning())
        return camera.getLastFrame();
    auto frame = outputBuffer.readWithTimeout(std::chrono::milliseconds(2*1000/camera.getFps())).value_or(camera.getLastFrame());
    if (BuildInfo::isDebug())
        profilers.at(ProfileType::ReadFps)->measure();
    while (!context->eventQueue.empty()){
        Event event;
        {
            std::lock_guard<std::mutex> lock(mutex);
            event = context->eventQueue.front();
            context->eventQueue.pop();
        }
        if (listeners.find(event.type) != listeners.end())
            listeners.at(event.type)(Event(event));
    }
    return frame;
}

void Engine::changePreset(const ControlMessage& message) {
    std::lock_guard<std::mutex> lock(mutex);
    Dispatcher::createPresetTask(message, taskQueue);
}

void Engine::executeOnce(const CmdMessage& message) {
    std::lock_guard<std::mutex> lock(mutex);
    Dispatcher::createDbTask(message, taskQueue);
}

void Engine::executeOnce(const CameraMessage& message) {
    camera.setPause(message.paused);
}

ControlMessage Engine::getPreset() const {
    ControlMessage config;
    config.boxSize = context->boxSize;
    config.opFlags = ControlFlags(context->opFlags);
    return config;
}

void Engine::listen(EventType eventType, Listener listener) {
    listeners[eventType] = listener;
}

void Engine::loadDirToDb(std::filesystem::path dirPath) {
    for (const auto& dirEntry: std::filesystem::directory_iterator(dirPath)) {
        auto path = dirEntry.path();
        if (!workflow->allowedExtensions.contains(path.extension().string()))
            continue;
        auto name = path.filename().stem();
        auto face = workflow->loadFace(path, name);
        if (!listeners.contains(Engine::EventType::FaceSaved))
            continue;
        Engine::SaveEventData eventData;
        eventData.filename = name;
        eventData.image = face;
        listeners[Engine::EventType::FaceSaved](Event(Engine::EventType::FaceSaved, eventData));
    }
}

Engine::~Engine() {
    finished = true;
    if (cameraThread.joinable())
        cameraThread.join();
    if (producerThread.joinable())
        producerThread.join();
    if (consumerThread.joinable())
        consumerThread.join();
}

void Engine::terminate() {
    finished = true;
    producerThread.join();
    consumerThread.join();
    cameraThread.join();
}

bool Engine::isRunning() const {
    return !finished;
}

void Engine::writeStats() {
    if (BuildInfo::isDebug()) {
        if (profilers.contains(ProfileType::CamFps)){
            std::ofstream outFileCam("camFps.csv");
            profilers.at(ProfileType::CamFps)->write(outFileCam);
        }
        if (profilers.contains(ProfileType::ReadFps)){
            std::ofstream outFile("readFps.csv");
            profilers.at(ProfileType::ReadFps)->write(outFile);
        }
    }
}
