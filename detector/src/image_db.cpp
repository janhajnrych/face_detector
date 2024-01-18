#include <exception>
#include "../include/image_db.h"
#include <opencv2/opencv.hpp>

void ImageDb::addImage(const std::string& name, cv::Mat embedding) {
    Item item;
    item.name = name;
    item.id = items.size();
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

ImageDb::Item ImageDb::operator[](unsigned index) const {
    auto it = items.begin();
    std::advance(it, index);
    return *it;
}

