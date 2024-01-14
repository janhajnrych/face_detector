#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H
#include "ui_form.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QImage>
#include <vector>
#include <memory>
#include "pipeline.h"
#include "../include/image_list_model.h"
#include <unordered_map>

class CaptureForm : public QWidget
{
    Q_OBJECT

public:
    explicit CaptureForm(std::unique_ptr<Pipeline> pipeline, QWidget *parent = nullptr);
    virtual ~CaptureForm(){ }
private slots:
    void updateActivity(bool checked);
    void pause();
    void unpause();
private:
    Ui_MainForm ui;
    QScopedPointer<QCamera> camera;
    QScopedPointer<QCameraImageCapture> imageCapture;
    void processCapturedImage(int requestId, const QImage& img);
    QImage processImage(QImage image);
    bool active = false;
    bool paused = false;
    bool markFace = false;
    std::shared_ptr<Pipeline> pipeline;
    void updateControlButtons();
    ImageListModel* imageListModel;
    QImage lastFrame;
};


#endif

