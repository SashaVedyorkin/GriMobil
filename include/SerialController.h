#pragma once

#include <Arduino.h>

class SerialController {
public:
  explicit SerialController(unsigned long baudRate = 9600);

  void begin();
  void logLine(const __FlashStringHelper* message);
  void logLine(const String& message);
  void logValue(unsigned long value);

private:
  unsigned long baudRate_;
};
