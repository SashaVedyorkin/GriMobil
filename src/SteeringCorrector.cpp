#include "SteeringCorrector.h"

namespace {
bool isNeutralSample(unsigned long pulseInMicroSec) {
  unsigned long centerPulseInMicroSec = AppConfig::Steering::kInputCenterInMicroSec;
  unsigned long neutralWindowInMicroSec = AppConfig::Steering::kCorrectorNeutralWindowInMicroSec;
  unsigned long deltaFromCenterInMicroSec =
      pulseInMicroSec > centerPulseInMicroSec ? pulseInMicroSec - centerPulseInMicroSec
                                              : centerPulseInMicroSec - pulseInMicroSec;

  return deltaFromCenterInMicroSec <= neutralWindowInMicroSec;
}
}  // namespace

SteeringCorrector::SteeringCorrector() : sampleCount_(0), nextIndex_(0) {
  reset();
}

void SteeringCorrector::reset() {
  for (uint8_t sampleIndex = 0; sampleIndex < AppConfig::Steering::kCorrectorSampleCount;
       ++sampleIndex) {
    samples_[sampleIndex] = AppConfig::Steering::kInputCenterInMicroSec;
  }

  sampleCount_ = 0;
  nextIndex_ = 0;
}

unsigned long SteeringCorrector::apply(unsigned long pulseInMicroSec) {
  samples_[nextIndex_] = pulseInMicroSec;
  nextIndex_ = (nextIndex_ + 1) % AppConfig::Steering::kCorrectorSampleCount;

  if (sampleCount_ < AppConfig::Steering::kCorrectorSampleCount) {
    ++sampleCount_;
  }

  uint8_t neutralSampleCount = 0;

  for (uint8_t sampleIndex = 0; sampleIndex < sampleCount_; ++sampleIndex) {
    if (isNeutralSample(samples_[sampleIndex])) {
      ++neutralSampleCount;
    }
  }

  if (neutralSampleCount >= AppConfig::Steering::kCorrectorNeutralMajorityCount) {
    return AppConfig::Steering::kInputCenterInMicroSec;
  }

  return pulseInMicroSec;
}
