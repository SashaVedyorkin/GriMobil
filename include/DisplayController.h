#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

#include "AppConfig.h"

struct DisplayBeginResult {
  bool ok;
  const char* message;
};

class DisplayController {
public:
  DisplayController();

  DisplayBeginResult begin();
  void renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3,
                      bool isCarSteeringEnabled, bool isRemoteRequiredForDrive,
                      int hallThrottleRawValue, const char* stateText);
  void renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3,
                      bool isCarSteeringEnabled, bool isRemoteRequiredForDrive,
                      int hallThrottleRawValue, const char* stateText,
                      unsigned long escPulseInMicroSec, unsigned long steeringPulseInMicroSec,
                      const char* custom);

private:
  Adafruit_SSD1306 display_;
  int toPercent(unsigned long pulse) const;
  bool isSignalVisible(unsigned long pulse) const;
  const char* getDriveDisplayLabel(const char* status) const;
  const char* getModeDisplayLabel(bool isRemoteRequiredForDrive) const;
};
