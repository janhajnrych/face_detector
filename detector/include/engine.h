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

class Workflow;
class Dispatcher;


class Engine
{
public:
    explicit Engine();
    void changePreset(const ControlMessage& config);
    void executeOnce(const CmdMessage& config);
    void executeOnce(const CameraMessage& config);
    cv::Mat read();
    void start();
    void terminate();
    ~Engine();
    bool isRunning() const;
    friend class Dispatcher;
    enum class EventType { FaceSaved=0, FaceRemoved=1};
    enum class ProfileType { ReadFps=0, CamFps=1};
    struct Event {
        EventType type;
        cv::Mat image;
        std::string data;
    };
    using Listener = std::function<void(Event)>;
    void listen(EventType eventType, Listener listener);
    void loadDirToDb(std::filesystem::path dirPath);
    void writeStats();
private:
    std::unique_ptr<Workflow> workflow;
    std::unique_ptr<Dispatcher> dispatcher;
    std::thread producerThread, consumerThread, cameraThread, eventThread;
    DataRail<std::tuple<cv::Mat, unsigned>> inputRail;
    DataRail<cv::Mat> outputBuffer;
    using TaskResult = std::tuple<cv::Mat, int>;
    using Task = std::packaged_task<TaskResult(cv::Mat)>;
    DataRail<Task> taskQueue;
    //cv::Mat processFrame(cv::Mat frame, Config config);
    std::atomic_bool finished;
    std::chrono::milliseconds readWaitTime;
    Camera camera;
    std::atomic<unsigned> opFlags, camFlags;
    std::atomic<unsigned> boxSize, searchThreshold;
    std::unordered_map<EventType, Listener> listeners;
    std::unordered_map<ProfileType, std::unique_ptr<Profiler>> profilers;
};


#endif

