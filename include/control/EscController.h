#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "Pca9685Controller.h"

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
};
