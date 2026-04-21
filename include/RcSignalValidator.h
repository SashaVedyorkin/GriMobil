#pragma once

#include <Arduino.h>

#include "RcInputController.h"

struct RcChannelState {
  unsigned long ch1;
  unsigned long ch2;
};

class RcSignalValidator {
public:
  RcSignalValidator(unsigned long signalTimeoutInMicroSec = 100000,
                    unsigned long signalMinInMicroSec = 900,
                    unsigned long signalMaxInMicroSec = 2200);

  RcChannelState validate(const RcChannelSnapshot& snapshot) const;

private:
  unsigned long signalTimeoutInMicroSec_;
  unsigned long signalMinInMicroSec_;
  unsigned long signalMaxInMicroSec_;

  unsigned long validateChannel(unsigned long pulseInMicroSec,
                                unsigned long pulseAgeInMicroSec) const;
};
