#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

#include "AppConfig.h"

class Pca9685Controller {
public:
  static void begin();
  static void writeMicroseconds(uint8_t channel, unsigned long pulseInMicroSec);

private:
  static Adafruit_PWMServoDriver driver_;
  static bool isInitialized_;
  static uint16_t microsecondsToTicks(unsigned long pulseInMicroSec);
};
