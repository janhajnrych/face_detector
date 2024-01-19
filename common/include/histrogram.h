#ifndef HISTROGRAM_H
#define HISTROGRAM_H
#include <vector>
#include <algorithm>
#include <numeric>
#include <math.h>
#include <tuple>


template<class ValueType, class CountType = unsigned long>
class Histrogram
{
public:
    static_assert(std::is_arithmetic<ValueType>::value, "wrong histrogram count type");
    static_assert(std::is_arithmetic<CountType>::value, "wrong histrogram value type");

    using Value_t = ValueType;
    using Count_t = CountType;

    Histrogram(ValueType minVal, ValueType maxVal, size_t nBins): minVal(minVal), maxVal(maxVal) {
        counts = std::vector<CountType>(nBins, 0);//.insert(counts.begin(), nBins, 0);
        counts.shrink_to_fit();
    }

    template <class T>
    void add(const T& value) {
        static_assert(std::is_arithmetic<T>::value, "incompatible type");
        auto convertedValue = static_cast<Value_t>(value);
        size_t binId = static_cast<size_t>(counts.size() * (convertedValue - minVal)/(maxVal - minVal));
        if (binId < 0 || binId > counts.size())
            return;
        counts[binId]++;
        countSum++;
    }

    template <class T>
    std::vector<T> getProbDens(T normRange = 1.0) const {
        static_assert(std::is_floating_point<T>::value, "wrong probability type");
        std::vector<T> probability;
        if (countSum == 0)
            return probability;
        std::transform(counts.begin(), counts.end(),std::back_inserter(probability), [this, normRange](const auto& item){
            return normRange * item/countSum;
        });
        return probability;
    }

    const std::vector<CountType>& getCounts() const {
        return counts;
    }

    ValueType getMin() const {
        return minVal;
    }

    ValueType getMax() const {
        return maxVal;
    }

    size_t size() const {
        return counts.size();
    }

    std::vector<ValueType> getBins() const {
        std::vector<ValueType> bins(counts.size());
        //std::iota(bins.begin(), bins.end(), 0);
        auto counter = minVal;
        auto step = (maxVal - minVal)/static_cast<ValueType>(counts.size());
        std::generate(bins.begin(), bins.end(), [&counter, step] () mutable {
            counter+=step;
            return counter;
        });
        return bins;
    }

    void reset() {
        std::fill(counts.begin(), counts.end(), 0);
    }

private:
    std::vector<CountType> counts;
    ValueType minVal;
    ValueType maxVal;
    CountType countSum = 0;
};


#endif

