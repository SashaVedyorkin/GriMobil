#pragma once

#include <Arduino.h>
#include <Servo.h>

#include "AppConfig.h"

struct EscUpdateResult {
  unsigned long value;
};

class EscController {
public:
  explicit EscController(uint8_t pin);

  void begin();
  EscUpdateResult update(unsigned long pulse);

private:
  uint8_t pin_;
  Servo esc_;
};
