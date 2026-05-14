#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class SteeringControlModeController {
public:
  SteeringControlModeController();

  void update(unsigned long receiverLockPulseInMicroSec);
  bool isCarSteeringEnabled() const;

private:
  bool isCarSteeringEnabled_;
  bool wasReceiverSteeringLockActive_;

  bool isReceiverSteeringLockActive(unsigned long receiverLockPulseInMicroSec) const;
};
