#ifndef STABILIZER_H
#define STABILIZER_H
#include <opencv2/core.hpp>
#include <string>
#include "component.h"
#include "exception.h"
#include <memory>
#include <Eigen/Dense>
#include "../include/kalman.h"
#include <optional>


class Stabilizer: public Component
{
public:
    using Exception = ComponentException<Stabilizer>;
    Stabilizer(float dt = 1.0/60.0);
    void setTimeStep(float dt);
    cv::Rect stabilize(const cv::Rect& rect, float dt = 1.0/60.0);
    cv::Rect stabilize();
    void setHint(const cv::Rect& rect);
private:
    static constexpr size_t nDim = 6;
    std::unique_ptr<KalmanFilter> kalmanFilter;
    Eigen::VectorXd input, output;
    std::optional<cv::Rect> prevRect;
};


#endif

