#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <string>
#include "component.h"


template <class ParentClass>
class ComponentException: public std::exception
{
public:
    static_assert(std::is_base_of<Component, ParentClass>::value, "wrong parent class of exception type");
    explicit ComponentException(const std::string& message): message(message) {    }

    virtual ~ComponentException() noexcept {}

    virtual const char* what() const noexcept {
        return getFullMessage().c_str();
    }
protected:
    std::string message;
private:
    std::string getFullMessage() const noexcept {
        return std::string(Component::getClassName<ParentClass>()) + std::string(": ") + std::string(message);
    }
};


#endif

