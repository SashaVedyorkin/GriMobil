#include "DriveController.h"

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
}  // namespace

DriveController::DriveController()
    : driveMode_(kNeutral),
      lastSwitchZone_(kCenter),
      lastEscPulseInMicroSec_(AppConfig::Esc::kNeutralPulseInMicroSec) {}

DriveUpdateResult DriveController::update(int hallThrottleRawValue,
                                          unsigned long ch1PulseInMicroSec,
                                          unsigned long ch3PulseInMicroSec) {
  bool isCh3Valid = ch3PulseInMicroSec != 0;

  if (!isCh3Valid) {
    lastSwitchZone_ = kCenter;
    unsigned long smoothNeutralPulseInMicroSec =
        smoothEscPulse(AppConfig::Esc::kNeutralPulseInMicroSec);
    lastEscPulseInMicroSec_ = smoothNeutralPulseInMicroSec;
    return {AppConfig::Text::kDriveNoSignal, getDriveDisplayLabel(), "P",
            smoothNeutralPulseInMicroSec, false};
  }

  SwitchZone currentSwitchZone = detectSwitchZone(ch3PulseInMicroSec);
  updateDriveMode(currentSwitchZone);

  if (driveMode_ == kNeutral) {
    unsigned long smoothNeutralPulseInMicroSec =
        smoothEscPulse(AppConfig::Esc::kNeutralPulseInMicroSec);
    lastEscPulseInMicroSec_ = smoothNeutralPulseInMicroSec;
    return {AppConfig::Text::kDriveNeutral, getDriveDisplayLabel(), "P",
            smoothNeutralPulseInMicroSec, true};
  }

  unsigned long hallTargetEscPulseInMicroSec = mapThrottleToEsc(hallThrottleRawValue);
  unsigned long receiverTargetEscPulseInMicroSec = mapReceiverThrottleToEsc(ch1PulseInMicroSec);
  unsigned long targetEscPulseInMicroSec =
      selectThrottleTargetEscPulse(hallThrottleRawValue, ch1PulseInMicroSec);
  const char* throttleSourceLabel =
      ch1PulseInMicroSec != 0 &&
              getDeltaFromNeutralInMicroSec(receiverTargetEscPulseInMicroSec) >=
                  getDeltaFromNeutralInMicroSec(hallTargetEscPulseInMicroSec)
          ? "RC"
          : "P";
  unsigned long quantizedEscPulseInMicroSec = applyEscHysteresis(targetEscPulseInMicroSec);
  unsigned long smoothEscPulseInMicroSec = smoothEscPulse(quantizedEscPulseInMicroSec);
  lastEscPulseInMicroSec_ = smoothEscPulseInMicroSec;

  return {getStatusMessage(true), getDriveDisplayLabel(), throttleSourceLabel,
          smoothEscPulseInMicroSec, true};
}

DriveController::SwitchZone DriveController::detectSwitchZone(unsigned long ch3PulseInMicroSec) const {
  if (ch3PulseInMicroSec >= AppConfig::Drive::kForwardSwitchThresholdInMicroSec) {
    return kUp;
  }
  if (ch3PulseInMicroSec <= AppConfig::Drive::kReverseSwitchThresholdInMicroSec) {
    return kDown;
  }
  return kCenter;
}

void DriveController::updateDriveMode(SwitchZone currentSwitchZone) {
  if (currentSwitchZone == lastSwitchZone_) {
    return;
  }

  if (currentSwitchZone == kUp) {
    if (driveMode_ == kNeutral) {
      driveMode_ = kForward;
    } else if (driveMode_ == kReverse) {
      driveMode_ = kNeutral;
    }
  } else if (currentSwitchZone == kDown) {
    if (driveMode_ == kForward) {
      driveMode_ = kNeutral;
    } else if (driveMode_ == kNeutral) {
      driveMode_ = kReverse;
    }
  }

  lastSwitchZone_ = currentSwitchZone;
}

unsigned long DriveController::mapThrottleToEsc(int hallThrottleRawValue) const {
  int constrainedHallThrottleRawValue = hallThrottleRawValue;
  if (constrainedHallThrottleRawValue > AppConfig::HallThrottle::kUpperPointRaw) {
    constrainedHallThrottleRawValue = AppConfig::HallThrottle::kUpperPointRaw;
  }
  if (constrainedHallThrottleRawValue < AppConfig::HallThrottle::kLowerPointRaw) {
    constrainedHallThrottleRawValue = AppConfig::HallThrottle::kLowerPointRaw;
  }

  if (driveMode_ == kForward) {
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

unsigned long DriveController::mapReceiverThrottleToEsc(unsigned long ch1PulseInMicroSec) const {
  if (ch1PulseInMicroSec == 0) {
    return AppConfig::Esc::kNeutralPulseInMicroSec;
  }

  unsigned long constrainedCh1PulseInMicroSec =
      constrainPulse(ch1PulseInMicroSec, AppConfig::Drive::kThrottleInputLowPointInMicroSec,
                     AppConfig::Drive::kThrottleInputHighPointInMicroSec);

  if (driveMode_ == kForward) {
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

unsigned long DriveController::selectThrottleTargetEscPulse(int hallThrottleRawValue,
                                                            unsigned long ch1PulseInMicroSec) const {
  unsigned long hallTargetEscPulseInMicroSec = mapThrottleToEsc(hallThrottleRawValue);
  unsigned long receiverTargetEscPulseInMicroSec = mapReceiverThrottleToEsc(ch1PulseInMicroSec);

  unsigned long neutralPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
  unsigned long hallDeltaInMicroSec =
      hallTargetEscPulseInMicroSec > neutralPulseInMicroSec
          ? hallTargetEscPulseInMicroSec - neutralPulseInMicroSec
          : neutralPulseInMicroSec - hallTargetEscPulseInMicroSec;
  unsigned long receiverDeltaInMicroSec =
      receiverTargetEscPulseInMicroSec > neutralPulseInMicroSec
          ? receiverTargetEscPulseInMicroSec - neutralPulseInMicroSec
          : neutralPulseInMicroSec - receiverTargetEscPulseInMicroSec;

  if (ch1PulseInMicroSec != 0 && receiverDeltaInMicroSec >= hallDeltaInMicroSec) {
    return receiverTargetEscPulseInMicroSec;
  }

  return hallTargetEscPulseInMicroSec;
}

unsigned long DriveController::quantizeEscPulse(unsigned long targetEscPulseInMicroSec) const {
  unsigned long neutralPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
  unsigned long stepInMicroSec = AppConfig::Drive::kEscStepInMicroSec;

  if (targetEscPulseInMicroSec == neutralPulseInMicroSec) {
    return neutralPulseInMicroSec;
  }

  if (targetEscPulseInMicroSec > neutralPulseInMicroSec) {
    unsigned long offsetInMicroSec = targetEscPulseInMicroSec - neutralPulseInMicroSec;
    unsigned long steppedOffsetInMicroSec = ((offsetInMicroSec + (stepInMicroSec / 2)) / stepInMicroSec) * stepInMicroSec;
    return constrainPulse(neutralPulseInMicroSec + steppedOffsetInMicroSec,
                          neutralPulseInMicroSec, AppConfig::Drive::kForwardEscMaxInMicroSec);
  }

  unsigned long offsetInMicroSec = neutralPulseInMicroSec - targetEscPulseInMicroSec;
  unsigned long steppedOffsetInMicroSec = ((offsetInMicroSec + (stepInMicroSec / 2)) / stepInMicroSec) * stepInMicroSec;
  return constrainPulse(neutralPulseInMicroSec - steppedOffsetInMicroSec,
                        AppConfig::Drive::kReverseEscMinInMicroSec, neutralPulseInMicroSec);
}

unsigned long DriveController::applyEscHysteresis(unsigned long targetEscPulseInMicroSec) const {
  if (abs((int)targetEscPulseInMicroSec - (int)lastEscPulseInMicroSec_) <
      AppConfig::Drive::kEscHysteresisInMicroSec) {
    return lastEscPulseInMicroSec_;
  }

  return quantizeEscPulse(targetEscPulseInMicroSec);
}

unsigned long DriveController::smoothEscPulse(unsigned long targetEscPulseInMicroSec) const {
  if (targetEscPulseInMicroSec == lastEscPulseInMicroSec_) {
    return lastEscPulseInMicroSec_;
  }

  unsigned long rampStepInMicroSec =
      targetEscPulseInMicroSec == AppConfig::Esc::kNeutralPulseInMicroSec
          ? AppConfig::Drive::kEscFastStopRampStepInMicroSec
          : AppConfig::Drive::kEscRampStepInMicroSec;

  unsigned long deltaInMicroSec =
      targetEscPulseInMicroSec > lastEscPulseInMicroSec_
          ? targetEscPulseInMicroSec - lastEscPulseInMicroSec_
          : lastEscPulseInMicroSec_ - targetEscPulseInMicroSec;

  if (deltaInMicroSec <= rampStepInMicroSec) {
    return targetEscPulseInMicroSec;
  }

  if (targetEscPulseInMicroSec > lastEscPulseInMicroSec_) {
    return lastEscPulseInMicroSec_ + rampStepInMicroSec;
  }

  return lastEscPulseInMicroSec_ - rampStepInMicroSec;
}

const char* DriveController::getStatusMessage(bool isSignalValid) const {
  if (!isSignalValid) {
    return AppConfig::Text::kDriveNoSignal;
  }

  if (driveMode_ == kForward) {
    return AppConfig::Text::kDriveForward;
  }

  if (driveMode_ == kReverse) {
    return AppConfig::Text::kDriveReverse;
  }

  return AppConfig::Text::kDriveNeutral;
}

const char* DriveController::getDriveDisplayLabel() const {
  if (driveMode_ == kForward) {
    return AppConfig::Text::kDisplayDriveForward;
  }

  if (driveMode_ == kReverse) {
    return AppConfig::Text::kDisplayDriveReverse;
  }

  return AppConfig::Text::kDisplayDriveNeutral;
}
