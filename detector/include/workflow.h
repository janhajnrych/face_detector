#ifndef WORKFLOW_H
#define WORKFLOW_H
#include <exception>
#include <string>
#include <opencv2/objdetect/objdetect.hpp>
#include <memory>
#include <unordered_map>
#include <optional>
#include "image_db.h"
#include <filesystem>
#include <deque>
#include <tuple>
#include "segment.h"
#include "component.h"
#include "exception.h"
#include "../include/stabilizer.h"
#include <Eigen/Dense>


class FaceDetector;
class ImageDb;
class Classifier;
class Segmenter;
class Stabilizer;


class Workflow: Component
{
public:
    using Exception = ComponentException<Workflow>;
    explicit Workflow();
    void detectFaces(cv::Mat frame, float relativeBoxSize = 1.0);
    cv::Mat drawFaceRects(cv::Mat image, bool stabilized);
    static cv::Mat getSlice(cv::Mat image, cv::Rect rect);
    const std::vector<cv::Rect>& getAllFaces() const;
    std::vector<cv::Rect> getLargerFaces(unsigned minArea) const;
    std::vector<cv::Rect> getLargerFaces(unsigned minArea, size_t maxNumber) const;
    cv::Mat drawText(cv::Mat image, cv::Point pos, const std::string& text) const;
    cv::Mat drawFaceNames(cv::Mat image, float offset = 0.f);
    bool saveFace(cv::Mat image, const std::string& name);
    cv::Mat loadFace(std::filesystem::path path, const std::string& name);
    cv::Mat segment(cv::Mat image);
    bool removeFace(const std::string& name);
    const std::string defaultSaveExt = ".png";
    const std::unordered_set<std::string> allowedExtensions = {".png",  ".jpg"};
    std::optional<cv::Mat> getLargestFace(cv::Mat frame) const;
private:
    std::unique_ptr<FaceDetector> faceDetector;
    std::unique_ptr<Classifier> classifier;
    std::unique_ptr<Segmenter> segmenter;
    std::unique_ptr<ImageDb> imageDb;
    std::unique_ptr<Stabilizer> stabilizer;
    std::vector<cv::Rect> detections;
    cv::Scalar defaultColor = cv::Scalar(0, 0, 255);
    cv::Scalar stabilizedColor = cv::Scalar(0, 255, 0);
    static cv::Rect getLargestRect(const std::vector<cv::Rect>& faces);
    std::string getFilename(const std::string& name) const;
    static cv::Rect clipRect(cv::Size size, cv::Rect rect);
    struct NameScores {
        Eigen::VectorXf scores;
        Eigen::VectorXf scoreBuffer;
        Eigen::VectorXd indices;
    };
    NameScores nameScores;
    void resetNameScores();
};


#endif

