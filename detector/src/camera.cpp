#include "../include/camera.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include <filesystem>
#include <iostream>
#include <algorithm>


Camera::Camera(): fps(0), readWaitTime(std::chrono::milliseconds(500)) {}

Camera::~Camera() {
    stop();
}

void Camera::start() {
    capture.open(-1);
    running = true;
    if (capture.isOpened()){
        fps = capture.get(cv::CAP_PROP_FPS);
        int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
        //lastFrame = cv::Mat(width, height, CV_8UC3, cv::Scalar(0, 0, 0));
        tempFrame = cv::Mat(width, height, CV_8UC3, cv::Scalar(0, 0, 0));
        readWaitTime = std::chrono::milliseconds(2*1000/fps);
    }
}

void Camera::stop() {
    if (running && capture.isOpened()){
        capture.release();
    }
    running = false;
}

void Camera::next() {
    if (!capture.isOpened())
        return;
    auto success = capture.read(tempFrame);
    if (!success)
        return;
    if (!running){
        stop();
        return;
    } else {
        if (tempFrame.empty())
            return;
        if (paused){
            if (!backupFrame.has_value()){
                cv::Mat lastFrame;
                tempFrame.copyTo(lastFrame);
                backupFrame = lastFrame;
            }
            return;
        }
        frameQueue.write(tempFrame);
    }
}

bool Camera::isRunning() const {
    return running;
}

cv::Mat Camera::getLastFrame() const {
    return backupFrame.value_or(tempFrame);
}

cv::Mat Camera::read() {
    return frameQueue.readWithTimeout(readWaitTime).value_or(backupFrame.value_or(tempFrame));
}

int Camera::getFps() const {
    return fps;
}

void Camera::setPause(bool newValue) {
    this->paused = newValue;
    if (!newValue)
        backupFrame.reset();
}

bool Camera::isPaused() const {
    return this->paused;
}



