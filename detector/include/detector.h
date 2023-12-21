#ifndef DETECTOR_H
#define DETECTOR_H
#include <opencv2/objdetect/objdetect.hpp>
#include <vector>
#include <string>

class DetectionException: public std::exception
{
public:
    explicit DetectionException(const std::string& message): message("DetectorException: " + message) {}
    virtual ~DetectionException() noexcept {}
    virtual const char* what() const noexcept {
        return message.c_str();
    }

protected:
    std::string message;
};

class Detector
{
public:
    explicit Detector(const std::string& cascadePath);
    cv::Mat drawFaces(cv::Mat cvImage);
private:
    cv::CascadeClassifier face_cascade;
};


#endif

