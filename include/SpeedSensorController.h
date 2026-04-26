#pragma once

#include <Arduino.h>

#include "AppConfig.h"

struct SpeedSensorSnapshot {
  bool isActive;
  float speedInKiloMetersPerHour;
};

class SpeedSensorController {
public:
  explicit SpeedSensorController(uint8_t pin);

  void begin();
  SpeedSensorSnapshot read() const;
  static void handleInterrupt();

private:
  static SpeedSensorController* instance_;

  uint8_t pin_;
  volatile bool isActive_;
  volatile bool lastLevelHigh_;
  volatile unsigned long lastPulseAtInMicroSec_;
  volatile unsigned long lastIntervalInMicroSec_;
};
