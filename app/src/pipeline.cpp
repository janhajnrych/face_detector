#include "../include/conversions.h"
#include "../include/pipeline.h"
#include "../../detector/include/image_db.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <variant>


ImagePipeline::ImagePipeline(std::filesystem::path dirPath): dirPath(dirPath) {
    engine = std::make_unique<Engine>();
    engine->listen(Engine::EventType::FaceSaved, [this](auto event){
        emit faceAdded(convertToQt(event.image), QString::fromStdString(event.data));
    });
    engine->listen(Engine::EventType::FaceRemoved, [this](auto event){
        emit faceRemoved(QString::fromStdString(event.data));
    });
}

ImagePipeline::~ImagePipeline() {
    engine->terminate();
    engine->writeStats();
}

QImage ImagePipeline::read() {
    return convertToQt(engine->read());
}

void ImagePipeline::changePreset(ControlMessage message) {
    engine->changePreset(message);
}


void ImagePipeline::scheduleCommand(CmdMessage message) {
    engine->executeOnce(message);
}

void ImagePipeline::loadImageDir() {
    engine->loadDirToDb(dirPath);
}

void ImagePipeline::start() {
    engine->start();
}

bool ImagePipeline::isRunning() const {
    return engine->isRunning();
}


