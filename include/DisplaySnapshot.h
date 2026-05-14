#pragma once

#include <Arduino.h>

#include "BatteryVoltageController.h"

struct OledDisplaySnapshot {
  unsigned long ch1PulseInMicroSec;
  unsigned long ch2PulseInMicroSec;
  unsigned long ch3PulseInMicroSec;
  bool isEffectiveCarSteeringEnabled;
  bool isRemoteRequiredForDrive;
  int hallThrottleRawValue;
  const char* stateText;
  unsigned long escPulseInMicroSec;
  unsigned long steeringPulseInMicroSec;
  const char* driveDisplayLabel;
  const char* throttleSourceLabel;
};

struct Lcd1602Snapshot {
  float speedInKiloMetersPerHour;
  BatteryVoltageSnapshot batteryVoltage;
};
