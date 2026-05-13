#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "SteeringCorrector.h"

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
  SteeringCorrector& steeringCorrector_;
  bool isCarSteeringEnabled_;
  bool wasReceiverSteeringLockActive_;

  unsigned long readPotentiometerPulseInMicroSec() const;
  long getOffsetFromCenterInMicroSec(unsigned long pulseInMicroSec) const;
  unsigned long getMagnitudeInMicroSec(long offsetInMicroSec) const;
  bool shouldUseReceiverSteering(unsigned long potentiometerPulseInMicroSec,
                                 unsigned long receiverPulseInMicroSec) const;
  bool isReceiverSteeringLockActive(unsigned long receiverLockPulseInMicroSec) const;
  void updateSteeringControlMode(unsigned long receiverLockPulseInMicroSec);
};
