#ifndef KALMAN_H
#define KALMAN_H
#include <Eigen/Dense>
#include "component.h"
#include "exception.h"

using namespace Eigen;

class KalmanFilter: public Component
{
public:
    using Exception = ComponentException<KalmanFilter>;
    KalmanFilter(unsigned nDim, float noise_level = 1);
    VectorXd operator()(const VectorXd& z);
    VectorXd operator()();
    void setGuess(const VectorXd z);
    void setModelTransform(const MatrixXd a);
    MatrixXd getModelTransform() const;
private:
    unsigned d = 2;
    MatrixXd A, B, H, P, I, K, Q, C, R, S;
    VectorXd x, y;
    int n;
    void calcGain();
};


#endif

