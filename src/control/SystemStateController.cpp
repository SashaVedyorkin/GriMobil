#include "SystemStateController.h"

SystemStateController::SystemStateController() : lastCh1NoSignalAtInMilliSec_(0) {}

const char* SystemStateController::resolveLcdStateText(bool isRemoteRequiredForDrive,
                                                       bool isRemoteThrottleSignalAvailable,
                                                       unsigned long nowInMilliSec) {
  if (!isRemoteThrottleSignalAvailable) {
    lastCh1NoSignalAtInMilliSec_ = nowInMilliSec;
  }

  if (!isRemoteRequiredForDrive) {
    return AppConfig::Text::kLcdStateNoCheckMode;
  }

  if (!isRemoteThrottleSignalAvailable ||
      (lastCh1NoSignalAtInMilliSec_ != 0 &&
       nowInMilliSec - lastCh1NoSignalAtInMilliSec_ <=
           AppConfig::Lcd1602::kCh1NoSignalHoldInMilliSec)) {
    return AppConfig::Text::kLcdStateCh1NoSignal;
  }

  return AppConfig::Text::kLcdStateCorrect;
}
