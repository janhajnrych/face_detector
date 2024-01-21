#include <gtest/gtest.h>
#include "../../detector/include/kalman.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>
#include <random>
#include <Eigen/Dense>
#include <fstream>
#include <math.h>
#include <functional>

class KalmanFilterFixture : public testing::TestWithParam<std::tuple<float, float, float, int, std::string>> {
protected:
    void SetUp() override {
        distr = std::uniform_real_distribution<double>(0, 1);
        generator = std::mt19937(std::random_device()());
    }

    double getRandFromDistr(double noiseLevel) {
        return distr(generator) * noiseLevel;
    }

    std::function<Eigen::VectorXd(double)> createGenerator(double baseline, double max_amplitude, double period, double decay, double noiseLevel) {
        return [this, baseline, max_amplitude, period, decay, noiseLevel](double t){
            Eigen::VectorXd m = Eigen::VectorXd(d);
            auto noise = getRandFromDistr(noiseLevel);
            double x = baseline + max_amplitude * std::sin(t / period) * std::exp(-t * decay) + noise;
            double y = baseline + max_amplitude * std::cos(t / period) * std::exp(-t * decay) + noise;
            m[0] = static_cast<int>(x);
            m[1] = static_cast<int>(y);
            return m;
        };
    }

    std::uniform_real_distribution<double> distr;
    std::mt19937 generator;
    size_t d = 2;
};


TEST_P(KalmanFilterFixture, KalmanFilterTest) {
    auto [noiseLevel, baseline, maxAmplitude, gap, filePath] = GetParam();
    Eigen::VectorXd m = Eigen::VectorXd(d);
    Eigen::VectorXd y_pred = Eigen::VectorXd(d);
    KalmanFilter kalmanFilter(d, noiseLevel);
    kalmanFilter.setGuess(Eigen::VectorXd::Constant(d, baseline));
    std::ofstream file(filePath);
    file << "t" << "," << "x_m" << "," << "y_m" << "," << "x_k" << "," << "y_k" << std::endl;
    double t = 0;
    size_t n1 = 200;
    auto gen1 = createGenerator(baseline, maxAmplitude, 8, 0.02, noiseLevel);
    for (size_t i=0; i<n1; i++) {
        t = static_cast<double>(i);
        m = gen1(t);
        y_pred = kalmanFilter(m);
        file << static_cast<int>(t) << "," << m[0] << "," << m[1] << "," << y_pred[0] << "," << y_pred[1] << std::endl;
    }
    for (size_t i=n1; i<n1+gap; i++) {
        y_pred = kalmanFilter();
    }
    n1+= gap;
    size_t n2 = 100;
    Eigen::VectorXd y_pred_prev = Eigen::VectorXd::Zero(d);
    Eigen::VectorXf dy_sums = Eigen::VectorXf::Zero(d);
    for (size_t i=n1; i<n2+n1; i++) {
        t = static_cast<double>(i);
        m = gen1(t);
        y_pred = kalmanFilter(m);
        dy_sums[0]+= fabs(y_pred_prev[0] - y_pred[0]);
        dy_sums[1]+= fabs(y_pred_prev[1] - y_pred[1]);
        y_pred_prev = y_pred;
        file << static_cast<int>(t) << "," << m[0] << "," << m[1] << "," << y_pred[0] << "," << y_pred[1] << std::endl;
    }
    double dy1_mean = dy_sums[0]/static_cast<double>(n2);
    ASSERT_LT(dy1_mean, noiseLevel);
    double dy2_mean = dy_sums[1]/static_cast<double>(n2);
    ASSERT_LT(dy2_mean, noiseLevel);
    n2 = n1 + n2;
    size_t n3 = 200;
    y_pred_prev = Eigen::VectorXd::Zero(d);
    dy_sums = Eigen::VectorXf::Zero(d);
    auto gen2 = createGenerator(baseline, maxAmplitude, 16, 0.02, noiseLevel);
    for (size_t i=n2; i<n2+n3; i++) {
        t = static_cast<double>(i);
        double t3 = static_cast<double>(i - n2);
        m = gen2(t3);
        y_pred = kalmanFilter(m);
        dy_sums[0]+= fabs(y_pred_prev[0] - y_pred[0]);
        dy_sums[1]+= fabs(y_pred_prev[1] - y_pred[1]);
        y_pred_prev = y_pred;
        file << static_cast<int>(t) << "," << m[0] << "," << m[1] << "," << y_pred[0] << "," << y_pred[1] << std::endl;
    }
    dy1_mean = dy_sums[0]/static_cast<double>(n3);
    ASSERT_LT(dy1_mean, noiseLevel);
    dy2_mean = dy_sums[1]/static_cast<double>(n3);
    ASSERT_LT(dy2_mean, noiseLevel);
    file.close();
}

INSTANTIATE_TEST_CASE_P(
    KalmanFilterTest,
    KalmanFilterFixture,
    ::testing::Values(
        std::make_tuple(8, 50, 20, 0, "test1.csv"),
        std::make_tuple(2, 80, 20, 0, "test2.csv"),
        std::make_tuple(4, 50, 10, 0, "test3.csv"),
        std::make_tuple(8, 50, 10, 50, "test4.csv")
    ));
