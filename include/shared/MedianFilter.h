#pragma once

#include <Arduino.h>

template <size_t SampleCount>
class MedianFilter {
public:
  MedianFilter() : sampleCount_(0), nextIndex_(0) {}

  void reset() {
    sampleCount_ = 0;
    nextIndex_ = 0;
  }

  unsigned long apply(unsigned long sample) {
    if (sample == 0) {
      reset();
      return 0;
    }

    samples_[nextIndex_] = sample;
    nextIndex_ = (nextIndex_ + 1) % SampleCount;

    if (sampleCount_ < SampleCount) {
      ++sampleCount_;
    }

    unsigned long sortedSamples[SampleCount];
    for (size_t sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex) {
      sortedSamples[sampleIndex] = samples_[sampleIndex];
    }

    for (size_t leftIndex = 0; leftIndex < sampleCount_; ++leftIndex) {
      for (size_t rightIndex = leftIndex + 1; rightIndex < sampleCount_; ++rightIndex) {
        if (sortedSamples[rightIndex] < sortedSamples[leftIndex]) {
          unsigned long temp = sortedSamples[leftIndex];
          sortedSamples[leftIndex] = sortedSamples[rightIndex];
          sortedSamples[rightIndex] = temp;
        }
      }
    }

    return sortedSamples[sampleCount_ / 2];
  }

private:
  unsigned long samples_[SampleCount];
  size_t sampleCount_;
  size_t nextIndex_;
};
