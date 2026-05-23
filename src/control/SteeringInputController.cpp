#include "SteeringInputController.h"

SteeringInputController::SteeringInputController(uint8_t potentiometerPin)
    : potentiometerPin_(potentiometerPin) {}

void SteeringInputController::begin() {
  pinMode(potentiometerPin_, INPUT);
}

SteeringInputResult SteeringInputController::resolve(unsigned long receiverSteeringPulseInMicroSec,
                                                     unsigned long receiverLockPulseInMicroSec,
                                                     bool isRemoteRequiredForDrive) {
  steeringControlModeController_.update(receiverLockPulseInMicroSec);
  unsigned long potentiometerPulseInMicroSec = readPotentiometerPulseInMicroSec();

  SteeringSourceSelection selection = steeringSourceArbitrator_.select(
      potentiometerPulseInMicroSec, receiverSteeringPulseInMicroSec,
      steeringControlModeController_.isCarSteeringEnabled(), isRemoteRequiredForDrive);
  return {selection.pulseInMicroSec, potentiometerPulseInMicroSec,
          selection.isEffectiveCarSteeringEnabled};
}

unsigned long SteeringInputController::readPotentiometerPulseInMicroSec() const {
  int potentiometerValue = analogRead(potentiometerPin_);

  return map(potentiometerValue, AppConfig::Potentiometer::kAnalogMin,
             AppConfig::Potentiometer::kAnalogMax, AppConfig::Steering::kInputRightMaxInMicroSec,
             AppConfig::Steering::kInputLeftMaxInMicroSec);
}
