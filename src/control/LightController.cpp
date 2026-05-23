#include "LightController.h"

LightController::LightController(uint8_t outputPin)
    : outputPin_(outputPin), isEnabled_(false) {}

void LightController::begin() {
  pinMode(outputPin_, OUTPUT);
  applyOutput();
}

void LightController::setEnabled(bool isEnabled) {
  if (isEnabled_ == isEnabled) {
    return;
  }

  isEnabled_ = isEnabled;
  applyOutput();
}

bool LightController::isEnabled() const {
  return isEnabled_;
}

void LightController::applyOutput() const {
  digitalWrite(outputPin_, isEnabled_ ? HIGH : LOW);
}
