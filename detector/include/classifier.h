#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <string>
#include "component.h"
#include "exception.h"


class Classifier: public Component
{
public:
    using Exception = ComponentException<Classifier>;
    explicit Classifier(const std::string& modelPath);
    cv::Mat getEmbedding(cv::Mat image);
    void load(const std::string& path);
private:
    cv::dnn::Net net;

};


#endif

