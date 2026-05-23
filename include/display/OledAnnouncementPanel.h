#pragma once

#include <Adafruit_SSD1306.h>

class OledAnnouncementPanel {
public:
  void render(Adafruit_SSD1306& display, bool isLeftIndicatorVisible, const char* announcementText,
              const char* rightIndicatorText) const;
};
