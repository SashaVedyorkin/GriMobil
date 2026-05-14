#include "SteeringSourceArbitrator.h"

SteeringSourceArbitrator::SteeringSourceArbitrator(SteeringCorrector& steeringCorrector)
    : steeringCorrector_(steeringCorrector) {}

SteeringSourceSelection SteeringSourceArbitrator::select(
    unsigned long potentiometerPulseInMicroSec, unsigned long receiverSteeringPulseInMicroSec,
    bool isCarSteeringEnabled, bool isRemoteRequiredForDrive) {
  bool isRemoteSteeringSignalAvailable = receiverSteeringPulseInMicroSec != 0;
  bool isReceiverSteeringForced =
      !isCarSteeringEnabled || (isRemoteRequiredForDrive && !isRemoteSteeringSignalAvailable);

  unsigned long steeringInputPulseInMicroSec = AppConfig::Steering::kInputCenterInMicroSec;

  if (isReceiverSteeringForced) {
    if (receiverSteeringPulseInMicroSec != 0) {
      steeringInputPulseInMicroSec = steeringCorrector_.apply(receiverSteeringPulseInMicroSec);
    } else {
      steeringCorrector_.reset();
    }
  } else {
    steeringCorrector_.reset();
    steeringInputPulseInMicroSec = potentiometerPulseInMicroSec;

    if (shouldUseReceiverSteering(potentiometerPulseInMicroSec,
                                  receiverSteeringPulseInMicroSec)) {
      steeringInputPulseInMicroSec = receiverSteeringPulseInMicroSec;
    }
  }

  return {steeringInputPulseInMicroSec, !isReceiverSteeringForced};
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
