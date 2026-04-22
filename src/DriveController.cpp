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
}  // namespace

DriveController::DriveController()
    : driveMode_(kNeutral),
      lastSwitchZone_(kCenter),
      lastEscPulseInMicroSec_(AppConfig::Esc::kNeutralPulseInMicroSec) {}

DriveUpdateResult DriveController::update(unsigned long ch1PulseInMicroSec,
                                          unsigned long ch3PulseInMicroSec) {
  bool isCh1Valid = ch1PulseInMicroSec != 0;
  bool isCh3Valid = ch3PulseInMicroSec != 0;

  if (!isCh3Valid) {
    driveMode_ = kNeutral;
    lastSwitchZone_ = kCenter;
    unsigned long smoothNeutralPulseInMicroSec =
        smoothEscPulse(AppConfig::Esc::kNeutralPulseInMicroSec);
    lastEscPulseInMicroSec_ = smoothNeutralPulseInMicroSec;
    return {AppConfig::Text::kDriveNoSignal, smoothNeutralPulseInMicroSec, false};
  }

  SwitchZone currentSwitchZone = detectSwitchZone(ch3PulseInMicroSec);
  updateDriveMode(currentSwitchZone);

  if (!isCh1Valid) {
    unsigned long smoothNeutralPulseInMicroSec =
        smoothEscPulse(AppConfig::Esc::kNeutralPulseInMicroSec);
    lastEscPulseInMicroSec_ = smoothNeutralPulseInMicroSec;
    return {AppConfig::Text::kDriveNoSignal, smoothNeutralPulseInMicroSec, false};
  }

  if (driveMode_ == kNeutral) {
    unsigned long smoothNeutralPulseInMicroSec =
        smoothEscPulse(AppConfig::Esc::kNeutralPulseInMicroSec);
    lastEscPulseInMicroSec_ = smoothNeutralPulseInMicroSec;
    return {AppConfig::Text::kDriveNeutral, smoothNeutralPulseInMicroSec, true};
  }

  unsigned long targetEscPulseInMicroSec = mapThrottleToEsc(ch1PulseInMicroSec);
  unsigned long quantizedEscPulseInMicroSec = applyEscHysteresis(targetEscPulseInMicroSec);
  unsigned long smoothEscPulseInMicroSec = smoothEscPulse(quantizedEscPulseInMicroSec);
  lastEscPulseInMicroSec_ = smoothEscPulseInMicroSec;

  return {getStatusMessage(true), smoothEscPulseInMicroSec, true};
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

unsigned long DriveController::mapThrottleToEsc(unsigned long ch1PulseInMicroSec) const {
  unsigned long constrainedCh1PulseInMicroSec =
      constrainPulse(ch1PulseInMicroSec, AppConfig::Drive::kThrottleInputMinInMicroSec,
                     AppConfig::Drive::kThrottleInputMaxInMicroSec);

  if (driveMode_ == kForward) {
    return map(constrainedCh1PulseInMicroSec, AppConfig::Drive::kThrottleInputMinInMicroSec,
               AppConfig::Drive::kThrottleInputMaxInMicroSec,
               AppConfig::Esc::kNeutralPulseInMicroSec,
               AppConfig::Drive::kForwardEscMaxInMicroSec);
  }

  return map(constrainedCh1PulseInMicroSec, AppConfig::Drive::kThrottleInputMinInMicroSec,
             AppConfig::Drive::kThrottleInputMaxInMicroSec,
             AppConfig::Esc::kNeutralPulseInMicroSec,
             AppConfig::Drive::kReverseEscMinInMicroSec);
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

  unsigned long deltaInMicroSec =
      targetEscPulseInMicroSec > lastEscPulseInMicroSec_
          ? targetEscPulseInMicroSec - lastEscPulseInMicroSec_
          : lastEscPulseInMicroSec_ - targetEscPulseInMicroSec;

  if (deltaInMicroSec <= AppConfig::Drive::kEscRampStepInMicroSec) {
    return targetEscPulseInMicroSec;
  }

  if (targetEscPulseInMicroSec > lastEscPulseInMicroSec_) {
    return lastEscPulseInMicroSec_ + AppConfig::Drive::kEscRampStepInMicroSec;
  }

  return lastEscPulseInMicroSec_ - AppConfig::Drive::kEscRampStepInMicroSec;
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
