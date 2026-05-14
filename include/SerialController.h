#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class SerialController {
public:
  explicit SerialController(unsigned long baudRate = AppConfig::Serial::kBaudRate);

  void begin();
  void logLine(const char* message);
  void logLine(const __FlashStringHelper* message);
  void logValue(unsigned long value);

private:
  unsigned long baudRate_;
};
