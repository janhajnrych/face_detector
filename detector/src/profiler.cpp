#include "../include/profiler.h"


long unsigned Profiler::getTime() {
    auto timestamp = Clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch());
    return ms.count();
}

Profiler::Profiler(unsigned minFps, unsigned maxFps, size_t nBins) {
    if (minFps > maxFps)
        throw Profiler::Exception("min > max");
    histrogram = std::make_unique<Histrogram_t>(minFps, maxFps, nBins);
    startPoint = getTime();
}

Profiler::Profiler(): Profiler(0, 100, 10) {}

void Profiler::measure() {
    std::lock_guard<std::mutex> lock(mutex);
    auto currentTime = getTime();
    timeSpan = currentTime - startPoint;
    startPoint = currentTime;
    if (timeSpan != 0)
        histrogram->add(1000/timeSpan);
}


Profiler::Histrogram_t Profiler::getHistory() const {
    std::lock_guard<std::mutex> lock(mutex);
    return *histrogram;
}

void Profiler::write(std::ofstream& file) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto bins = histrogram->getBins();
    auto probabilities = histrogram->getProbDens<float>();
    std::vector<std::tuple<Histrogram_t::Value_t, float>> rows;
    std::transform(bins.begin(),bins.end(), probabilities.begin(), std::back_inserter(rows), [](const auto& bin, const auto& prob) {
        return std::make_tuple(bin, prob);
    });
    for (const auto& row : rows) {
        file << std::get<0>(row) << "," << std::get<1>(row) << std::endl;
    }
}

