#ifndef RAIL_H
#define RAIL_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <class T, class Container = std::deque<T>>
class DataRail
{
public:
    DataRail(): queue(), mutex(), condition() {}

    ~DataRail(){}

    void write(T t)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(t);
        }
        condition.notify_one();
    }

    void writeMove(T&& t)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(t));
        }
        condition.notify_one();
    }

    T read()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&](){ return !queue.empty(); });
        T val = queue.front();
        queue.pop();
        return val;
    }

    T readMove()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&](){ return !queue.empty(); });
        auto val = std::move(queue.front());
        queue.pop();
        return val;
    }


    T front()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&](){ return !queue.empty(); });
        T val = queue.front();
        return val;
    }

    std::optional<T> readWithTimeout(std::chrono::milliseconds waitTime) {
        std::unique_lock<std::mutex> lock(mutex);
        auto stopped = !condition.wait_for(lock, waitTime, [&](){ return !queue.empty(); });
        if (stopped)
            return std::optional<T>();
        T val = queue.front();
        queue.pop();
        return val;
    }

    size_t size() {
        return queue.size();
    }

private:
    std::queue<T, Container> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;
};


#endif
