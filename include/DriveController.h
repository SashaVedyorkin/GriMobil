#pragma once

#include <Arduino.h>

#include "AppConfig.h"

struct DriveUpdateResult {
  const char* statusMessage;
  unsigned long escPulseInMicroSec;
  bool isSignalValid;
};

class DriveController {
public:
  DriveController();

  DriveUpdateResult update(unsigned long ch1PulseInMicroSec, unsigned long ch3PulseInMicroSec);

private:
  enum DriveMode {
    kNeutral,
    kForward,
    kReverse
  };

  enum SwitchZone {
    kDown,
    kCenter,
    kUp
  };

  DriveMode driveMode_;
  SwitchZone lastSwitchZone_;
  unsigned long lastEscPulseInMicroSec_;

  SwitchZone detectSwitchZone(unsigned long ch3PulseInMicroSec) const;
  void updateDriveMode(SwitchZone currentSwitchZone);
  unsigned long mapThrottleToEsc(unsigned long ch1PulseInMicroSec) const;
  unsigned long quantizeEscPulse(unsigned long targetEscPulseInMicroSec) const;
  unsigned long applyEscHysteresis(unsigned long targetEscPulseInMicroSec) const;
  unsigned long smoothEscPulse(unsigned long targetEscPulseInMicroSec) const;
  const char* getStatusMessage(bool isSignalValid) const;
};
