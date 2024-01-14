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
    QImage processFrame(QImage image, ControlMessage message);
    void start();
private:
    std::shared_ptr<Workflow> workflow;
    std::unique_ptr<Engine> engine;
    const std::string saveExt = ".png";
    std::filesystem::path dirPath;
};


#endif

