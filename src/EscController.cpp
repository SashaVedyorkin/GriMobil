#include "EscController.h"

EscController::EscController(uint8_t pin) : pin_(pin), isStopped_(true) {}

void EscController::begin() {
  esc_.attach(pin_);
  esc_.writeMicroseconds(AppConfig::Esc::kNeutralPulseInMicroSec);
  delay(AppConfig::Esc::kStartupDelayInMilliSec);
}

EscUpdateResult EscController::update(unsigned long pulse) {
  if (pulse > AppConfig::Esc::kSignalMinInMicroSec &&
      pulse < AppConfig::Esc::kSignalMaxInMicroSec) {
    int deltaFromNeutral = abs((int)pulse - AppConfig::Esc::kNeutralPulseInMicroSec);

    if (isStopped_) {
      if (deltaFromNeutral <= AppConfig::Esc::kStopExitDeadbandInMicroSec) {
        return writeNeutral(AppConfig::Text::kEscStop);
      }
    } else if (deltaFromNeutral <= AppConfig::Esc::kStopEnterDeadbandInMicroSec) {
      return writeNeutral(AppConfig::Text::kEscStop);
    }

    isStopped_ = false;

    if (pulse > AppConfig::Esc::kNeutralPulseInMicroSec) {
      esc_.writeMicroseconds((int)pulse);
      return {nullptr, pulse, true};
    } else {
      return writeNeutral(AppConfig::Text::kEscReverse);
    }
  } else {
    return writeNeutral(AppConfig::Text::kEscNoSignal);
  }
}

EscUpdateResult EscController::writeNeutral(const char* reason) {
  isStopped_ = true;
  esc_.writeMicroseconds(AppConfig::Esc::kNeutralPulseInMicroSec);
  return {reason, 0, false};
}
