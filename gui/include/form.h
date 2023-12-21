#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H
#include "ui_form.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QImage>
#include <vector>
#include <memory>
#include "pipeline.h"

class CaptureForm : public QWidget
{
    Q_OBJECT

public:
    explicit CaptureForm(std::unique_ptr<Pipeline> pipeline, QWidget *parent = nullptr);
    virtual ~CaptureForm(){ }

private slots:
    void updateActivity(bool checked);
private:
    Ui_MainForm ui;
    QScopedPointer<QCamera> camera;
    QScopedPointer<QCameraImageCapture> imageCapture;
    void processCapturedImage(int requestId, const QImage& img);
    bool active = false;
    std::unique_ptr<Pipeline> pipeline;
};


#endif

