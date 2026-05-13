#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "Pca9685Controller.h"

class SteeringController {
public:
  explicit SteeringController(uint8_t pin);

  void begin();
  unsigned long update(unsigned long ch2PulseInMicroSec);

private:
  uint8_t pin_;
  unsigned long lastAppliedPulseInMicroSec_;

  unsigned long mapInputToServo(unsigned long ch2PulseInMicroSec) const;
};
