#pragma once

#include <LiquidCrystal_I2C.h>

#include "AppConfig.h"
#include "DisplaySnapshot.h"

class Lcd1602DisplayStrategy {
public:
  virtual ~Lcd1602DisplayStrategy() {}

  virtual void buildLines(const Lcd1602Snapshot& snapshot, char line1[17],
                          char line2[17]) const = 0;
};

class DebugLcd1602DisplayStrategy : public Lcd1602DisplayStrategy {
public:
  void buildLines(const Lcd1602Snapshot& snapshot, char line1[17],
                  char line2[17]) const override;
};

class UserLcd1602DisplayStrategy : public Lcd1602DisplayStrategy {
public:
  void buildLines(const Lcd1602Snapshot& snapshot, char line1[17],
                  char line2[17]) const override;
};
