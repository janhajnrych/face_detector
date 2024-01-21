#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/kalman.h"



KalmanFilter::KalmanFilter(unsigned nDim, float noise_level):
    d(nDim),
    A(MatrixXd::Identity(d, d)),
    H(MatrixXd::Identity(d, d)),
    Q(MatrixXd::Zero(d, d)),
    R(MatrixXd::Constant(d, d, noise_level)),
    P(MatrixXd::Identity(d, d)),
    I(MatrixXd::Identity(d, d)),
    K(MatrixXd::Identity(d, d) * 0.5),
    x(VectorXd::Zero(d)),
    y(VectorXd::Zero(d)),
    C(MatrixXd::Identity(d, d)),
    S(MatrixXd::Identity(d, d)){
}

void KalmanFilter::setGuess(const VectorXd z) {
    x = z;
}

void KalmanFilter::calcGain() {
    x = A * x;
    P = A * P * A.transpose();
    P = P + Q;
    S = H * P * H.transpose() + R;
    K = (P * H.transpose()).array() / S.array();
}

VectorXd KalmanFilter::operator()(const VectorXd& z) {
    calcGain();
    y = z - H * x;
    x = x + K * y;
    P = (I - K * H) * P;
    return x;
}

VectorXd KalmanFilter::operator()() {
    calcGain();
    P = (I - K * H) * P;
    return x;
}
