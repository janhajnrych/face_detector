#ifndef WORKFLOW_H
#define WORKFLOW_H
#include <exception>
#include <string>
#include <opencv2/objdetect/objdetect.hpp>
#include <memory>
#include <unordered_map>
#include <optional>
#include <filesystem>
#include <deque>
#include <tuple>
#include <boost/circular_buffer.hpp>

class FaceDetector;


class Workflow
{
public:
    explicit Workflow();
    void detectFaces(cv::Mat frame, float relativeBoxSize = 1.0);
    cv::Mat drawFaceRects(cv::Mat image);
    static cv::Mat getSlice(cv::Mat image, cv::Rect rect);
    const std::vector<cv::Rect>& getAllFaces() const;
    std::vector<cv::Rect> getLargerFaces(unsigned minArea) const;
    std::vector<cv::Rect> getLargerFaces(unsigned minArea, size_t maxNumber) const;
    const std::string saveExt = ".png";
private:
    std::unique_ptr<FaceDetector> faceDetector;
    std::vector<cv::Rect> detections;
    cv::Scalar color = cv::Scalar(0, 0, 255);
    static cv::Rect getLargestFace(const std::vector<cv::Rect>& faces);
    static cv::Rect clipRect(cv::Size size, cv::Rect rect);

};


#endif

