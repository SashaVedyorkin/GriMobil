#pragma once

#include <Arduino.h>
#include <Servo.h>

struct EscUpdateResult {
  const __FlashStringHelper* message;
  unsigned long value;
  bool isSuccess;
};

class EscController {
public:
  explicit EscController(uint8_t pin);

  void begin();
  EscUpdateResult update(unsigned long pulse);

private:
  static const int kNeutralPulse = 1500;
  static const int kSignalMin = 1000;
  static const int kSignalMax = 2000;
  static const int kStopEnterDeadband = 10;
  static const int kStopExitDeadband = 35;

  uint8_t pin_;
  Servo esc_;
  bool isStopped_;

  EscUpdateResult writeNeutral(const __FlashStringHelper* reason);
};
