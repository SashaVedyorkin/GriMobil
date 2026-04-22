#pragma once

#include <Arduino.h>

#include "AppConfig.h"

struct RcChannelMeasurement {
  unsigned long pulseInMicroSec;
  unsigned long pulseAgeInMicroSec;
};

class RcChannelReadStrategy {
public:
  virtual ~RcChannelReadStrategy() {}

  virtual void begin() = 0;
  virtual RcChannelMeasurement read() = 0;
};

class InterruptChannelReadStrategy : public RcChannelReadStrategy {
public:
  InterruptChannelReadStrategy(uint8_t pin, uint8_t slot);

  void begin() override;
  RcChannelMeasurement read() override;
  void handleInterrupt();

private:
  uint8_t pin_;
  uint8_t slot_;
  volatile unsigned long riseInMicroSec_;
  volatile unsigned long pulseInMicroSec_;
  volatile unsigned long lastPulseInMicroSec_;
};

class PulseInChannelReadStrategy : public RcChannelReadStrategy {
public:
  explicit PulseInChannelReadStrategy(uint8_t pin);

  void begin() override;
  RcChannelMeasurement read() override;

private:
  uint8_t pin_;
  unsigned long pulseInMicroSec_;
  unsigned long lastPulseInMicroSec_;
};
