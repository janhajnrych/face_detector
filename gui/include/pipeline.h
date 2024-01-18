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
#include "../../common/include/control_message.h"


class Pipeline: public QObject
{
    Q_OBJECT
public:
    virtual QImage read() =0;
    virtual void acceptMessage(ControlMessage message) =0;
    virtual void saveFace(QString name) =0;
    virtual void removeFace(QString name) =0;
    virtual void loadImageDir() =0;
    virtual void start() =0;
    virtual bool isRunning() const =0;
signals:
    void faceAdded(QImage, QString);
    void faceRemoved(QString);
};


#endif

