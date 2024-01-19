#include "../include/window.h"
#include <QPalette>
#include <QBoxLayout>
#include "../include/form.h"
#include "../include/pipeline.h"
#include "../common/include/version.h"

MainWindow::MainWindow()
{
    auto palette = QApplication::palette();
    palette.setColor(QPalette::Window, QColor(61, 56, 70));
    setPalette(palette);
    setAutoFillBackground(true);
    auto version = BuildInfo::getVersion();
    setWindowTitle(QString("Face Detector - version %1").arg(QString::fromStdString(version)));
}

int MainWindow::runApp(int argc, char* argv[], std::unique_ptr<Pipeline> pipeline) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(580, 920);
    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, &window);
    layout->setContentsMargins(5, 5, 5, 5);
    auto form = new CaptureForm(std::move(pipeline), &window);
    layout->addWidget(form, 0, Qt::AlignCenter);
    window.setLayout(layout);
    window.show();
    return app.exec();
}
