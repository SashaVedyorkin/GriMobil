#pragma once

#include <Arduino.h>

#include "AppConfig.h"
struct SteeringSourceSelection {
  unsigned long pulseInMicroSec;
  bool isEffectiveCarSteeringEnabled;
};

class SteeringSourceArbitrator {
public:
  SteeringSourceSelection select(unsigned long potentiometerPulseInMicroSec,
                                 unsigned long receiverSteeringPulseInMicroSec,
                                 bool isCarSteeringEnabled,
                                 bool isRemoteRequiredForDrive);

private:
  long getOffsetFromCenterInMicroSec(unsigned long pulseInMicroSec) const;
  unsigned long getMagnitudeInMicroSec(long offsetInMicroSec) const;
  bool shouldUseReceiverSteering(unsigned long potentiometerPulseInMicroSec,
                                 unsigned long receiverPulseInMicroSec) const;
};
