#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "AppConfig.h"
#include "DisplaySnapshot.h"

class Lcd1602Controller {
public:
  Lcd1602Controller();

  void begin();
  void render(const Lcd1602Snapshot& snapshot);

private:
  LiquidCrystal_I2C lcd_;
  char lastSpeedLine_[17];
  char lastVoltageLine_[17];
};
