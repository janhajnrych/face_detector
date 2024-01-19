#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H
#include <memory>
#include "../../gui/include/pipeline.h"
#include "../../detector/include/detector.h"
#include "../../detector/include/workflow.h"
#include <QImage>
#include "../../gui/include/image_list_model.h"
#include <future>
#include "../../detector/include/engine.h"
#include <bitset>


class Workflow;


class ImagePipeline: public Pipeline {
public:
    explicit ImagePipeline(std::filesystem::path dirPath);
    ~ImagePipeline();
    void changePreset(ControlMessage message);
    void scheduleCommand(CmdMessage message);
    void scheduleCommand(CameraMessage message);
    QImage read();
    void loadImageDir();
    void start();
    bool isRunning() const;
private:
    std::unique_ptr<Engine> engine;
    std::filesystem::path dirPath;
};


#endif

