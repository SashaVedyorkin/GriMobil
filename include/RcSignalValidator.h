#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "RcInputController.h"

struct RcChannelState {
  unsigned long ch1;
  unsigned long ch2;
  unsigned long ch3;
  unsigned long ch4;
};

class RcSignalValidator {
public:
  RcSignalValidator(
      unsigned long signalTimeoutInMicroSec = AppConfig::Rc::kSignalTimeoutInMicroSec,
      unsigned long signalMinInMicroSec = AppConfig::Rc::kSignalMinInMicroSec,
      unsigned long signalMaxInMicroSec = AppConfig::Rc::kSignalMaxInMicroSec);

  RcChannelState validate(const RcChannelSnapshot& snapshot);

private:
  unsigned long signalTimeoutInMicroSec_;
  unsigned long signalMinInMicroSec_;
  unsigned long signalMaxInMicroSec_;
  unsigned long lastValidCh1InMicroSec_;
  unsigned long lastValidCh2InMicroSec_;
  unsigned long lastValidCh3InMicroSec_;
  unsigned long lastValidCh4InMicroSec_;

  unsigned long validateChannel(unsigned long pulseInMicroSec,
                                unsigned long pulseAgeInMicroSec,
                                unsigned long& lastValidPulseInMicroSec);
};
