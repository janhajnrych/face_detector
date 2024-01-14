#include <iostream>
#include <memory>
#include <string>
#include "../include/pipeline.h"
#include "../../gui/include/window.h"


int main(int argc, char* argv[]) {
    MainWindow::runApp(argc, argv, std::make_unique<ImagePipeline>(std::filesystem::current_path()));
    return 0;
}
