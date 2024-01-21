#include <opencv2/imgproc.hpp>
#include <exception>
#include "../include/stabilizer.h"




Stabilizer::Stabilizer():
    kalmanFilter(std::make_unique<KalmanFilter>(nDim, 5)),
    input(VectorXd::Zero(nDim)),
    output(VectorXd::Zero(nDim)){

}

cv::Rect Stabilizer::stabilize(const cv::Rect& rect) {
    auto tl = rect.tl();
    auto br = rect.br();
    input[0] = tl.x;
    input[1] = tl.y;
    input[2] = br.x;
    input[3] = br.y;
    output = kalmanFilter->operator()(input);
    cv::Point pt1(output[0], output[1]);
    cv::Point pt2(output[2], output[3]);
    return cv::Rect(pt1, pt2);
}

cv::Rect Stabilizer::stabilize() {
    output = kalmanFilter->operator()();
    cv::Point pt1(output[0], output[1]);
    cv::Point pt2(output[2], output[3]);
    return cv::Rect(pt1, pt2);
}

void Stabilizer::setHint(const cv::Rect& rect) {
    auto tl = rect.tl();
    auto br = rect.br();
    input[0] = tl.x;
    input[1] = tl.y;
    input[2] = br.x;
    input[3] = br.y;
    kalmanFilter->setGuess(input);
}
