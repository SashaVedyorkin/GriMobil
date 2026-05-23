#include "OledAnnouncementPanel.h"

#include <string.h>

void OledAnnouncementPanel::render(Adafruit_SSD1306& display, bool isLeftIndicatorVisible,
                                   const char* announcementText,
                                   const char* rightIndicatorText) const {
  char announcementBuffer[18];
  size_t announcementLength = strlen(announcementText);
  const int panelStartX = 12;
  const int panelEndX = 108;
  const int panelWidth = panelEndX - panelStartX;
  const int glyphWidth = 6;

  if (announcementLength > 17) {
    announcementLength = 17;
  }

  memcpy(announcementBuffer, announcementText, announcementLength);
  announcementBuffer[announcementLength] = '\0';

  if (isLeftIndicatorVisible) {
    display.setCursor(0, 56);
    display.print("*");
  }

  display.setCursor(6, 56);
  display.print(".");

  int textWidth = static_cast<int>(announcementLength) * glyphWidth;
  int textX = panelStartX + ((panelWidth - textWidth) / 2);
  if (textX < panelStartX) {
    textX = panelStartX;
  }

  display.setCursor(textX, 56);
  display.print(announcementBuffer);

  display.setCursor(114, 56);
  display.print(".");

  display.setCursor(120, 56);
  display.print(rightIndicatorText);
}
