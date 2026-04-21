#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

struct DisplayBeginResult {
  bool ok;
  const __FlashStringHelper* message;
};

class DisplayController {
public:
  DisplayController();

  DisplayBeginResult begin();
  void renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3);
  void renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3, const String& custom);

private:
  static const int kScreenWidth = 128;
  static const int kScreenHeight = 64;
  static const int kOledReset = -1;
  static const int kI2cAddress = 0x3C;
  int percent1 = 0;
  int percent2 = 0;
  int percent3 = 3;

  Adafruit_SSD1306 display_;
  int toPercent(unsigned long pulse);
};
