#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "AppConfig.h"
#include "BatteryVoltageController.h"

class Lcd1602Controller {
public:
  Lcd1602Controller();

  void begin();
  void render(float speedInKiloMetersPerHour, BatteryVoltageSnapshot batteryVoltage);

private:
  LiquidCrystal_I2C lcd_;
  char lastSpeedLine_[17];
  char lastVoltageLine_[17];
};
