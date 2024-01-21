#ifndef STABILIZER_H
#define STABILIZER_H
#include <opencv2/core.hpp>
#include <string>
#include "component.h"
#include "exception.h"
#include <memory>
#include <Eigen/Dense>
#include "../include/kalman.h"

class Stabilizer: public Component
{
public:
    using Exception = ComponentException<Stabilizer>;
    Stabilizer();
    cv::Rect stabilize(const cv::Rect& rect);
    cv::Rect stabilize();
    void setHint(const cv::Rect& rect);
private:
    static constexpr size_t nDim = 4;
    std::unique_ptr<KalmanFilter> kalmanFilter;
    Eigen::VectorXd input, output;
};


#endif

