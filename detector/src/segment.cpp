#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/segment.h"
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


Segmenter::Segmenter(const std::string& modelPath) {
    load(modelPath);
}

cv::Mat Segmenter::calcMask(cv::Mat image) {
    cv::Mat frame;
    cv::resize(image, frame, size, cv::INTER_NEAREST);
    cv::Mat blob;
    auto mean = cv::Scalar(0.485, 0.456, 0.406);
    blob = cv::dnn::blobFromImage(frame, 1.0 / 255.0, cv::Size(), mean, true, false);
    auto std = cv::Scalar(0.229, 0.224, 0.225);
    cv::divide(blob, std, blob);
    net.setInput(blob);
    cv::Mat outBlob;
    net.forward(outBlob);
    return argmax(outBlob);
}

cv::Mat Segmenter::filterClasses(cv::Mat mask, SegClass classes) {
    cv::Mat thresholded;
    if (classes == SegClass::Face) {
        cv::threshold(mask, thresholded, 15, 255, cv::THRESH_TOZERO_INV);
        cv::threshold(thresholded, thresholded, 0, 255, cv::THRESH_BINARY);
    }
    else if (classes == SegClass::Hair){
        cv::threshold(mask, thresholded, 17, 255, cv::THRESH_TOZERO_INV);
        cv::threshold(mask, thresholded, 16, 255, cv::THRESH_TOZERO);
        cv::threshold(thresholded, thresholded, 0, 255, cv::THRESH_BINARY);
    }
    return thresholded;
}

cv::Mat Segmenter::argmax(cv::Mat blob){
    auto rows = blob.size[2];
    auto cols = blob.size[3];
    auto nChannels = blob.size[1];
    cv::Mat classID = cv::Mat::zeros(rows, cols, CV_8U);
    cv::Mat maxVal(rows, cols, CV_32F, blob.data);
    for (int ch = 0; ch < nChannels; ch++){
        for (int row = 0; row < rows; row++){
            const auto *ptrScore = blob.ptr<float>(0, ch, row);
            auto *ptrMaxCl = classID.ptr<char>(row);
            auto *ptrMaxVal = maxVal.ptr<float>(row);
            for (int col = 0; col < cols; col++){
                if (ptrScore[col] > ptrMaxVal[col]){
                    ptrMaxVal[col] = ptrScore[col];
                    ptrMaxCl[col] = static_cast<char>(ch);
                }
            }
        }
    }
    return classID;
}

cv::Mat Segmenter::colorize(cv::Mat image, cv::Mat mask, cv::Scalar color) {
    cv::Mat out;
    image.copyTo(out, mask);
    return image;
}

void Segmenter::load(const std::string& path){
    cv::dnn::Net network;
    try {
        network = cv::dnn::readNetFromONNX(path);
    }
    catch (cv::Exception& e) {
        throw Segmenter::Exception(std::string("Not loaded! ") + std::string(e.what()));
    }
    net = network;
}


