#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/detector.h"



Detector::Detector(const std::string& cascadePath) {
    face_cascade.load(cascadePath);
    if(face_cascade.empty()){
        throw DetectionException("Classifier has not been loaded!");
    }
}

cv::Mat Detector::drawFaces(cv::Mat image) {
    std::vector<cv::Rect> faces;
    const static unsigned minSize = 64;
    face_cascade.detectMultiScale(image, faces, 1.1, 10, 0, cv::Size(minSize, minSize));
    const static double alpha = 0.33;
    const static cv::Scalar color(0, 0, 255);
    cv::Mat overlay;
    image.copyTo(overlay);
    for(auto& face : faces){
        cv::rectangle(image, face.tl(), face.br(), color, 4);
        cv::rectangle(overlay, face.tl(), face.br(), color, -1);
        cv::addWeighted(overlay, alpha, image, 1 - alpha, 0, image);
    }
    return image;
}
