#ifndef DETECTOR_H
#define DETECTOR_H
#include <exception>
#include <string>
#include <opencv2/objdetect/objdetect.hpp>
#include "component.h"
#include "exception.h"


class FaceDetector: public Component
{
public:
    using Exception = ComponentException<FaceDetector>;
    void load(const std::string& cascadePath);
    void detect(cv::Mat cvImage, std::vector<cv::Rect>& detections) const;
protected:
    cv::CascadeClassifier cascade;
    unsigned minSize = 64;
};


#endif

