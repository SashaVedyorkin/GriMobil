#include "SteeringController.h"

namespace {
unsigned long constrainValue(unsigned long value, unsigned long minValue, unsigned long maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}
}  // namespace

SteeringController::SteeringController(uint8_t pin)
    : pin_(pin) {}

void SteeringController::begin() {
  servo_.attach(pin_);
  servo_.writeMicroseconds(AppConfig::Steering::kOutputCenterInMicroSec);
  delay(AppConfig::Steering::kStartupDelayInMilliSec);
}

unsigned long SteeringController::update(unsigned long ch2PulseInMicroSec) {
  if (ch2PulseInMicroSec == 0) {
    servo_.writeMicroseconds(AppConfig::Steering::kOutputCenterInMicroSec);
    return AppConfig::Steering::kOutputCenterInMicroSec;
  }

  unsigned long targetPulseInMicroSec = mapInputToServo(ch2PulseInMicroSec);
  servo_.writeMicroseconds((int)targetPulseInMicroSec);
  return targetPulseInMicroSec;
}

unsigned long SteeringController::mapInputToServo(unsigned long ch2PulseInMicroSec) const {
  unsigned long deltaFromCenterInMicroSec =
      ch2PulseInMicroSec > AppConfig::Steering::kInputCenterInMicroSec
          ? ch2PulseInMicroSec - AppConfig::Steering::kInputCenterInMicroSec
          : AppConfig::Steering::kInputCenterInMicroSec - ch2PulseInMicroSec;

  if (deltaFromCenterInMicroSec <= AppConfig::Steering::kInputDeadZoneInMicroSec) {
    return AppConfig::Steering::kOutputCenterInMicroSec;
  }

  if (ch2PulseInMicroSec >= AppConfig::Steering::kInputCenterInMicroSec) {
    unsigned long constrainedInput = constrainValue(
        ch2PulseInMicroSec, AppConfig::Steering::kInputCenterInMicroSec,
        AppConfig::Steering::kInputLeftMaxInMicroSec);

    return map(constrainedInput, AppConfig::Steering::kInputCenterInMicroSec,
               AppConfig::Steering::kInputLeftMaxInMicroSec,
               AppConfig::Steering::kOutputCenterInMicroSec,
               AppConfig::Steering::kOutputLeftMaxInMicroSec);
  }

  unsigned long constrainedInput =
      constrainValue(ch2PulseInMicroSec, AppConfig::Steering::kInputRightMaxInMicroSec,
                     AppConfig::Steering::kInputCenterInMicroSec);

  return map(constrainedInput, AppConfig::Steering::kInputRightMaxInMicroSec,
             AppConfig::Steering::kInputCenterInMicroSec,
             AppConfig::Steering::kOutputRightMaxInMicroSec,
             AppConfig::Steering::kOutputCenterInMicroSec);
}
