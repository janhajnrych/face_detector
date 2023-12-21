#include <iostream>
#include <memory>
#include <string>
#include "gui/include/pipeline.h"
#include "detector/include/detector.h"
#include "gui/include/window.h"



class ImagePipeline: public Pipeline {
public:
    ImagePipeline(const std::string& cascadePath){
        detector = ImagePipeline::createDetector(cascadePath);
    }
    Image execute(Image image) {
        if (!detector)
            return image;
        cv::Mat cvImage(image.height, image.width, CV_8UC3, image.data, image.bytesPerLine);
        cvImage = detector->drawFaces(cvImage);
        image.data = cvImage.data;
        return image;
    }
private:
    std::unique_ptr<Detector> detector;

    static std::unique_ptr<Detector> createDetector(const std::string& cascadePath) {
        std::unique_ptr<Detector> detector;
        try {
            detector = std::make_unique<Detector>(cascadePath);
        }
        catch (DetectionException exception) {
            detector = nullptr;
            std::cerr << exception.what() << std::endl;
        }
        return detector;
    }
};




int main(int argc, char* argv[])
{
    std::string cascadePath = "haarcascade_frontalface_default.xml";
    MainWindow::runApp(argc, argv, std::make_unique<ImagePipeline>(cascadePath));
    return 0;
}
