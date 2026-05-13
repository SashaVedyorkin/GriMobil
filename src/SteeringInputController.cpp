#include "SteeringInputController.h"

SteeringInputController::SteeringInputController(uint8_t potentiometerPin,
                                                 SteeringCorrector& steeringCorrector)
    : potentiometerPin_(potentiometerPin),
      steeringCorrector_(steeringCorrector),
      isCarSteeringEnabled_(true),
      wasReceiverSteeringLockActive_(false) {}

void SteeringInputController::begin() {
  pinMode(potentiometerPin_, INPUT);
}

SteeringInputResult SteeringInputController::resolve(unsigned long receiverSteeringPulseInMicroSec,
                                                     unsigned long receiverLockPulseInMicroSec,
                                                     bool isRemoteRequiredForDrive) {
  updateSteeringControlMode(receiverLockPulseInMicroSec);

  bool isRemoteSteeringSignalAvailable = receiverSteeringPulseInMicroSec != 0;
  bool isReceiverSteeringForced =
      !isCarSteeringEnabled_ || (isRemoteRequiredForDrive && !isRemoteSteeringSignalAvailable);

  unsigned long steeringInputPulseInMicroSec = AppConfig::Steering::kInputCenterInMicroSec;

  if (isReceiverSteeringForced) {
    if (receiverSteeringPulseInMicroSec != 0) {
      steeringInputPulseInMicroSec = steeringCorrector_.apply(receiverSteeringPulseInMicroSec);
    } else {
      steeringCorrector_.reset();
    }
  } else {
    steeringCorrector_.reset();
    steeringInputPulseInMicroSec = readPotentiometerPulseInMicroSec();

    if (shouldUseReceiverSteering(steeringInputPulseInMicroSec, receiverSteeringPulseInMicroSec)) {
      steeringInputPulseInMicroSec = receiverSteeringPulseInMicroSec;
    }
  }

  return {steeringInputPulseInMicroSec, !isReceiverSteeringForced};
}

unsigned long SteeringInputController::readPotentiometerPulseInMicroSec() const {
  int potentiometerValue = analogRead(potentiometerPin_);

  return map(potentiometerValue, AppConfig::Potentiometer::kAnalogMin,
             AppConfig::Potentiometer::kAnalogMax, AppConfig::Steering::kInputRightMaxInMicroSec,
             AppConfig::Steering::kInputLeftMaxInMicroSec);
}

long SteeringInputController::getOffsetFromCenterInMicroSec(unsigned long pulseInMicroSec) const {
  return static_cast<long>(pulseInMicroSec) -
         static_cast<long>(AppConfig::Steering::kInputCenterInMicroSec);
}

unsigned long SteeringInputController::getMagnitudeInMicroSec(long offsetInMicroSec) const {
  return offsetInMicroSec >= 0 ? static_cast<unsigned long>(offsetInMicroSec)
                               : static_cast<unsigned long>(-offsetInMicroSec);
}

bool SteeringInputController::shouldUseReceiverSteering(
    unsigned long potentiometerPulseInMicroSec, unsigned long receiverPulseInMicroSec) const {
  if (receiverPulseInMicroSec == 0) {
    return false;
  }

  long receiverOffsetInMicroSec = getOffsetFromCenterInMicroSec(receiverPulseInMicroSec);
  unsigned long receiverDeltaFromCenterInMicroSec =
      getMagnitudeInMicroSec(receiverOffsetInMicroSec);

  if (receiverDeltaFromCenterInMicroSec < AppConfig::Steering::kReceiverOverrideDeltaInMicroSec) {
    return false;
  }

  long potentiometerOffsetInMicroSec =
      getOffsetFromCenterInMicroSec(potentiometerPulseInMicroSec);
  unsigned long potentiometerDeltaFromCenterInMicroSec =
      getMagnitudeInMicroSec(potentiometerOffsetInMicroSec);

  bool isSameDirection =
      (receiverOffsetInMicroSec > 0 && potentiometerOffsetInMicroSec > 0) ||
      (receiverOffsetInMicroSec < 0 && potentiometerOffsetInMicroSec < 0);

  if (isSameDirection &&
      receiverDeltaFromCenterInMicroSec < potentiometerDeltaFromCenterInMicroSec) {
    return false;
  }

  return true;
}

bool SteeringInputController::isReceiverSteeringLockActive(
    unsigned long receiverLockPulseInMicroSec) const {
  return receiverLockPulseInMicroSec != 0 &&
         receiverLockPulseInMicroSec <= AppConfig::Steering::kReceiverLockThresholdInMicroSec;
}

void SteeringInputController::updateSteeringControlMode(unsigned long receiverLockPulseInMicroSec) {
  bool isReceiverSteeringLockActiveNow =
      isReceiverSteeringLockActive(receiverLockPulseInMicroSec);

  if (isReceiverSteeringLockActiveNow && !wasReceiverSteeringLockActive_) {
    isCarSteeringEnabled_ = !isCarSteeringEnabled_;
  }

  wasReceiverSteeringLockActive_ = isReceiverSteeringLockActiveNow;
}
