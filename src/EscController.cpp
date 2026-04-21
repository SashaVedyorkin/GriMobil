#include "EscController.h"

EscController::EscController(uint8_t pin) : pin_(pin), isStopped_(true) {}

void EscController::begin() {
  esc_.attach(pin_);
  esc_.writeMicroseconds(kNeutralPulse);
  delay(3000);
}

EscUpdateResult EscController::update(unsigned long pulse) {
  if (pulse > kSignalMin && pulse < kSignalMax) {
    int deltaFromNeutral = abs((int)pulse - kNeutralPulse);

    if (isStopped_) {
      if (deltaFromNeutral <= kStopExitDeadband) {
        return writeNeutral(F("1500 STOP"));
      }
    } else if (deltaFromNeutral <= kStopEnterDeadband) {
      return writeNeutral(F("1500 STOP"));
    }

    isStopped_ = false;

    if (pulse > kNeutralPulse) {
      esc_.writeMicroseconds((int)pulse);
      return {nullptr, pulse, true};
    } else {
      return writeNeutral(F("1500 REV_WAIT"));
    }
  } else {
    return writeNeutral(F("1500 NO_SIG"));
  }
}

EscUpdateResult EscController::writeNeutral(const __FlashStringHelper* reason) {
  isStopped_ = true;
  esc_.writeMicroseconds(kNeutralPulse);
  return {reason, 0, false};
}
