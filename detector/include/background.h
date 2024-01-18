#ifndef RMBG_H
#define RMBG_H
#include <exception>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>


class BackgroundSubtractor
{
public:
    BackgroundSubtractor();
    cv::Mat removeBackground(cv::Mat frame);
private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> subtractor;
    cv::Mat kernel;
};


#endif

