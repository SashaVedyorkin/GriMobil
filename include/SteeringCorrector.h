#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class SteeringCorrector {
public:
  SteeringCorrector();

  void reset();
  unsigned long apply(unsigned long pulseInMicroSec);

private:
  unsigned long samples_[AppConfig::Steering::kCorrectorSampleCount];
  uint8_t sampleCount_;
  uint8_t nextIndex_;
};
