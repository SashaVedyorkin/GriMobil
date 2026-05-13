#pragma once

#include <Arduino.h>

#include "AppConfig.h"

struct BatteryVoltageSnapshot {
  bool isDefined;
  float voltageInVolts;
};

class BatteryVoltageController {
public:
  explicit BatteryVoltageController(uint8_t pin);

  void begin();
  BatteryVoltageSnapshot read() const;

private:
  uint8_t pin_;
};
