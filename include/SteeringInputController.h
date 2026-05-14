#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "SteeringControlModeController.h"
#include "SteeringSourceArbitrator.h"

struct SteeringInputResult {
  unsigned long pulseInMicroSec;
  bool isEffectiveCarSteeringEnabled;
};

class SteeringInputController {
public:
  SteeringInputController(uint8_t potentiometerPin, SteeringCorrector& steeringCorrector);

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
