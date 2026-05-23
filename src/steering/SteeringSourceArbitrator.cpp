#include "SteeringSourceArbitrator.h"

SteeringSourceSelection SteeringSourceArbitrator::select(
    unsigned long potentiometerPulseInMicroSec, unsigned long receiverSteeringPulseInMicroSec,
    bool isCarSteeringEnabled, bool isRemoteRequiredForDrive) {
  bool isRemoteSteeringSignalAvailable = receiverSteeringPulseInMicroSec != 0;
  bool isReceiverSteeringForced =
      !isCarSteeringEnabled || (isRemoteRequiredForDrive && !isRemoteSteeringSignalAvailable);

  if (isReceiverSteeringForced) {
    if (receiverSteeringPulseInMicroSec != 0) {
      return {receiverSteeringPulseInMicroSec, false};
    }

    return {AppConfig::Steering::kInputCenterInMicroSec, false};
  }

  if (isRemoteSteeringSignalAvailable &&
      shouldUseReceiverSteering(potentiometerPulseInMicroSec, receiverSteeringPulseInMicroSec)) {
    return {receiverSteeringPulseInMicroSec, true};
  }

  return {potentiometerPulseInMicroSec, true};
}

long SteeringSourceArbitrator::getOffsetFromCenterInMicroSec(unsigned long pulseInMicroSec) const {
  return static_cast<long>(pulseInMicroSec) -
         static_cast<long>(AppConfig::Steering::kInputCenterInMicroSec);
}

unsigned long SteeringSourceArbitrator::getMagnitudeInMicroSec(long offsetInMicroSec) const {
  return offsetInMicroSec >= 0 ? static_cast<unsigned long>(offsetInMicroSec)
                               : static_cast<unsigned long>(-offsetInMicroSec);
}

bool SteeringSourceArbitrator::shouldUseReceiverSteering(
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
