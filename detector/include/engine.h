#ifndef ENGINE_H
#define ENGINE_H
#include <memory>
#include <thread>
#include <opencv2/core.hpp>
#include "../include/rail.h"
#include <tuple>
#include <atomic>
#include <bitset>
#include <queue>
#include <future>
#include <atomic>
#include "camera.h"
#include "../../common/include/control_message.h"
#include <chrono>
#include <variant>
#include "profiler.h"
#include <filesystem>
#include <variant>


class Workflow;
class Context;

class Engine
{
public:
    explicit Engine();
    void changePreset(const ControlMessage& config);
    void executeOnce(const CmdMessage& config);
    void executeOnce(const CameraMessage& config);
    ControlMessage getPreset() const;
    cv::Mat read();
    void start();
    void terminate();
    ~Engine();
    bool isRunning() const;
    friend class Dispatcher;
    enum class EventType { FaceSaved=0, FaceRemoved=1};
    enum class ProfileType { ReadFps=0, CamFps=1};
    struct SaveEventData {
        cv::Mat image;
        std::string filename;
    };
    struct RemoveEventData {
        std::string filename;
    };
    struct Event {
        EventType type;
        using DataType = std::variant<SaveEventData, RemoveEventData>;
        DataType data;
        Event() = default;
        Event(EventType type, DataType data): type(type), data(data) {}
    };

    using Listener = std::function<void(Event)>;
    void listen(EventType eventType, Listener listener);
    void loadDirToDb(std::filesystem::path dirPath);
    void writeStats();
private:
    std::unique_ptr<Context> context;
    std::unique_ptr<Workflow> workflow;
    std::thread producerThread, consumerThread, cameraThread;
    DataRail<cv::Mat> inputRail;
    DataRail<cv::Mat> outputBuffer;
    using TaskResult = bool;
    using Task = std::packaged_task<TaskResult(Workflow&, Context&, cv::Mat&)>;
    std::queue<Task> taskQueue;
    std::atomic_bool finished;
    std::chrono::milliseconds readWaitTime;
    Camera camera;
    std::unordered_map<EventType, Listener> listeners;
    std::unordered_map<ProfileType, std::unique_ptr<Profiler>> profilers;
    mutable std::mutex mutex;
    void execCommands(cv::Mat frame);
};


#endif

