#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "DriveModeController.h"

struct ThrottleSelection {
  unsigned long targetEscPulseInMicroSec;
  const char* sourceLabel;
};

class ThrottleArbitrator {
public:
  ThrottleSelection select(DriveModeController::DriveMode driveMode, int hallThrottleRawValue,
                           unsigned long ch1PulseInMicroSec,
                           bool isLocalThrottleEnabled) const;

private:
  unsigned long mapHallThrottleToEsc(DriveModeController::DriveMode driveMode,
                                     int hallThrottleRawValue) const;
  unsigned long mapReceiverThrottleToEsc(DriveModeController::DriveMode driveMode,
                                         unsigned long ch1PulseInMicroSec) const;
};
