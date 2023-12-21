#include "../include/form.h"
#include <QCheckBox>
#include <QCameraInfo>
#include <QPushButton>
#include <QUrl>
#include <QAudioEncoderSettings>
#include <QFileDialog>
#include <QDir>

CaptureForm::CaptureForm(std::unique_ptr<Pipeline> pipeline, QWidget *parent): QWidget(parent), pipeline(std::move(pipeline))
{
    ui.setupUi(this);
    connect(ui.activityCheckBox, &QCheckBox::toggled, this, &CaptureForm::updateActivity);
    ui.label->setText("waiting");
    ui.activityCheckBox->setCheckState(active ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    imageCapture.reset(new QCameraImageCapture(camera.data()));
    //faces.reserve(100);

    //setWindowState(Qt::WindowMaximized);

    connect(imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &CaptureForm::processCapturedImage);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->start();
}

void CaptureForm::updateActivity(bool checked)
{
    ui.label->setText(checked ? "loading" : "waiting");
    ui.detectButton->setEnabled(checked);
    active = checked;
    if (active)
        imageCapture->capture();
}

void CaptureForm::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);
    if (!active)
        return;
    QImage scaledImage = img.scaled(ui.label->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    auto convertedImage = scaledImage.convertToFormat(QImage::Format_RGB888, Qt::ColorOnly).rgbSwapped();
    Pipeline::Image image(convertedImage.bits(), convertedImage.width(), convertedImage.height(), convertedImage.bytesPerLine());
    auto output = pipeline->execute(image);
    QImage view(output.data, output.width, output.height, output.bytesPerLine, QImage::Format_RGB888);
    ui.label->setPixmap(QPixmap::fromImage(view.rgbSwapped()));
    imageCapture->capture();
}
