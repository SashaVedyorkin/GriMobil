#include "EscController.h"

EscController::EscController(uint8_t pin) : pin_(pin) {}

void EscController::begin() {
  esc_.attach(pin_);
  esc_.writeMicroseconds(AppConfig::Esc::kNeutralPulseInMicroSec);
  delay(AppConfig::Esc::kStartupDelayInMilliSec);
}

EscUpdateResult EscController::update(unsigned long pulse) {
  unsigned long constrainedPulse = pulse;

  if (constrainedPulse < AppConfig::Esc::kSignalMinInMicroSec) {
    constrainedPulse = AppConfig::Esc::kSignalMinInMicroSec;
  }
  if (constrainedPulse > AppConfig::Esc::kSignalMaxInMicroSec) {
    constrainedPulse = AppConfig::Esc::kSignalMaxInMicroSec;
  }

  esc_.writeMicroseconds((int)constrainedPulse);
  return {constrainedPulse};
}
