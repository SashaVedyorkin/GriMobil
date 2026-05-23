#include "DriveController.h"

DriveController::DriveController() = default;

DriveUpdateResult DriveController::update(int hallThrottleRawValue,
                                          unsigned long ch1PulseInMicroSec,
                                          unsigned long ch3PulseInMicroSec,
                                          bool isLocalControlsEnabled) {
  bool isCh3Valid = ch3PulseInMicroSec != 0;

  driveModeController_.update(ch3PulseInMicroSec);

  if (driveModeController_.currentMode() == DriveModeController::kNeutral) {
    unsigned long shapedNeutralPulseInMicroSec =
        escCommandShaper_.apply(AppConfig::Esc::kNeutralPulseInMicroSec);
    return {driveModeController_.currentMode(), driveModeController_.getStatusMessage(isCh3Valid),
            driveModeController_.getDisplayLabel(), "P", shapedNeutralPulseInMicroSec,
            isCh3Valid};
  }

  ThrottleSelection selection = throttleArbitrator_.select(
      driveModeController_.currentMode(), hallThrottleRawValue, ch1PulseInMicroSec,
      isLocalControlsEnabled);
  unsigned long shapedEscPulseInMicroSec =
      escCommandShaper_.apply(selection.targetEscPulseInMicroSec);

  return {driveModeController_.currentMode(), driveModeController_.getStatusMessage(isCh3Valid),
          driveModeController_.getDisplayLabel(), selection.sourceLabel,
          shapedEscPulseInMicroSec, isCh3Valid};
}
