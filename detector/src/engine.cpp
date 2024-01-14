#include "../include/engine.h"
#include "../include/workflow.h"
#include "../include/detector.h"
#include <functional>
#include <opencv2/opencv.hpp>


Engine::Engine(std::shared_ptr<Workflow> workfow): workflow(workfow), finished(true), readWaitTime(std::chrono::milliseconds(1000/60)){}


void Engine::start() {
    finished = false;
    producerThread = std::thread([&](){
        while (!finished) {
            auto [frame, config] = inputRail.readMove();
            outputRail.write(processFrame(frame, config));
        }
    });
    consumerThread = std::thread([&](){
        while (!finished) {
            auto frame = outputRail.readMove();
            outputBuffer.write(frame);
        }
    });
}


cv::Mat Engine::acceptFrame(cv::Mat frame, const Config& config) {
    inputRail.writeMove(std::make_tuple(std::move(frame), config));
    auto result = outputBuffer.readWithTimeout(readWaitTime).value_or(frame);
    return result;
}

cv::Mat Engine::processFrame(cv::Mat frame, Config config) {
    if (config.flags == 0)
        return frame;
    auto boxSize = static_cast<float>(config.boxSize)/100.0;
    auto flags = config.flags;
    workflow->detectFaces(frame, boxSize);
    float offset = flags[Operation::WithFaceRect] ? -0.5f: 0.f;
    auto result = frame;
    // if (flags[Operation::WithSegm])
    //     result = workflow->segment(result);
    if (flags[Operation::WithFaceRect])
        result = workflow->drawFaceRects(result);
    // if (flags[Operation::WithFaceNames])
    //     result = workflow->drawFaceNames(result, offset * boxSize);
    return result;
}

Engine::~Engine() {
    finished = true;
    if (producerThread.joinable())
        producerThread.join();
    if (consumerThread.joinable())
        consumerThread.join();
}

void Engine::terminate() {
    finished = true;
    producerThread.join();
    consumerThread.join();
}

bool Engine::isRunning() const {
    return !finished;
}
