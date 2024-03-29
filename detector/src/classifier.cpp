#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/classifier.h"
#include <filesystem>
#include <iostream>
#include <opencv2/dnn/dnn.hpp>
#include <algorithm>


namespace {

    static cv::Mat toGray(cv::Mat colorMat) {
        cv::Mat greyMat;
        cv::cvtColor(colorMat, greyMat, cv::COLOR_BGR2GRAY);
        return greyMat;
    }

}

Classifier::Classifier(const std::string& modelPath) {
    load(modelPath);
}

cv::Mat Classifier::getEmbedding(cv::Mat image) {
    cv::Mat blob;
    static const cv::Size size = cv::Size(96, 96);
    cv::dnn::blobFromImage(toGray(image), blob, 1.0, size, cv::Scalar(), true, false);
    model.setInput(blob);
    std::vector<cv::Mat> outputs;
    model.forward(outputs, model.getUnconnectedOutLayersNames());
    if (outputs.size() != 1)
        throw "wrong array";
    return outputs[0];
}

// void Classifier::load(const std::string& path){
//     try {
//         net = cv::dnn::readNetFromONNX(path);
//     }
//     catch (cv::Exception& e) {
//         throw Classifier::Exception("Not loaded!");
//     }
// }


