#include "DriveModeController.h"

DriveModeController::DriveModeController() : driveMode_(kNeutral), lastSwitchZone_(kCenter) {}

void DriveModeController::update(unsigned long ch3PulseInMicroSec) {
  if (ch3PulseInMicroSec == 0) {
    lastSwitchZone_ = kCenter;
    return;
  }

  SwitchZone currentSwitchZone = detectSwitchZone(ch3PulseInMicroSec);
  updateDriveMode(currentSwitchZone);
}

DriveModeController::DriveMode DriveModeController::currentMode() const {
  return driveMode_;
}

const char* DriveModeController::getStatusMessage(bool isSignalValid) const {
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

const char* DriveModeController::getDisplayLabel() const {
  if (driveMode_ == kForward) {
    return AppConfig::Text::kDisplayDriveForward;
  }

  if (driveMode_ == kReverse) {
    return AppConfig::Text::kDisplayDriveReverse;
  }

  return AppConfig::Text::kDisplayDriveNeutral;
}

DriveModeController::SwitchZone DriveModeController::detectSwitchZone(
    unsigned long ch3PulseInMicroSec) const {
  if (ch3PulseInMicroSec >= AppConfig::Drive::kForwardSwitchThresholdInMicroSec) {
    return kUp;
  }
  if (ch3PulseInMicroSec <= AppConfig::Drive::kReverseSwitchThresholdInMicroSec) {
    return kDown;
  }
  return kCenter;
}

void DriveModeController::updateDriveMode(SwitchZone currentSwitchZone) {
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
