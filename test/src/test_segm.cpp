#include <gtest/gtest.h>
#include "../../detector/include/segment.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>


class SegmenterFixture : public testing::TestWithParam<std::string> {
protected:
    void SetUp() override {
        imgDir = std::filesystem::path("data");
        segmenter = std::make_unique<Segmenter>(getModelPath().string());
    }

    std::filesystem::path getModelPath() {
        const auto dir = std::filesystem::current_path().parent_path() / std::filesystem::path("data");
        return dir / std::filesystem::path("bisenet.onnx");
    }

    cv::Mat readImage(const std::string& filename) const {
        std::filesystem::path path = imgDir / filename;
        return cv::imread(path.string(), cv::IMREAD_COLOR);

    }

    std::filesystem::path writeImage(cv::Mat image, const std::string& filename) const {
        std::filesystem::path path = imgDir / filename;
        cv::imwrite(path.string(), image);
        return path;
    }

    std::unique_ptr<Segmenter> segmenter;
    std::filesystem::path imgDir;
};


// TEST_P(SegmenterFixture, PrepareTest) {
//     std::string filename = GetParam();
//     auto image = readImage(filename);
//     auto prepared = segmenter->prepare(image);
//     auto size = prepared.size();
//     EXPECT_EQ(size.width, 256);
//     EXPECT_EQ(size.height, 256);
// }

TEST_P(SegmenterFixture, SegmentTest) {
    std::string filename = GetParam();
    auto image = readImage(filename);
    auto masks = segmenter->calcMask(image);
    masks = Segmenter::filterClasses(masks, Segmenter::SegClass::Hair);
    auto size = masks.size();
    EXPECT_EQ(size.width, 256);
    EXPECT_EQ(size.height, 256);
    double min, max;
    cv::minMaxLoc(masks, &min, &max);
    EXPECT_EQ(min, 0);
    EXPECT_EQ(max, 255);
    cv::Mat resized;
    cv::resize(image, resized, segmenter->size, cv::INTER_NEAREST);
    cv::Mat masked;
    cv::Mat overlay(resized.cols, resized.rows, CV_8UC3, cv::Scalar(255, 0, 0));
    float alpha = 0.33;
    cv::Mat temp;
    cv::addWeighted(overlay, alpha, resized, 1 - alpha, 0, temp);
    temp.copyTo(masked, masks);
    cv::Mat invertedMask;
    cv::bitwise_not(masks, invertedMask);
    resized.copyTo(masked, invertedMask);
    cv::Mat out;
    cv::resize(masked, out, image.size(), cv::INTER_AREA);
    auto out_filename = std::filesystem::path(filename);
    writeImage(out, out_filename.stem().string() + "_masked.png");
}

INSTANTIATE_TEST_CASE_P(
    SegmentationTests,
    SegmenterFixture,
    ::testing::Values(
        "img4.jpg",
        "img5.jpg",
        "img6.jpg"
    ));
