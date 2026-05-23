#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class EscCommandShaper {
public:
  EscCommandShaper();

  unsigned long apply(unsigned long targetEscPulseInMicroSec);

private:
  unsigned long lastEscPulseInMicroSec_;

  unsigned long quantizeEscPulse(unsigned long targetEscPulseInMicroSec) const;
  unsigned long applyEscHysteresis(unsigned long targetEscPulseInMicroSec) const;
};
