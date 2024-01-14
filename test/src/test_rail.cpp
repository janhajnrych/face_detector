#include <gtest/gtest.h>
#include "../../detector/include/rail.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>


class RailFixture : public testing::Test {
protected:
    void SetUp() override {
        imgDir = std::filesystem::path("data");
    }

    cv::Mat readImage(const std::string& filename) const {
        std::filesystem::path path = imgDir / filename;
        return cv::imread(path.string(), cv::IMREAD_COLOR);
    }

    std::filesystem::path imgDir;
};



TEST_F(RailFixture, TestIo) {
    auto rail = DataRail<int>();
    int out = 0;
    auto read_thread = std::thread([&](){
        out = rail.read();
    });
    auto write_thread = std::thread([&](){
        rail.write(1);
    });
    EXPECT_EQ(out, 0);
    write_thread.join();
    read_thread.join();
    EXPECT_EQ(out, 1);
}
