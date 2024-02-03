#include "../include/conversions.h"
#include "../include/pipeline.h"
#include "../../detector/include/image_db.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <variant>


ImagePipeline::ImagePipeline(std::filesystem::path dirPath): dirPath(dirPath) {
    engine = std::make_unique<Engine>();
    engine->listen(Engine::EventType::FaceSaved, [this](auto event){
        Engine::SaveEventData data = std::get<Engine::SaveEventData>(event.data);
        emit faceAdded(convertToQt(data.image), QString::fromStdString(data.filename));
    });
    engine->listen(Engine::EventType::FaceRemoved, [this](auto event){
        Engine::RemoveEventData data = std::get<Engine::RemoveEventData>(event.data);
        emit faceRemoved(QString::fromStdString(data.filename));
    });
}

ImagePipeline::~ImagePipeline() {
    engine->terminate();
    engine->writeStats();
}

QImage ImagePipeline::read() {
    return convertToQt(engine->read());
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

void ImagePipeline::setBoxSize(unsigned size) {
    auto preset = engine->getPreset();
    preset.boxSize = size;
    engine->changePreset(preset);
}

void ImagePipeline::setOperation(ImageOperation operation, bool value) {
    auto preset = engine->getPreset();
    preset.opFlags.set(operation, value);
    engine->changePreset(preset);
}

void ImagePipeline::saveFace(const QString& name) {
    CmdMessage message;
    message.dbFlags.set(DbOperation::SaveFaceToDb, 1);
    message.filename = name.toStdString();
    engine->executeOnce(message);
}

void ImagePipeline::removeFace(const QString& name) {
    CmdMessage message;
    message.dbFlags.set(DbOperation::RemoveFaceFromDb, 1);
    message.filename = name.toStdString();
    engine->executeOnce(message);
}

void ImagePipeline::setCameraState(bool paused) {
    CameraMessage message;
    message.paused = paused;
    engine->executeOnce(message);
}


