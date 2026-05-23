#include "SteeringControlModeController.h"

SteeringControlModeController::SteeringControlModeController()
    : isCarSteeringEnabled_(true), wasReceiverSteeringLockActive_(false) {}

void SteeringControlModeController::update(unsigned long receiverLockPulseInMicroSec) {
  bool isReceiverSteeringLockActiveNow =
      isReceiverSteeringLockActive(receiverLockPulseInMicroSec);

  if (isReceiverSteeringLockActiveNow && !wasReceiverSteeringLockActive_) {
    isCarSteeringEnabled_ = !isCarSteeringEnabled_;
  }

  wasReceiverSteeringLockActive_ = isReceiverSteeringLockActiveNow;
}

bool SteeringControlModeController::isCarSteeringEnabled() const {
  return isCarSteeringEnabled_;
}

bool SteeringControlModeController::isReceiverSteeringLockActive(
    unsigned long receiverLockPulseInMicroSec) const {
  return receiverLockPulseInMicroSec != 0 &&
         receiverLockPulseInMicroSec <= AppConfig::Steering::kReceiverLockThresholdInMicroSec;
}
