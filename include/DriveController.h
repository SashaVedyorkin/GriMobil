#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "DriveModeController.h"
#include "EscCommandShaper.h"
#include "ThrottleArbitrator.h"

struct DriveUpdateResult {
  const char* statusMessage;
  const char* driveDisplayLabel;
  const char* throttleSourceLabel;
  unsigned long escPulseInMicroSec;
  bool isSignalValid;
};

class DriveController {
public:
  DriveController();

  DriveUpdateResult update(int hallThrottleRawValue, unsigned long ch1PulseInMicroSec,
                           unsigned long ch3PulseInMicroSec, bool isLocalControlsEnabled);

private:
  DriveModeController driveModeController_;
  ThrottleArbitrator throttleArbitrator_;
  EscCommandShaper escCommandShaper_;
};
