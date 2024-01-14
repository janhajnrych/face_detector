#ifndef CONVERSIONS_H
#define CONVERSIONS_H
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>

cv::Mat convertToCv(QImage image);
QImage convertToQt(cv::Mat mat);


#endif

