#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <string>
#include "model.h"


class Classifier: public Model<cv::dnn::Net>
{
public:
    using Exception = ComponentException<Classifier>;
    explicit Classifier() = delete;
    explicit Classifier(const Classifier&) = delete;
    explicit Classifier(const std::string& modelPath);
    cv::Mat getEmbedding(cv::Mat image);
private:
    //cv::dnn::Net net;
    //void load(const std::string& path);
};


#endif

