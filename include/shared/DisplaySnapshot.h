#pragma once

#include <Arduino.h>

#include "BatteryVoltageController.h"

struct OledDisplaySnapshot {
  unsigned long ch1PulseInMicroSec;
  unsigned long ch2PulseInMicroSec;
  unsigned long ch3PulseInMicroSec;
  unsigned long steeringPotentiometerPulseInMicroSec;
  bool isEffectiveCarSteeringEnabled;
  bool isRemoteRequiredForDrive;
  int hallThrottleRawValue;
  const char* stateText;
  unsigned long escPulseInMicroSec;
  unsigned long steeringPulseInMicroSec;
  const char* driveDisplayLabel;
  const char* throttleSourceLabel;
  float speedInKiloMetersPerHour;
  BatteryVoltageSnapshot batteryVoltage;
  uint8_t throttlePercent;
  bool isFrontLightEnabled;
  const char* announcementText;
};

struct Lcd1602Snapshot {
  float speedInKiloMetersPerHour;
  BatteryVoltageSnapshot batteryVoltage;
  bool isEffectiveCarSteeringEnabled;
  bool isRemoteRequiredForDrive;
  uint8_t throttlePercent;
  const char* stateText;
};
