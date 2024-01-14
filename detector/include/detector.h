#ifndef DETECTOR_H
#define DETECTOR_H
#include <exception>
#include <string>
#include <opencv2/objdetect/objdetect.hpp>


class FaceDetector
{
public:
    class Exception: public std::exception
    {
    public:
        explicit Exception(const std::string& message);
        virtual ~Exception() noexcept;
        virtual const char* what() const noexcept;
    protected:
        std::string message;
    };
    void load(const std::string& cascadePath);
    void detect(cv::Mat cvImage, std::vector<cv::Rect>& detections) const;
protected:
    cv::CascadeClassifier cascade;
    unsigned minSize = 64;
};


#endif

