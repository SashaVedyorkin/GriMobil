#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "SteeringControlModeController.h"
#include "SteeringSourceArbitrator.h"

struct SteeringInputResult {
  unsigned long pulseInMicroSec;
  unsigned long potentiometerPulseInMicroSec;
  bool isEffectiveCarSteeringEnabled;
};

class SteeringInputController {
public:
  explicit SteeringInputController(uint8_t potentiometerPin);

  void begin();
  SteeringInputResult resolve(unsigned long receiverSteeringPulseInMicroSec,
                              unsigned long receiverLockPulseInMicroSec,
                              bool isRemoteRequiredForDrive);

private:
  uint8_t potentiometerPin_;
  SteeringControlModeController steeringControlModeController_;
  SteeringSourceArbitrator steeringSourceArbitrator_;

  unsigned long readPotentiometerPulseInMicroSec() const;
};
