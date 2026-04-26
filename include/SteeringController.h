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
  enum SteeringDirection {
    kCenterDirection,
    kPositiveDirection,
    kNegativeDirection
  };

  uint8_t pin_;
  Servo servo_;
  unsigned long currentPulseInMicroSec_;
  unsigned long currentPulseFixedInSubMicroSec_;
  unsigned long latchedTargetPulseInMicroSec_;
  unsigned long lastRampUpdateInMilliSec_;
  SteeringDirection latchedDirection_;

  unsigned long mapInputToServo(unsigned long ch2PulseInMicroSec) const;
  unsigned long resolveTargetPulse(unsigned long requestedPulseInMicroSec);
  unsigned long smoothServoPulse(unsigned long targetPulseInMicroSec);
  unsigned long calculateRampStep(unsigned long deltaInMicroSec) const;
};
