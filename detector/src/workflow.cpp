#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/workflow.h"
#include <type_traits>
#include <iostream>
#include "../include/detector.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <iostream>
#include <tuple>

namespace {

    template <class T>
    std::unique_ptr<T> createDetector(const std::string& cascadePath) {
        static_assert(std::is_base_of<FaceDetector, T>::value, "wrong detector type");
        std::unique_ptr<T> detector;
        try {
            detector = std::make_unique<T>();
            detector->load(cascadePath);
        }
        catch (FaceDetector::Exception exception) {
            detector = nullptr;
            std::cerr << exception.what() << std::endl;
        }
        return detector;
    }

    inline cv::Point getRectCenter(const cv::Rect& rect){
        return cv::Point((rect.br() + rect.tl())/2);
    }


}


Workflow::Workflow():
    faceDetector(createDetector<FaceDetector>("../data/haarcascade_frontalface_default.xml")){}

void Workflow::detectFaces(cv::Mat frame, float relativeBoxSize) {
    detections.clear();
    faceDetector->detect(frame, detections);
    for (auto& face : detections) {
        cv::Point center = getRectCenter(face);
        face.width = static_cast<int>(face.width * relativeBoxSize);
        face.height = static_cast<int>(face.height * relativeBoxSize);
        cv::Point newCenter = getRectCenter(face);
        face.x-= newCenter.x - center.x;
        face.y-= newCenter.y - center.y;
    }
}

cv::Rect Workflow::clipRect(cv::Size size, cv::Rect rect) {
    cv::Range cols(std::max(rect.tl().x, 0), std::min(rect.br().x, size.width));
    cv::Range rows(std::max(rect.tl().y, 0), std::min(rect.br().y, size.height));
    return cv::Rect(cols.start, rows.start, cols.end - cols.start, rows.end - rows.start);
}

cv::Mat Workflow::getSlice(cv::Mat image, cv::Rect rect) {
    // auto size = image.size();
    // cv::Range cols(std::max(rect.tl().x, 0), std::min(rect.br().x, size.width));
    // cv::Range rows(std::max(rect.tl().y, 0), std::min(rect.br().y, size.height));
    return image(clipRect(image.size(), rect));
}

cv::Mat Workflow::drawFaceRects(cv::Mat image) {
    static double alpha = 0.33;
    static double minAlpha = 0.01;
    cv::Mat overlay;
    image.copyTo(overlay);
    for(auto& face: detections){
        cv::Rect rect(face);
        cv::rectangle(image, rect.tl(), rect.br(), color, 4);
        if (alpha < minAlpha)
            continue;
        cv::rectangle(overlay, rect.tl(), rect.br(), color, -1);
        cv::addWeighted(overlay, alpha, image, 1 - alpha, 0, image);
    }
    return image;
}

cv::Rect Workflow::getLargestFace(const std::vector<cv::Rect>& faces) {
    auto iter = std::max_element(faces.begin(), faces.end(), [](auto a, auto b){
        return a.area() < b.area();
    });
    return *iter;
}

const std::vector<cv::Rect>& Workflow::getAllFaces() const {
    return detections;
}

std::vector<cv::Rect> Workflow::getLargerFaces(unsigned minArea) const {
    std::vector<cv::Rect> results;
    std::copy_if(detections.begin(), detections.end(), std::back_inserter(results), [minArea](cv::Rect rect){
        return rect.area() >= minArea;
    });
    return results;
}

std::vector<cv::Rect> Workflow::getLargerFaces(unsigned minArea, size_t maxNumber) const {
    std::vector<cv::Rect> results;
    std::copy_if(detections.begin(), detections.end(), std::back_inserter(results), [minArea](cv::Rect rect){
        return rect.area() >= minArea;
    });
    auto comparator = [](cv::Rect left, cv::Rect right) {
        return left.area() < right.area();
    };
    std::priority_queue<cv::Rect, std::vector<cv::Rect>, decltype(comparator)> queue(results.begin(), results.end(), comparator);
    results.clear();
    while (!queue.empty()) {
        results.push_back(std::move(const_cast<cv::Rect&>(queue.top())));
        queue.pop();
    }
    return results;
}
