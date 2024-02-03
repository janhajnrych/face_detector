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
    void setPause(bool newValue);
    bool isPaused() const;
private:
    cv::Mat tempFrame;
    std::optional<cv::Mat> backupFrame;
    DataRail<cv::Mat> frameQueue;
    std::atomic_bool running = false;
    std::atomic_bool paused = false;
    cv::VideoCapture capture;
    std::chrono::milliseconds readWaitTime;
    int fps;
};


#endif

