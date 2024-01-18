#ifndef CONTROLMESSAGE_H
#define CONTROLMESSAGE_H
#include <bitset>
#include <optional>
#include <string>


enum Operation { Detection=0, Recognition=1, Segmentation=2, SaveFaceToDb=3, RemoveFaceFromDb=4};
using ControlFlags = std::bitset<5>;

struct ControlMessage {
    ControlFlags flags = {0b00000};
    unsigned boxSize = 100;
    std::optional<std::string> filename;
};


#endif

