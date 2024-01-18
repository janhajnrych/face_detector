#ifndef COMPONENT_H
#define COMPONENT_H
#include <exception>
#include <string>

class Component
{
public:
    template <class T>
    static const char* getClassName() noexcept {
        return typeid(T).name();
    }
};


#endif

