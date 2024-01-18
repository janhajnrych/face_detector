#include <gtest/gtest.h>
#include "../../detector/include/camera.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>


class CameraFixture : public testing::Test {
protected:
    void SetUp() override {
        //imgDir = std::filesystem::path("data");
    }

    // cv::Mat readImage(const std::string& filename) const {
    //     std::filesystem::path path = imgDir / filename;
    //     return cv::imread(path.string(), cv::IMREAD_COLOR);
    // }

//    std::filesystem::path imgDir;

};



TEST_F(CameraFixture, TestCamera) {
    std::atomic<int> n = 10;
    std::vector<int> frames;
    std::atomic<int> i = 0;
    Camera camera;
    auto camThread = std::thread([&](){
        camera.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        while (camera.isRunning() && i < n) {
            camera.next();
            auto frame = camera.read();
            frames.push_back(frame.cols);
            if (i >= n)
                camera.stop();
            i++;
        }
        camera.stop();
    });
    camThread.join();
    EXPECT_EQ(frames.size(), n);
}
