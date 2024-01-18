#ifndef COMMAND_H
#define COMMAND_H
#include "workflow.h"
#include "workflow.h"
#include <opencv2/core.hpp>

template <typename T>
concept Command = requires(T s) {
    s.execute(std::declval<Workflow>(), std::declval<cv::Mat>());
};


#endif

