#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class DriveModeController {
public:
  enum DriveMode {
    kNeutral,
    kForward,
    kReverse
  };

  DriveModeController();

  void update(unsigned long ch3PulseInMicroSec);
  DriveMode currentMode() const;
  const char* getStatusMessage(bool isSignalValid) const;
  const char* getDisplayLabel() const;

private:
  enum SwitchZone {
    kDown,
    kCenter,
    kUp
  };

  DriveMode driveMode_;
  SwitchZone lastSwitchZone_;

  SwitchZone detectSwitchZone(unsigned long ch3PulseInMicroSec) const;
  void updateDriveMode(SwitchZone currentSwitchZone);
};
