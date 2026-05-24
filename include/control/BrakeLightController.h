#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class BrakeLightController {
public:
  BrakeLightController();

  bool update(unsigned long escPulseInMicroSec, float speedInKiloMetersPerHour);

private:
  bool isBrakeLightEnabled_;
  bool isNeutralStopHoldActive_;
  unsigned long lastThrottleMagnitudeInMicroSec_;

  unsigned long getThrottleMagnitudeInMicroSec(unsigned long escPulseInMicroSec) const;
};
