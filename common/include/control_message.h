#ifndef CONTROLMESSAGE_H
#define CONTROLMESSAGE_H
#include <bitset>
#include <optional>
#include <string>


enum ImageOperation { Detection=0, Recognition=1, Segmentation=2, Stablization=3};
enum CameraOperation { PauseCamera=0, UnpauseCamera=1};
enum DbOperation { SaveFaceToDb=0, RemoveFaceFromDb=1};

using ControlFlags = std::bitset<4>;
using DbFlags = std::bitset<2>;
using CameraFlags = std::bitset<2>;

struct ControlMessage {
    ControlFlags opFlags = {0b0000};
    unsigned boxSize = 100;

};

struct CmdMessage {
    DbFlags dbFlags = {0b00};
    std::optional<std::string> filename;
};

struct CameraMessage {
    bool paused;
};


#endif

