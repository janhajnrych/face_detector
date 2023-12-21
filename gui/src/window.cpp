#include "../include/window.h"
#include <QPalette>
#include "../include/form.h"
#include "../include/pipeline.h"

MainWindow::MainWindow()
{
    auto palette = QApplication::palette();
    palette.setColor(QPalette::Window, QColor(61, 56, 70));
    setPalette(palette);
    setAutoFillBackground(true);
}

int MainWindow::runApp(int argc, char* argv[], std::unique_ptr<Pipeline> pipeline) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(650, 500);
    window.layout()->setContentsMargins(5, 5, 5, 5);
    auto form = new CaptureForm(std::move(pipeline), &window);
    window.layout()->addWidget(form);
    window.setLayout(layout);
    window.show();
    return app.exec();
}
