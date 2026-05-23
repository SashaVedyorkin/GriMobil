#include "Pca9685Controller.h"

Adafruit_PWMServoDriver Pca9685Controller::driver_ =
    Adafruit_PWMServoDriver(AppConfig::Pca9685::kI2cAddress);
bool Pca9685Controller::isInitialized_ = false;

void Pca9685Controller::begin() {
  if (isInitialized_) {
    return;
  }

  driver_.begin();
  driver_.setPWMFreq(AppConfig::Pca9685::kPwmFrequencyInHertz);
  isInitialized_ = true;
}

void Pca9685Controller::writeMicroseconds(uint8_t channel, unsigned long pulseInMicroSec) {
  if (!isInitialized_) {
    begin();
  }

  driver_.setPWM(channel, 0, microsecondsToTicks(pulseInMicroSec));
}

uint16_t Pca9685Controller::microsecondsToTicks(unsigned long pulseInMicroSec) {
  unsigned long ticks = (pulseInMicroSec * AppConfig::Pca9685::kPwmFrequencyInHertz *
                         AppConfig::Pca9685::kPwmResolution) /
                        1000000UL;

  if (ticks >= AppConfig::Pca9685::kPwmResolution) {
    ticks = AppConfig::Pca9685::kPwmResolution - 1;
  }

  return static_cast<uint16_t>(ticks);
}
