#include "../include/conversions.h"


cv::Mat convertToCv(QImage image) {
    auto swapped = image.rgbSwapped();
    return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
}

QImage convertToQt(cv::Mat mat){
    return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
}
