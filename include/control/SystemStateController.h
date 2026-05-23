#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class SystemStateController {
public:
  SystemStateController();

  const char* resolveLcdStateText(bool isRemoteRequiredForDrive, bool isRemoteThrottleSignalAvailable,
                                  unsigned long nowInMilliSec);

private:
  unsigned long lastCh1NoSignalAtInMilliSec_;
};
