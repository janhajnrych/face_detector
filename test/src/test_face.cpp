#include <gtest/gtest.h>
#include "../../detector/include/detector.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>


class FaceDetectorFixture : public testing::TestWithParam<std::string> {
protected:
    void SetUp() override {
        detector = std::make_unique<FaceDetector>();
        detector->load(getModelPath().string());
        imgDir = std::filesystem::path("data");
    }

    std::filesystem::path getModelPath() {
        const auto dir = std::filesystem::current_path().parent_path() / std::filesystem::path("data");
        return dir / std::filesystem::path("haarcascade_frontalface_default.xml");
    }

    cv::Mat readImage(const std::string& filename) const {
        std::filesystem::path path = imgDir / filename;
        return cv::imread(path.string(), cv::IMREAD_COLOR);
    }

    std::unique_ptr<FaceDetector> detector;
    std::filesystem::path imgDir;
};


TEST_F(FaceDetectorFixture, FaceDetectorLoad) {
    EXPECT_NO_THROW(detector->load(getModelPath().string()));
}

TEST_F(FaceDetectorFixture, FaceDetectorLoadException) {
    EXPECT_THROW(detector->load("???"), FaceDetector::Exception);
}

TEST_P(FaceDetectorFixture, FaceDetectionWithFileReading) {
    std::string filename = GetParam();
    auto image = readImage(filename);
    std::vector<cv::Rect> detections;
    detector->detect(image, detections);
    EXPECT_EQ(detections.size(), 1);
}

INSTANTIATE_TEST_CASE_P(
    FaceDetectionTest,
    FaceDetectorFixture,
    ::testing::Values(
        "img1.jpg",
        "img2.jpg",
        "img3.jpg"
    ));
