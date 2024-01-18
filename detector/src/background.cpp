#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/background.h"



BackgroundSubtractor::BackgroundSubtractor() {
    kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    subtractor = cv::createBackgroundSubtractorMOG2();
}

cv::Mat BackgroundSubtractor::removeBackground(cv::Mat frame) {
    cv::Mat mask;
    subtractor->apply(frame, mask);
    cv::Mat outFrame;  // Result output
    frame.copyTo(outFrame, mask);
    return outFrame;
}
