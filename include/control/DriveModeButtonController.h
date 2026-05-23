#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class DriveModeButtonController {
public:
  DriveModeButtonController(uint8_t forwardPin, uint8_t reversePin);

  void begin();
  unsigned long readOverridePulseInMicroSec() const;

private:
  uint8_t forwardPin_;
  uint8_t reversePin_;
};
