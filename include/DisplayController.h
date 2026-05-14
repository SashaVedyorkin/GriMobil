#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

#include "AppConfig.h"
#include "DisplaySnapshot.h"

struct DisplayBeginResult {
  bool ok;
  const char* message;
};

class DisplayController {
public:
  DisplayController();

  DisplayBeginResult begin();
  void render(const OledDisplaySnapshot& snapshot);

private:
  Adafruit_SSD1306 display_;
  bool isSignalVisible(unsigned long pulse) const;
  const char* getModeDisplayLabel(bool isRemoteRequiredForDrive) const;
};
