#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/detector.h"


void FaceDetector::load(const std::string& cascadePath) {
    cascade.load(cascadePath);
    if(cascade.empty()){
        throw FaceDetector::Exception("Not loaded!");
    }
}

void FaceDetector::detect(cv::Mat image, std::vector<cv::Rect>& detections) const {
    const_cast<cv::CascadeClassifier*>(&cascade)->detectMultiScale(image, detections, 1.1, 10, 0, cv::Size(minSize, minSize));
}
