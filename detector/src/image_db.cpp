#include <exception>
#include "../include/image_db.h"
#include <opencv2/opencv.hpp>
#include <ranges>

void ImageDb::addImage(const std::string& name, cv::Mat embedding) {
    Item item;
    item.name = name;
    nextId++;
    item.id = nextId;
    //item.path = name + saveExt;
    //cv::imwrite(item.path, image);
    item.embedding = embedding;
    items.push_back(item);
}

std::optional<ImageDb::Item> ImageDb::findById(unsigned id) const {
    auto it = std::find_if(items.begin(), items.end(), [id](auto item){
        return item.id == id;
    });
    if (it == items.end())
        return std::optional<ImageDb::Item>();
    return *it;
}

void ImageDb::removeAt(unsigned index) {
    auto it = items.begin();
    std::advance(it, index);
    items.erase(it);
}

bool ImageDb::removeByName(const std::string& name) {
    auto it = std::find_if(items.begin(), items.end(), [name](auto item){
        return item.name == name;
    });
    if (it == items.end())
        return false;
    items.erase(it);
    return true;
}

size_t ImageDb::size() const {
    return items.size();
}

void ImageDb::clear() {
    items.clear();
}

std::optional<ImageDb::Item> ImageDb::findSimilar(cv::Mat refEmbedding) const {
    auto it = std::min_element(items.begin(), items.end(), [refEmbedding](auto a, auto b){
        return cv::norm(refEmbedding - a.embedding) < cv::norm(refEmbedding - b.embedding);
    });
    if (it == items.end()) {
        return std::optional<ImageDb::Item>();
    }
    return *it;
}

Eigen::VectorXf ImageDb::getDistances(cv::Mat refEmbedding) const {
    Eigen::VectorXf result = Eigen::VectorXf::Zero(items.size());
    for (unsigned i=0; i<items.size(); i++){
        result[i] = cv::norm(refEmbedding - operator[](i).embedding);
    };
    return result;
}

void ImageDb::getScores(Eigen::VectorXf& result, cv::Mat refEmbedding, float threshold) const {
    for (unsigned i=0; i<items.size(); i++){
        auto dist = cv::norm(refEmbedding - operator[](i).embedding);
        if (dist < threshold)
            result[i] = 1.0/(dist+1e-6);
    };
}

Eigen::VectorXd ImageDb::getIndices() const {
    Eigen::VectorXd result = Eigen::VectorXd::Zero(items.size());
    for (unsigned i=0; i<items.size(); i++){
        result[i] = operator[](i).id;
    };
    return result;
}

ImageDb::Item ImageDb::operator[](unsigned index) const {
    auto it = items.begin();
    std::advance(it, index);
    return *it;
}

