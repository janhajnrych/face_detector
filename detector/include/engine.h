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
#include "command.h"
#include <variant>


class Workflow;
class Dispatcher;


class Engine
{
public:
    using Config = ControlMessage;
    explicit Engine();
    void changePreset(const Config& config);
    void executeOnce(const Config& config);
    cv::Mat read();
    void start();
    void terminate();
    ~Engine();
    bool isRunning() const;
    friend class Dispatcher;
    enum class EventType { FaceSaved=0, FaceRemoved=1};
    struct Event {
        EventType type;
        cv::Mat image;
        std::string data;
    };
    using Listener = std::function<void(Event)>;
    void listen(EventType eventType, Listener listener);
    void loadDirToDb(std::filesystem::path dirPath);
private:
    std::unique_ptr<Workflow> workflow;
    std::unique_ptr<Dispatcher> dispatcher;
    std::thread producerThread, consumerThread, cameraThread;
    DataRail<std::tuple<cv::Mat, unsigned>> inputRail;
    DataRail<cv::Mat> outputBuffer;
    using TaskResult = std::tuple<cv::Mat, int>;
    using Task = std::packaged_task<TaskResult(cv::Mat)>;
    DataRail<Task> taskQueue;
    //cv::Mat processFrame(cv::Mat frame, Config config);
    std::atomic_bool finished;
    std::chrono::milliseconds readWaitTime;
    Camera camera;
    std::atomic<unsigned> flags;
    std::atomic<unsigned> boxSize;
    std::unordered_map<EventType, Listener> listeners;

};


#endif

