#ifndef PROFILER_H
#define PROFILER_H
#include <memory>
#include <tuple>
#include <mutex>
#include <chrono>
#include "../../common/include/histrogram.h"
#include <atomic>
#include "component.h"
#include "exception.h"
#include <fstream>


class Profiler: public Component
{
public:
    using Exception = ComponentException<Profiler>;
    using Histrogram_t = Histrogram<unsigned>;
    explicit Profiler();
    explicit Profiler(unsigned maxFps, unsigned minFps, size_t nBins);
    explicit Profiler(const Profiler& other) = delete;
    void measure();
    Histrogram_t getHistory() const;
    Profiler& operator =(const Profiler& other) = delete;
    void write(std::ofstream& file) const;
private:
    std::unique_ptr<Histrogram_t> histrogram;
    using Clock = std::chrono::high_resolution_clock;
    long unsigned startPoint;
    long unsigned timeSpan;
    static long unsigned getTime();
    mutable std::mutex mutex;
};


#endif

