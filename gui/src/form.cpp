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
#include <QChartView>

void configureListView(QListView* imageList){
    imageList->setViewMode(QListView::IconMode);
    imageList->setFlow(QListView::LeftToRight);
    imageList->setSelectionMode(QListView::SingleSelection);
}

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

// QChartView* createChart() {
//     return new QChartView();
// }


CaptureForm::CaptureForm(std::unique_ptr<Pipeline> pipeline, QWidget *parent): QWidget(parent), pipeline(std::move(pipeline))
{
    ui.setupUi(this);
    connect(ui.activityCheckBox, &QCheckBox::toggled, this, &CaptureForm::updateActivity);
    connect(ui.locateCheckBox, &QCheckBox::toggled, this, &CaptureForm::toggleDetection);
    ui.label->setText("waiting");
    ui.activityCheckBox->setCheckState(active ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui.locateCheckBox->setCheckState(controlFlags[Operation::Detection] ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    imageListModel = new ImageListModel(ui.listView);
    ui.listView->setModel(imageListModel);
    configureListView(ui.listView);
    connect(ui.stopButton, &QPushButton::clicked, this, &CaptureForm::pause);
    connect(ui.startButton, &QPushButton::clicked, this, &CaptureForm::unpause);
    connect(ui.detectButton, &QPushButton::clicked, this, &CaptureForm::toggleClassification);
    connect(ui.segmentButton, &QPushButton::clicked, this, &CaptureForm::toggleSegmentation);
    ui.boxSizeSlider->setMinimum(50);
    ui.boxSizeSlider->setValue(100);
    ui.boxSizeSlider->setMaximum(200);
    connect(ui.addFaceButton, &QPushButton::clicked, this, &CaptureForm::saveFace);
    connect(this->pipeline.get(), &Pipeline::faceAdded, this->imageListModel, &ImageListModel::addImage);
    connect(this->pipeline.get(), &Pipeline::faceRemoved, this->imageListModel, &ImageListModel::removeImage);
    connect(ui.removeFaceButton, &QPushButton::clicked, this, &CaptureForm::removeFace);
    this->pipeline->loadImageDir();
    updateSwitchButton(ui.segmentButton, "Segment", Operation::Segmentation);
    updateSwitchButton(ui.detectButton, "Classify", Operation::Recognition);
    this->pipeline->start();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CaptureForm::tick);
    timer->start(1000/30);
    unpause();
}

void CaptureForm::saveFace() {
    this->pause();
    auto name = showIndentificationDialog(this);
    if (name.has_value())
        this->pipeline->saveFace(name.value());
    this->unpause();
}

void CaptureForm::removeFace() {
    auto index = ui.listView->currentIndex();
    this->pipeline->removeFace(imageListModel->getName(index));
}

void CaptureForm::tick() {
    auto frame = pipeline->read();
    if (!active)
        return;
    auto size = ui.label->size();
    ui.label->setPixmap(QPixmap::fromImage(convert(frame, size)));
}

void CaptureForm::sendMessage() {
    ControlMessage message;
    message.flags = controlFlags;
    message.boxSize = ui.boxSizeSlider->value();
    pipeline->acceptMessage(message);
}

void CaptureForm::updateActivity(bool checked)
{
    ui.label->setText(checked ? "loading" : "waiting");
    active = checked;
}

void CaptureForm::pause() {
    paused = true;
    updateControlButtons();
}

void CaptureForm::unpause() {
    paused = false;
    updateControlButtons();
}


void CaptureForm::toggleClassification() {
    auto op = Operation::Recognition;
    controlFlags = controlFlags.flip(op);
    updateSwitchButton(ui.detectButton, "Classify", op);
    sendMessage();
}

void CaptureForm::toggleDetection() {
    auto op = Operation::Detection;
    controlFlags = controlFlags.flip(op);
    sendMessage();
}

void CaptureForm::toggleSegmentation() {
    auto op = Operation::Segmentation;
    controlFlags = controlFlags.flip(op);
    updateSwitchButton(ui.segmentButton, "Segmentation", op);
    sendMessage();
}

QString CaptureForm::getFlagColor(bool flag) const {
    if (flag)
        return ui.startButton->palette().base().color().name();
    return ui.stopButton->palette().base().color().name();
}

void CaptureForm::updateSwitchButton(QPushButton* button, const QString& name, Operation op) {
    auto flagValue = controlFlags[op];
    auto color = getFlagColor(flagValue);
    QString qss = QString("background-color: %1").arg(color);
    button->setStyleSheet(qss);
    button->setText(flagValue ? QString("Stop %1").arg(name): QString("Start %1").arg(name));
}

void CaptureForm::updateControlButtons(){
    ui.stopButton->setVisible(!paused);
    ui.startButton->setVisible(paused);
}
