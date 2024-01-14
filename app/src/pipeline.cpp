#include "../include/conversions.h"
#include "../include/pipeline.h"
#include <memory>
#include <opencv2/opencv.hpp>


ImagePipeline::ImagePipeline(std::filesystem::path dirPath): dirPath(dirPath) {
    workflow = std::make_shared<Workflow>();
    engine = std::make_unique<Engine>(workflow);
}


ImagePipeline::~ImagePipeline() {
    engine->terminate();
}

QImage ImagePipeline::processFrame(QImage image, ControlMessage message) {
    Engine::Config newConfig;
    newConfig.boxSize = message.boxSize;
    newConfig.flags = message.flags;
    auto result = engine->acceptFrame(convertToCv(image), newConfig);
    return convertToQt(result);
}


void ImagePipeline::start() {
    engine->start();
}


