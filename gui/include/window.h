#ifndef WINDOW_H
#define WINDOW_H
#include <QMainWindow>
#include <memory>
#include "../include/pipeline.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    virtual ~MainWindow(){ }
    static int runApp(int argc, char* argv[], std::unique_ptr<Pipeline> pipeline);
};


#endif

