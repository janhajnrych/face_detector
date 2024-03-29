#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <exception>
#include "../include/workflow.h"
#include <type_traits>
#include <iostream>
#include "../include/detector.h"
#include "../include/image_db.h"
#include "../include/classifier.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "../include/image_db.h"

#include <iostream>

namespace {

    template <class T>
    std::unique_ptr<T> createDetector(const std::string& cascadePath) {
        static_assert(std::is_base_of<FaceDetector, T>::value, "wrong detector type");
        std::unique_ptr<T> detector;
        try {
            detector = std::make_unique<T>(cascadePath);
        }
        catch (FaceDetector::ModelException exception) {
            detector = nullptr;
            std::cerr << exception.what() << std::endl;
        }
        return detector;
    }

    inline cv::Point getRectCenter(const cv::Rect& rect){
        return cv::Point((rect.br() + rect.tl())/2);
    }


}


Workflow::Workflow():
    faceDetector(createDetector<FaceDetector>("../data/haarcascade_frontalface_default.xml")),
    classifier(std::make_unique<Classifier>("../data/classifier.onnx")),
    segmenter(std::make_unique<Segmenter>("../data/segmentation.onnx")),
    stabilizer(std::make_unique<Stabilizer>()),
    imageDb(std::make_unique<ImageDb>())
{
    resetNameScores();
}

void Workflow::setTimeStep(float dt) {
    stabilizer->setTimeStep(dt);
}


void Workflow::resetNameScores() {
    nameScores.scores = Eigen::VectorXf::Constant(imageDb->size(), 0.0);
    nameScores.indices = imageDb->getIndices();
    nameScores.scoreBuffer = Eigen::VectorXf::Constant(imageDb->size(), 0.0);
}

void Workflow::detectFaces(cv::Mat frame, float relativeBoxSize) {
    detections.clear();
    faceDetector->detect(frame, detections);
    for (auto& face : detections) {
        cv::Point center = getRectCenter(face);
        face.width = static_cast<int>(face.width * relativeBoxSize);
        face.height = static_cast<int>(face.height * relativeBoxSize);
        cv::Point newCenter = getRectCenter(face);
        face.x-= newCenter.x - center.x;
        face.y-= newCenter.y - center.y;
    }
}

cv::Mat Workflow::drawText(cv::Mat image, cv::Point pos, const std::string& text) const {
    cv::putText(image, text, pos, cv::FONT_HERSHEY_DUPLEX, 1.0, defaultColor, 2);
    return image;
}

cv::Rect Workflow::clipRect(cv::Size size, cv::Rect rect) {
    cv::Range cols(std::max(rect.tl().x, 0), std::min(rect.br().x, size.width));
    cv::Range rows(std::max(rect.tl().y, 0), std::min(rect.br().y, size.height));
    return cv::Rect(cols.start, rows.start, cols.end - cols.start, rows.end - rows.start);
}

cv::Mat Workflow::getSlice(cv::Mat image, cv::Rect rect) {
    return image(clipRect(image.size(), rect));
}

cv::Mat Workflow::drawFaceRects(cv::Mat image) {
    static constexpr double alpha = 0.33;
    cv::Mat overlay;
    image.copyTo(overlay);
    auto largest_iter = detections.end();
    largest_iter = std::max_element(detections.begin(), detections.end(), [](auto a, auto b){
        return a.area() < b.area();
    });
    stabilizer->setHint(*largest_iter);
    for(auto it = detections.begin(); it != detections.end(); ++it) {
        auto rect = *it;
        cv::rectangle(image, rect.tl(), rect.br(), defaultColor, 4);
        cv::rectangle(overlay, rect.tl(), rect.br(), defaultColor, -1);
        cv::addWeighted(overlay, alpha, image, 1 - alpha, 0, image);
    }
    return image;
}

cv::Mat Workflow::drawFaceRects(cv::Mat image, float trackPeriod) {
    static constexpr double alpha = 0.33;
    cv::Mat overlay;
    image.copyTo(overlay);
    auto largest_iter = detections.end();
    largest_iter = std::max_element(detections.begin(), detections.end(), [](auto a, auto b){
        return a.area() < b.area();
    });
    for(auto it = detections.begin(); it != detections.end(); ++it) {
        auto rect = *it;
        if (it == largest_iter) {
            auto stabilizedRect = stabilizer->stabilize(rect, trackPeriod);
            cv::rectangle(image, stabilizedRect.tl(), stabilizedRect.br(), stabilizedColor, 4);
            cv::rectangle(overlay, stabilizedRect.tl(), stabilizedRect.br(), stabilizedColor, -1);
        }
        else {
            cv::rectangle(image, rect.tl(), rect.br(), defaultColor, 4);
            cv::rectangle(overlay, rect.tl(), rect.br(), defaultColor, -1);
        }
        cv::addWeighted(overlay, alpha, image, 1 - alpha, 0, image);
    }
    return image;
}

static inline size_t argmax(Eigen::VectorXf vector){
    float maxElem = 0.0;
    size_t maxIndex = 0;
    for (size_t i=0; i<vector.rows(); i++){
        if (vector[i] > maxElem){
            maxIndex = i;
            maxElem = vector[i];
        }
    }
    return maxIndex;
}

cv::Mat Workflow::drawFaceNames(cv::Mat image, float offset) {
    if (nameScores.indices.rows() == 0)
        return image;
    static float updateFactor = 0.25;
    auto embedding = classifier->getEmbedding(image);
    for (auto face: getLargerFaces(128, 64)){
        imageDb->getScores(nameScores.scoreBuffer, embedding, 2.0);
        nameScores.scores = nameScores.scores * (1.f - updateFactor) + updateFactor * nameScores.scoreBuffer;
        auto index = nameScores.indices[argmax(nameScores.scores)];
        auto searchResult = imageDb->findById(index);
        if (!searchResult.has_value())
            continue;
        const auto& item = searchResult.value();
        auto x = face.tl().x;
        auto y = 0.5*(face.tl().y + face.br().y);
        auto pos = cv::Point(x, y + face.height * offset);
        drawText(image, pos, item.name);
    }
    return image;
}

cv::Rect Workflow::getLargestRect(const std::vector<cv::Rect>& faces) {
    auto iter = std::max_element(faces.begin(), faces.end(), [](auto a, auto b){
        return a.area() < b.area();
    });
    return *iter;
}

const std::vector<cv::Rect>& Workflow::getAllFaces() const {
    return detections;
}

std::vector<cv::Rect> Workflow::getLargerFaces(unsigned minArea) const {
    std::vector<cv::Rect> results;
    std::copy_if(detections.begin(), detections.end(), std::back_inserter(results), [minArea](cv::Rect rect){
        return rect.area() >= minArea;
    });
    return results;
}

std::vector<cv::Rect> Workflow::getLargerFaces(unsigned minArea, size_t maxNumber) const {
    std::vector<cv::Rect> results;
    std::copy_if(detections.begin(), detections.end(), std::back_inserter(results), [minArea](cv::Rect rect){
        return rect.area() >= minArea;
    });
    auto comparator = [](cv::Rect left, cv::Rect right) {
        return left.area() < right.area();
    };
    std::priority_queue<cv::Rect, std::vector<cv::Rect>, decltype(comparator)> queue(results.begin(), results.end(), comparator);
    results.clear();
    while (!queue.empty()) {
        results.push_back(std::move(const_cast<cv::Rect&>(queue.top())));
        queue.pop();
    }
    return results;
}


bool Workflow::saveFace(cv::Mat face, const std::string& name) {
    imageDb->addImage(name, classifier->getEmbedding(face));
    cv::imwrite(getFilename(name), face);
    resetNameScores();
    return true;
}

cv::Mat Workflow::loadFace(std::filesystem::path path, const std::string& name) {
    auto face = cv::imread(path.string());
    imageDb->addImage(name, classifier->getEmbedding(face));
    resetNameScores();
    return face;
}

cv::Mat Workflow::segment(cv::Mat frame) {
    if (detections.size() == 0)
        return frame;
    const static float alpha = 0.25;
    auto face = getLargestRect(detections);
    face = clipRect(frame.size(), face);
    auto slice = getSlice(frame, face);
    auto masks = segmenter->calcMask(slice);
    auto segm = Segmenter::filterClasses(masks, Segmenter::SegClass::Face);
    cv::Mat resized;
    cv::resize(slice, resized, segmenter->size, cv::INTER_NEAREST);
    cv::Mat masked;
    cv::Mat overlay(resized.cols, resized.rows, CV_8UC3, cv::Scalar(255, 0, 0));
    cv::Mat temp;
    cv::addWeighted(overlay, alpha, resized, 1 - alpha, 0, temp);
    temp.copyTo(masked, segm);
    cv::Mat invertedMask;
    cv::bitwise_not(segm, invertedMask);
    resized.copyTo(masked, invertedMask);
    cv::Mat out;
    cv::resize(masked, out, slice.size(), cv::INTER_AREA);
    out.copyTo(frame(face));
    return frame;
}

std::string Workflow::getFilename(const std::string& name) const {
    return name + defaultSaveExt;
}

bool Workflow::removeFace(const std::string &name) {
    auto result = imageDb->removeByName(name);
    if (result)
        std::filesystem::remove(getFilename(name));
    resetNameScores();
    return result;
}

std::optional<cv::Mat> Workflow::getLargestFace(cv::Mat frame) const {
    auto rect = getLargestRect(detections);
    return getSlice(frame, rect);
}
