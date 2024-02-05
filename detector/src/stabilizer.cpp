#include <opencv2/imgproc.hpp>
#include <exception>
#include "../include/stabilizer.h"

namespace {

    cv::Point getCenter(const cv::Rect& rect){
        auto tl = rect.tl();
        auto br = rect.br();
        return cv::Point(0.5*(tl.x, br.x), 0.5*(tl.y, br.y));
    }

}


Stabilizer::Stabilizer(float dt):
    kalmanFilter(std::make_unique<KalmanFilter>(nDim, 6)),
    input(VectorXd::Zero(nDim)),
    output(VectorXd::Zero(nDim)){
    setTimeStep(dt);
}

void Stabilizer::setTimeStep(float dt) {
    auto transform = kalmanFilter->getModelTransform();
    transform(0, 4) = dt;
    transform(1, 5) = dt;
    transform(2, 4) = dt;
    transform(3, 5) = dt;
    kalmanFilter->setModelTransform(transform);
}

cv::Rect Stabilizer::stabilize(const cv::Rect& rect, float dt) {
    auto tl = rect.tl();
    auto br = rect.br();
    input[0] = tl.x;
    input[1] = tl.y;
    input[2] = br.x;
    input[3] = br.y;
    auto currentCenter = getCenter(rect);
    auto prevCenter = getCenter(prevRect.value_or(rect));
    input[4] = (currentCenter.x - prevCenter.x)/dt;
    input[5] = (currentCenter.y - prevCenter.y)/dt;
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
