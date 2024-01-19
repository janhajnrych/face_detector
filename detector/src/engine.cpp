#include "../include/engine.h"
#include "../include/workflow.h"
#include "../include/detector.h"
#include <functional>
#include <opencv2/opencv.hpp>
#include <map>


class Dispatcher {
private:
    using TaskFunction = std::function<Engine::TaskResult(cv::Mat)>;
    using ImageFunction = std::function<cv::Mat(cv::Mat)>;
    using ImageTaskFactory = std::function<ImageFunction(ControlMessage config, Workflow* workflow)>;
    using DbTaskFactory = std::function<ImageFunction(CmdMessage config, Workflow* workflow)>;
public:

    Dispatcher() {
        imageTaskFactories[ImageOperation::Detection] = [](ControlMessage, Workflow* workflow) {
            return std::bind(&Workflow::drawFaceRects, workflow, std::placeholders::_1);
        };
        imageTaskFactories[ImageOperation::Recognition] = [](ControlMessage message, Workflow* workflow) {
            float offset = message.opFlags[ImageOperation::Detection] ? -0.5f: 0.f;
            return std::bind(&Workflow::drawFaceNames, workflow, std::placeholders::_1, offset * message.boxSize);
        };
        imageTaskFactories[ImageOperation::Segmentation] = [](ControlMessage, Workflow* workflow){
            return std::bind(&Workflow::segment, workflow, std::placeholders::_1);
        };
        imageOpOrder = {ImageOperation::Detection, ImageOperation::Segmentation, ImageOperation::Recognition};
        imageOpOrder.shrink_to_fit();
        dbTaskFactories[DbOperation::SaveFaceToDb] = [this](CmdMessage config, Workflow* workflow) {
            return std::bind(&Dispatcher::makeFaceWrite, this, std::placeholders::_1, workflow, config);
        };
        dbTaskFactories[DbOperation::RemoveFaceFromDb] = [this](CmdMessage config, Workflow* workflow) {
            return std::bind(&Dispatcher::makeFaceRemove, this, std::placeholders::_1, workflow, config);
        };
    }

    void createImageTasks(ControlMessage config, Workflow* workflow, DataRail<Engine::Task>& taskQueue) {
        if (config.opFlags.any()){
            auto func = std::bind(&Dispatcher::makeDetections, std::placeholders::_1, config.boxSize, workflow);
            Engine::Task&& task = Engine::Task(withErrorCode(func));
            taskQueue.writeMove(std::move(task));
        }
        for (auto operation: imageOpOrder) {
            if (!config.opFlags[operation])
                continue;
            auto factory = imageTaskFactories[operation];
            auto taskFunc = factory(config, workflow);
            Engine::Task&& task = Engine::Task(withErrorCode(taskFunc));
            taskQueue.writeMove(std::move(task));
        }
    }

    void createDbTasks(CmdMessage message, Workflow* workflow, DataRail<Engine::Task>& taskQueue) {
        for (auto operation: dbOpOrder) {
            if (!message.dbFlags[operation])
                continue;
            auto factory = dbTaskFactories[operation];
            auto taskFunc = factory(message, workflow);
            Engine::Task&& task = Engine::Task(withErrorCode(taskFunc));
            taskQueue.writeMove(std::move(task));
        }
    }

    static TaskFunction withErrorCode(const std::function<cv::Mat(cv::Mat)>& function) { // decorator
        return [function](cv::Mat frame) {
            auto output = frame;
            try {
                output = function(frame);
            }
            catch (Workflow::Exception&) {
                return std::make_tuple(output, 1);
            }
            catch (cv::Exception& e) {
                return std::make_tuple(output, 2);
            }
            return std::make_tuple(output, 0);
        };
    }

    std::queue<Engine::Event> eventQueue;

private:
    static cv::Mat makeDetections(cv::Mat frame, unsigned boxSize, Workflow* workflow) {
        float relativeBoxSize = static_cast<float>(boxSize) * 0.01f;
        workflow->detectFaces(frame, relativeBoxSize);
        return frame;
    }

    cv::Mat makeFaceWrite(cv::Mat frame, Workflow* workflow, CmdMessage config) {
        auto name = config.filename.value_or("face.png");
        auto face = workflow->saveFace(frame, name);
        Engine::Event event;
        event.data = name;
        event.image = face;
        event.type = Engine::EventType::FaceSaved;
        eventQueue.push(event);
        return frame;
    }

    cv::Mat makeFaceRemove(cv::Mat frame, Workflow* workflow, CmdMessage config) {
        if (!config.filename.has_value())
            return frame;
        auto name = config.filename.value();
        auto success = workflow->removeFace(name);
        if (!success)
            return frame;
        Engine::Event event;
        event.data = name;
        event.type = Engine::EventType::FaceRemoved;
        eventQueue.push(event);
        return frame;
    }

    std::unordered_map<ImageOperation, ImageTaskFactory> imageTaskFactories;
    std::unordered_map<DbOperation, DbTaskFactory> dbTaskFactories;
    std::vector<ImageOperation> imageOpOrder;
    std::vector<DbOperation> dbOpOrder;


};


Engine::Engine():
    workflow(std::make_unique<Workflow>()),
    finished(true),
    readWaitTime(std::chrono::milliseconds(1000/60)),
    dispatcher(std::make_unique<Dispatcher>()) {}


void Engine::start() {
    finished = false;
    cameraThread = std::thread([&](){
        camera.start();
        while (camera.isRunning() && !finished) {
            camera.next();
            if (finished)
                camera.stop();
        }
    });
    producerThread = std::thread([&](){
        while (!finished) {
            auto frame = camera.read();
            if (frame.empty())
                continue;
            ControlMessage config;
            config.boxSize = boxSize;
            config.opFlags = ControlFlags(flags);
            unsigned nTasksStart = taskQueue.size();
            this->dispatcher->createImageTasks(config, workflow.get(), taskQueue);
            inputRail.write(std::make_tuple(frame, taskQueue.size() - nTasksStart));
        }
    });
    consumerThread = std::thread([&](){
        while (!finished) {
            auto [frame, nTasks] = inputRail.readMove();
            for (unsigned i=0; i < nTasks; i++) {
                if (taskQueue.size() == 0)
                    break;
                auto task = taskQueue.readMove();
                auto future = task.get_future();
                task(frame);
                auto [newFrame, errorCode] = future.get();
                if (errorCode == 0) {
                    outputBuffer.write(newFrame);
                }
                else
                    outputBuffer.write(frame);
            }
            while (!dispatcher->eventQueue.empty()){
                auto event = dispatcher->eventQueue.front();
                dispatcher->eventQueue.pop();
                if (listeners.find(event.type) != listeners.end())
                    listeners.at(event.type)(event);
            }
        }
    });
}

cv::Mat Engine::read() {
    if (!camera.isRunning())
        return camera.getLastFrame();
    return outputBuffer.readWithTimeout(std::chrono::milliseconds(1000/camera.getFps())).value_or(camera.getLastFrame());
}

void Engine::changePreset(const ControlMessage& message) {
    flags = static_cast<unsigned>(message.opFlags.to_ulong());
    boxSize = message.boxSize;
}

void Engine::executeOnce(const CmdMessage& message) {
    dispatcher->createDbTasks(message, workflow.get(), taskQueue);
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
        Engine::Event event;
        event.data = name;
        event.image = face;
        event.type = Engine::EventType::FaceSaved;
        listeners[event.type](event);
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
