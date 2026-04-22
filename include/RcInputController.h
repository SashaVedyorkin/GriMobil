#pragma once

#include <Arduino.h>
#include "RcChannelReadStrategy.h"

struct RcChannelSnapshot {
  unsigned long ch1PulseInMicroSec;
  unsigned long ch2PulseInMicroSec;
  unsigned long ch3PulseInMicroSec;
  unsigned long ch1PulseAgeInMicroSec;
  unsigned long ch2PulseAgeInMicroSec;
  unsigned long ch3PulseAgeInMicroSec;
};

class RcInputController {
public:
  RcInputController(uint8_t ch1Pin, uint8_t ch2Pin, uint8_t ch3Pin);

  void begin();
  RcChannelSnapshot read();

private:
  InterruptChannelReadStrategy ch1Strategy_;
  InterruptChannelReadStrategy ch2Strategy_;
  PulseInChannelReadStrategy ch3Strategy_;
};
