#include "DriveModeButtonController.h"

DriveModeButtonController::DriveModeButtonController(uint8_t forwardPin, uint8_t reversePin)
    : forwardPin_(forwardPin), reversePin_(reversePin) {}

void DriveModeButtonController::begin() {
  pinMode(forwardPin_, INPUT_PULLUP);
  pinMode(reversePin_, INPUT_PULLUP);
}

unsigned long DriveModeButtonController::readOverridePulseInMicroSec() const {
  bool isForwardPressed = digitalRead(forwardPin_) == LOW;
  bool isReversePressed = digitalRead(reversePin_) == LOW;

  if (isForwardPressed && !isReversePressed) {
    return AppConfig::Drive::kForwardSwitchThresholdInMicroSec;
  }

  if (isReversePressed && !isForwardPressed) {
    return AppConfig::Drive::kReverseSwitchThresholdInMicroSec;
  }

  return 0;
}
