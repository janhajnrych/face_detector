#include <gtest/gtest.h>
#include "../../detector/include/classifier.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>


class FaceClassifierFixture : public testing::TestWithParam<std::tuple<std::string, std::string, float, float>> {
protected:
    void SetUp() override {
        classifier = std::make_unique<Classifier>();
        classifier->load(getModelPath(classifierFilename).string());
        imgDir = std::filesystem::path("data");
    }

    std::filesystem::path getModelPath(const std::string& filename) {
        const auto dir = std::filesystem::current_path().parent_path() / std::filesystem::path("data");
        return dir / std::filesystem::path(filename);
    }

    cv::Mat readImage(const std::string& filename) const {
        std::filesystem::path path = imgDir / filename;
        return cv::imread(path.string(), cv::IMREAD_COLOR);
    }

    std::string classifierFilename = "classifier.onnx";
    std::unique_ptr<Classifier> classifier;
    std::filesystem::path imgDir;
};


TEST_F(FaceClassifierFixture, FaceClassifierLoad) {
    EXPECT_NO_THROW(classifier->load(getModelPath(classifierFilename).string()));
}

TEST_F(FaceClassifierFixture, FaceClassifierLoadException) {
    EXPECT_THROW(classifier->load("???"), Classifier::Exception);
}

TEST_P(FaceClassifierFixture, FaceClassifierWithFileReading) {
    auto [path_a, path_b, expected_score, tolerance] = GetParam();
    auto image_a = readImage(path_a);
    auto image_b = readImage(path_b);
    auto embedding_a = classifier->getEmbedding(image_a);
    auto embedding_b = classifier->getEmbedding(image_b);
    auto score = cv::norm(embedding_a - embedding_b);
    EXPECT_NEAR(score, expected_score, tolerance);
}

INSTANTIATE_TEST_CASE_P(
    FaceClassifierTest,
    FaceClassifierFixture,
    ::testing::Values(
        std::make_tuple("img1.jpg", "img1.jpg", 0, 1e-3),
        std::make_tuple("img2.jpg", "img2.jpg", 0, 1e-3),
        std::make_tuple("img1.jpg", "img2.jpg", 1.5, 0.5),
        std::make_tuple("img1.jpg", "img2.jpg", 1.5, 0.5),
        std::make_tuple("img2.jpg", "img3.jpg", 1.5, 0.5)
    ));
