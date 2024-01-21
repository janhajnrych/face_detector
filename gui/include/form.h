#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H
#include "ui_form.h"
#include <QImage>
#include <vector>
#include <memory>
#include "pipeline.h"
#include "../include/image_list_model.h"
#include <unordered_map>
#include <QTimer>
#include "../../common/include/control_message.h"


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
    void toggleClassification();
    void toggleSegmentation();
    void toggleDetection();
    void tick();
    void saveFace();
    void removeFace();
    void onBoxSizeChanged(int value);
    void toggleStabilization();
private:
    Ui_MainForm ui;
    bool active = false;
    bool paused = false;
    bool segmentation = false;
    bool recognition = false;
    std::shared_ptr<Pipeline> pipeline;
    void updateControlButtons();
    ImageListModel* imageListModel;
    void updateSwitchButton(QPushButton* button, const QString& name, bool flagValue);
    QString getFlagColor(bool flag) const;
    QTimer* timer;
};


#endif

