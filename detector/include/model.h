#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <concepts>
#include "component.h"
#include "exception.h"
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/objdetect/objdetect.hpp>


template <typename T>
concept Loadable = std::is_constructible<T>::value && std::is_copy_constructible<T>::value;

template<Loadable T>
class Model: public Component {
public:
    explicit Model<T>() = delete;
    explicit Model<T>(const Model<T>&) = delete;
};


template <>
struct Model<cv::dnn::Net>: public Component {
public:
    using ModelException = ComponentException<Model<cv::dnn::Net>>;
protected:
    cv::dnn::Net model;
    void load(const std::string& path) {
        try {
            model = cv::dnn::readNetFromONNX(path);
        }
        catch (cv::Exception& e) {
            throw ModelException(std::string("Not loaded! ") + std::string(e.what()));
        }
        if (model.empty())
            throw ModelException(std::string("Model is empty!"));
    }
};

template <>
struct Model<cv::CascadeClassifier>: public Component {
public:
    using ModelException = ComponentException<Model<cv::CascadeClassifier>>;
protected:
    cv::CascadeClassifier model;
    void load(const std::string& path) {
        try {
            model = cv::CascadeClassifier(path);
        }
        catch (cv::Exception& e) {
            throw ModelException(std::string("Not loaded! ") + std::string(e.what()));
        }
        if (model.empty())
            throw ModelException(std::string("Model is empty!"));
    }
};


#endif

