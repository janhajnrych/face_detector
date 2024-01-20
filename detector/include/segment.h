#ifndef SEGMENT_H
#define SEGMENT_H
#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <string>
#include "model.h"


class Segmenter: public Model<cv::dnn::Net>
{
public:
    using Exception = ComponentException<Segmenter>;
    enum class SegClass {
        Face = 0,
        Hair = 1
    };
    explicit Segmenter(const std::string& modelPath);
    cv::Mat calcMask(cv::Mat image);
    static cv::Mat filterClasses(cv::Mat mask, SegClass classes = SegClass::Face);
    static cv::Mat argmax(cv::Mat blob);
    static cv::Mat colorize(cv::Mat image, cv::Mat mask, cv::Scalar color);
    const cv::Size size = cv::Size(256, 256);
};


#endif

