#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class EscCommandShaper {
public:
  EscCommandShaper();

  unsigned long apply(unsigned long targetEscPulseInMicroSec);
  unsigned long lastAppliedPulseInMicroSec() const;

private:
  unsigned long lastEscPulseInMicroSec_;

  unsigned long quantizeEscPulse(unsigned long targetEscPulseInMicroSec) const;
  unsigned long applyEscHysteresis(unsigned long targetEscPulseInMicroSec) const;
  unsigned long smoothEscPulse(unsigned long targetEscPulseInMicroSec) const;
};
