#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "AppConfig.h"

class Lcd1602Controller {
public:
  Lcd1602Controller();

  void begin();
  void render(float speedInKiloMetersPerHour, bool isRemoteRequiredForDrive);

private:
  LiquidCrystal_I2C lcd_;
  char lastSpeedLine_[17];
  char lastModeLine_[17];
};
