#pragma once

#include <Arduino.h>
#include <Servo.h>

#include "AppConfig.h"

struct EscUpdateResult {
  const char* message;
  unsigned long value;
  bool isSuccess;
};

class EscController {
public:
  explicit EscController(uint8_t pin);

  void begin();
  EscUpdateResult update(unsigned long pulse);

private:
  uint8_t pin_;
  Servo esc_;
  bool isStopped_;

  EscUpdateResult writeNeutral(const char* reason);
};
