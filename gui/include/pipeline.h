#ifndef PIPELINE_H
#define PIPELINE_H
#include <optional>
#include <vector>
#include <string>
#include <QImage>
#include <QObject>
#include <memory>
#include <filesystem>
#include "image_list_model.h"
#include <bitset>
#include "../../gui/include/image_list_model.h"


class Pipeline: public QObject
{
    Q_OBJECT
public:
    struct ControlMessage {
        using Flags = std::bitset<3>;
        constexpr static Flags defaultFlag = {0b000};
        Flags flags = defaultFlag;
        unsigned boxSize = 100;
    };
    virtual QImage processFrame(QImage image, ControlMessage message) =0;
    virtual void start() =0;
};


#endif

