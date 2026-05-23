#include "EscCommandShaper.h"

namespace {
unsigned long constrainPulse(unsigned long value, unsigned long minValue, unsigned long maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}
}

EscCommandShaper::EscCommandShaper()
    : lastEscPulseInMicroSec_(AppConfig::Esc::kNeutralPulseInMicroSec) {}

unsigned long EscCommandShaper::apply(unsigned long targetEscPulseInMicroSec) {
  unsigned long quantizedEscPulseInMicroSec = applyEscHysteresis(targetEscPulseInMicroSec);
  lastEscPulseInMicroSec_ = quantizedEscPulseInMicroSec;
  return lastEscPulseInMicroSec_;
}

unsigned long EscCommandShaper::quantizeEscPulse(unsigned long targetEscPulseInMicroSec) const {
  unsigned long neutralPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
  unsigned long stepInMicroSec = AppConfig::Drive::kEscStepInMicroSec;

  if (targetEscPulseInMicroSec == neutralPulseInMicroSec) {
    return neutralPulseInMicroSec;
  }

  if (targetEscPulseInMicroSec > neutralPulseInMicroSec) {
    unsigned long offsetInMicroSec = targetEscPulseInMicroSec - neutralPulseInMicroSec;
    unsigned long steppedOffsetInMicroSec =
        ((offsetInMicroSec + (stepInMicroSec / 2)) / stepInMicroSec) * stepInMicroSec;
    return constrainPulse(neutralPulseInMicroSec + steppedOffsetInMicroSec,
                          neutralPulseInMicroSec, AppConfig::Drive::kForwardEscMaxInMicroSec);
  }

  unsigned long offsetInMicroSec = neutralPulseInMicroSec - targetEscPulseInMicroSec;
  unsigned long steppedOffsetInMicroSec =
      ((offsetInMicroSec + (stepInMicroSec / 2)) / stepInMicroSec) * stepInMicroSec;
  return constrainPulse(neutralPulseInMicroSec - steppedOffsetInMicroSec,
                        AppConfig::Drive::kReverseEscMinInMicroSec, neutralPulseInMicroSec);
}

unsigned long EscCommandShaper::applyEscHysteresis(unsigned long targetEscPulseInMicroSec) const {
  if (abs((int)targetEscPulseInMicroSec - (int)lastEscPulseInMicroSec_) <
      AppConfig::Drive::kEscHysteresisInMicroSec) {
    return lastEscPulseInMicroSec_;
  }

  return quantizeEscPulse(targetEscPulseInMicroSec);
}
