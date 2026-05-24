#include "BrakeLightController.h"

BrakeLightController::BrakeLightController()
    : isBrakeLightEnabled_(false),
      isNeutralStopHoldActive_(false),
      lastThrottleMagnitudeInMicroSec_(0) {}

bool BrakeLightController::update(unsigned long escPulseInMicroSec,
                                  float speedInKiloMetersPerHour) {
  unsigned long currentThrottleMagnitudeInMicroSec =
      getThrottleMagnitudeInMicroSec(escPulseInMicroSec);

  if (lastThrottleMagnitudeInMicroSec_ != 0 && currentThrottleMagnitudeInMicroSec == 0) {
    isNeutralStopHoldActive_ = true;
    isBrakeLightEnabled_ = true;
  }

  if (isNeutralStopHoldActive_) {
    if (speedInKiloMetersPerHour <=
        AppConfig::BrakeLight::kStoppedSpeedThresholdInKiloMetersPerHour) {
      isNeutralStopHoldActive_ = false;
      isBrakeLightEnabled_ = false;
    }

    lastThrottleMagnitudeInMicroSec_ = currentThrottleMagnitudeInMicroSec;
    return isBrakeLightEnabled_;
  }

  lastThrottleMagnitudeInMicroSec_ = currentThrottleMagnitudeInMicroSec;
  return isBrakeLightEnabled_;
}

unsigned long BrakeLightController::getThrottleMagnitudeInMicroSec(
    unsigned long escPulseInMicroSec) const {
  unsigned long neutralPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
  return escPulseInMicroSec > neutralPulseInMicroSec ? escPulseInMicroSec - neutralPulseInMicroSec
                                                     : neutralPulseInMicroSec - escPulseInMicroSec;
}
