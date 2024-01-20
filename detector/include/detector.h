#ifndef DETECTOR_H
#define DETECTOR_H
#include <exception>
#include <string>
#include <opencv2/objdetect/objdetect.hpp>
#include "model.h"


class FaceDetector: public Model<cv::CascadeClassifier>
{
public:
    void detect(cv::Mat cvImage, std::vector<cv::Rect>& detections) const;
    FaceDetector(const std::string& path);
protected:
    unsigned minSize = 64;
};


#endif

