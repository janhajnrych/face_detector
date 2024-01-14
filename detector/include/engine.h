#ifndef ENGINE_H
#define ENGINE_H
#include <memory>
#include <thread>
#include <opencv2/core.hpp>
#include "../include/rail.h"
#include <tuple>
#include <atomic>
#include <bitset>
#include <queue>
#include <future>
#include <atomic>

class Workflow;

class Engine
{
public:
    enum Operation { WithFaceRect=0, WithFaceNames=1, WithSegm=2};
    using Flags = std::bitset<3>;
    struct Config {
        Flags flags = {0b000};
        unsigned boxSize = 100;
    };
    explicit Engine(std::shared_ptr<Workflow> workfow);
    cv::Mat acceptFrame(cv::Mat frame, const Config& config);
    void start();
    void terminate();
    ~Engine();
    bool isRunning() const;
private:
    std::shared_ptr<Workflow> workflow;
    std::thread producerThread, consumerThread;
    DataRail<std::tuple<cv::Mat, Config>> inputRail;
    DataRail<cv::Mat> outputRail;
    DataRail<cv::Mat> outputBuffer;
    cv::Mat processFrame(cv::Mat frame, Config config);
    std::atomic_bool finished;
    std::chrono::milliseconds readWaitTime;
};


#endif

