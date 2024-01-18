#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/core.hpp>
#include <memory>
#include "rail.h"
#include <atomic>
#include <opencv2/videoio.hpp>
#include "rail.h"


class Camera
{
public:
    explicit Camera();
    ~Camera();
    void start();
    void stop();
    void next();
    bool isRunning() const;
    cv::Mat getLastFrame() const;
    cv::Mat read();
    int getFps() const;
private:
    cv::Mat frame;
    DataRail<cv::Mat> frameQueue;
    std::atomic_bool running = false;
    cv::VideoCapture capture;
    std::chrono::milliseconds readWaitTime;
    int fps;
};


#endif

