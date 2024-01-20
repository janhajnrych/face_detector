#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H
#include <memory>
#include "../../gui/include/pipeline.h"
#include <QImage>
#include <future>
#include "../../detector/include/engine.h"


class Workflow;


class ImagePipeline: public Pipeline {
public:
    explicit ImagePipeline(std::filesystem::path dirPath);
    ~ImagePipeline();
    QImage read();
    void loadImageDir();
    void start();
    bool isRunning() const;
    void setBoxSize(unsigned size);
    void setOperation(ImageOperation operation, bool value);
    void saveFace(const QString& name);
    void removeFace(const QString& name);
    void setCameraState(bool paused);
private:
    std::unique_ptr<Engine> engine;
    std::filesystem::path dirPath;
};


#endif

