#pragma once

#include <Adafruit_SSD1306.h>

#include "AppConfig.h"
#include "DisplaySnapshot.h"
#include "OledAnnouncementPanel.h"

class OledDisplayStrategy {
public:
  virtual ~OledDisplayStrategy() {}

  virtual void render(Adafruit_SSD1306& display, const OledDisplaySnapshot& snapshot) const = 0;
};

class DebugOledDisplayStrategy : public OledDisplayStrategy {
public:
  void render(Adafruit_SSD1306& display, const OledDisplaySnapshot& snapshot) const override;

private:
  bool isSignalVisible(unsigned long pulse) const;
  const char* getModeDisplayLabel(bool isRemoteRequiredForDrive) const;
};

class UserOledDisplayStrategy : public OledDisplayStrategy {
public:
  void render(Adafruit_SSD1306& display, const OledDisplaySnapshot& snapshot) const override;

private:
  OledAnnouncementPanel announcementPanel_;
};
