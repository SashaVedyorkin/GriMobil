#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class LightController {
public:
  explicit LightController(uint8_t outputPin);

  void begin();
  void setEnabled(bool isEnabled);
  bool isEnabled() const;

private:
  uint8_t outputPin_;
  bool isEnabled_;
  void applyOutput() const;
};
