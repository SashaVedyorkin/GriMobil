#include "ThrottleArbitrator.h"

namespace {
unsigned long constrainPulse(unsigned long value, unsigned long minValue, unsigned long maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

unsigned long getDeltaFromNeutralInMicroSec(unsigned long pulseInMicroSec) {
  unsigned long neutralPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
  return pulseInMicroSec > neutralPulseInMicroSec ? pulseInMicroSec - neutralPulseInMicroSec
                                                  : neutralPulseInMicroSec - pulseInMicroSec;
}
}

ThrottleSelection ThrottleArbitrator::select(DriveModeController::DriveMode driveMode,
                                             int hallThrottleRawValue,
                                             unsigned long ch1PulseInMicroSec,
                                             bool isLocalThrottleEnabled) const {
  unsigned long hallTargetEscPulseInMicroSec =
      isLocalThrottleEnabled ? mapHallThrottleToEsc(driveMode, hallThrottleRawValue)
                             : AppConfig::Esc::kNeutralPulseInMicroSec;
  unsigned long receiverTargetEscPulseInMicroSec =
      mapReceiverThrottleToEsc(driveMode, ch1PulseInMicroSec);

  if (ch1PulseInMicroSec != 0 &&
      getDeltaFromNeutralInMicroSec(receiverTargetEscPulseInMicroSec) >=
          getDeltaFromNeutralInMicroSec(hallTargetEscPulseInMicroSec)) {
    return {receiverTargetEscPulseInMicroSec, "RC"};
  }

  return {hallTargetEscPulseInMicroSec, "P"};
}

unsigned long ThrottleArbitrator::mapHallThrottleToEsc(
    DriveModeController::DriveMode driveMode, int hallThrottleRawValue) const {
  int constrainedHallThrottleRawValue = hallThrottleRawValue;
  if (constrainedHallThrottleRawValue > AppConfig::HallThrottle::kUpperPointRaw) {
    constrainedHallThrottleRawValue = AppConfig::HallThrottle::kUpperPointRaw;
  }
  if (constrainedHallThrottleRawValue < AppConfig::HallThrottle::kLowerPointRaw) {
    constrainedHallThrottleRawValue = AppConfig::HallThrottle::kLowerPointRaw;
  }

  if (driveMode == DriveModeController::kForward) {
    return map(constrainedHallThrottleRawValue, AppConfig::HallThrottle::kUpperPointRaw,
               AppConfig::HallThrottle::kLowerPointRaw,
               AppConfig::Esc::kNeutralPulseInMicroSec,
               AppConfig::Drive::kForwardEscMaxInMicroSec);
  }

  return map(constrainedHallThrottleRawValue, AppConfig::HallThrottle::kUpperPointRaw,
             AppConfig::HallThrottle::kLowerPointRaw,
             AppConfig::Esc::kNeutralPulseInMicroSec,
             AppConfig::Drive::kReverseEscMinInMicroSec);
}

unsigned long ThrottleArbitrator::mapReceiverThrottleToEsc(
    DriveModeController::DriveMode driveMode, unsigned long ch1PulseInMicroSec) const {
  if (ch1PulseInMicroSec == 0) {
    return AppConfig::Esc::kNeutralPulseInMicroSec;
  }

  unsigned long constrainedCh1PulseInMicroSec =
      constrainPulse(ch1PulseInMicroSec, AppConfig::Drive::kThrottleInputLowPointInMicroSec,
                     AppConfig::Drive::kThrottleInputHighPointInMicroSec);

  if (driveMode == DriveModeController::kForward) {
    return map(constrainedCh1PulseInMicroSec, AppConfig::Drive::kThrottleInputLowPointInMicroSec,
               AppConfig::Drive::kThrottleInputHighPointInMicroSec,
               AppConfig::Esc::kNeutralPulseInMicroSec,
               AppConfig::Drive::kForwardEscMaxInMicroSec);
  }

  return map(constrainedCh1PulseInMicroSec, AppConfig::Drive::kThrottleInputLowPointInMicroSec,
             AppConfig::Drive::kThrottleInputHighPointInMicroSec,
             AppConfig::Esc::kNeutralPulseInMicroSec,
             AppConfig::Drive::kReverseEscMinInMicroSec);
}
