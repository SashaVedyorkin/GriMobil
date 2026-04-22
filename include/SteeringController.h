#pragma once

#include <Arduino.h>
#include <Servo.h>

#include "AppConfig.h"

class SteeringController {
public:
  explicit SteeringController(uint8_t pin);

  void begin();
  unsigned long update(unsigned long ch2PulseInMicroSec);

private:
  uint8_t pin_;
  Servo servo_;

  unsigned long mapInputToServo(unsigned long ch2PulseInMicroSec) const;
};
