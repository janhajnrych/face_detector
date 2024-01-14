#include "../include/form.h"
#include <QCheckBox>
#include <QCameraInfo>
#include <QPushButton>
#include <QUrl>
#include <QAudioEncoderSettings>
#include <QFileDialog>
#include <QDir>
#include <QListView>
#include <QInputDialog>
#include <optional>
#include <iostream>
#include <unordered_map>
#include <QMessageBox>


std::optional<QString> showIndentificationDialog(QWidget* parent){
    bool ok = false;
    QString name = QInputDialog::getText(parent, "Identification",
                                         "Person's name:", QLineEdit::Normal,
                                         "", &ok);
    if (ok)
        return name;
    return std::optional<QString>();
}

QImage convert(QImage image, QSize size) {
    QImage scaledImage = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return scaledImage.convertToFormat(QImage::Format_RGB888, Qt::ColorOnly);
}


CaptureForm::CaptureForm(std::unique_ptr<Pipeline> pipeline, QWidget *parent): QWidget(parent), pipeline(std::move(pipeline))
{
    ui.setupUi(this);
    connect(ui.activityCheckBox, &QCheckBox::toggled, this, &CaptureForm::updateActivity);
    connect(ui.locateCheckBox, &QCheckBox::toggled, this, [this](auto value){
        markFace = value;
    });
    ui.label->setText("waiting");
    ui.activityCheckBox->setCheckState(active ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui.locateCheckBox->setCheckState(markFace ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    imageCapture.reset(new QCameraImageCapture(camera.data()));
    connect(imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &CaptureForm::processCapturedImage);
    connect(ui.stopButton, &QPushButton::clicked, this, &CaptureForm::pause);
    connect(ui.startButton, &QPushButton::clicked, this, &CaptureForm::unpause);
    ui.boxSizeSlider->setMinimum(50);
    ui.boxSizeSlider->setValue(100);
    ui.boxSizeSlider->setMaximum(200);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    this->pipeline->start();
    camera->start();
    unpause();
}

void CaptureForm::updateActivity(bool checked)
{
    ui.label->setText(checked ? "loading" : "waiting");
    active = checked;
    if (active)
        imageCapture->capture();
}

void CaptureForm::pause() {
    paused = true;
    updateControlButtons();
}

void CaptureForm::unpause() {
    paused = false;
    updateControlButtons();
    imageCapture->capture();
}

void CaptureForm::updateControlButtons(){
    ui.stopButton->setVisible(!paused);
    ui.startButton->setVisible(paused);
}

QImage CaptureForm::processImage(QImage frame) {
    Pipeline::ControlMessage message;
    Pipeline::ControlMessage::Flags flags = Pipeline::ControlMessage::defaultFlag;
    flags.set(0, markFace);
    message.flags = flags;
    message.boxSize = ui.boxSizeSlider->value();
    auto output = pipeline->processFrame(frame, message);      
    return output;
}

void CaptureForm::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);
    if (!active)
        return;
    lastFrame = img;
    auto size = ui.label->size();
    auto output = processImage(convert(lastFrame, size));
    ui.label->setPixmap(QPixmap::fromImage(convert(output, size)));
    if (paused)
        return;
    imageCapture->capture();
}
