#ifndef IMAGEDB_H
#define IMAGEDB_H
#include <string>
#include <list>
#include "detector.h"
#include <optional>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <opencv2/core.hpp>
#include "classifier.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <Eigen/Dense>


class Classifier;

class ImageDb
{
public:
    struct Item {
        unsigned id;
        std::string name;
        cv::Mat embedding;
    };
    void addImage(const std::string& name, cv::Mat embedding);
    std::optional<Item> findById(unsigned id) const;
    void removeAt(unsigned index);
    bool removeByName(const std::string& name);
    std::optional<Item> findSimilar(cv::Mat refEmbeddings) const;
    Eigen::VectorXf getDistances(cv::Mat refEmbeddings) const;
    void getScores(Eigen::VectorXf& result, cv::Mat refEmbedding, float threshold) const;
    Eigen::VectorXd getIndices() const;
    size_t size() const;
    void clear();
    Item operator[](unsigned index) const;
private:
    std::list<Item> items;
    unsigned nextId = 0;
};


#endif

