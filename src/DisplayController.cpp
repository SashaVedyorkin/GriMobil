#include "DisplayController.h"

DisplayController::DisplayController()
    : display_(AppConfig::Display::kScreenWidth, AppConfig::Display::kScreenHeight, &Wire,
               AppConfig::Display::kOledReset) {}

DisplayBeginResult DisplayController::begin() {
  if (!display_.begin(SSD1306_SWITCHCAPVCC, AppConfig::Display::kI2cAddress)) {
    return {false, AppConfig::Text::kOledNotFound};
  }

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);
  display_.println(AppConfig::Text::kStartupMonitorTitle);
  display_.display();
  delay(AppConfig::Display::kStartupDelayInMilliSec);

  return {true, nullptr};
}

void DisplayController::renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3) {
  renderChannels(ch1, ch2, ch3, "");
}

int DisplayController::toPercent(unsigned long pulse) const {
  if (pulse < AppConfig::Display::kPercentMinPulseInMicroSec) {
    pulse = AppConfig::Display::kPercentMinPulseInMicroSec;
  }
  if (pulse > AppConfig::Display::kPercentMaxPulseInMicroSec) {
    pulse = AppConfig::Display::kPercentMaxPulseInMicroSec;
  }
  return map(pulse, AppConfig::Display::kPercentMinPulseInMicroSec,
             AppConfig::Display::kPercentMaxPulseInMicroSec, 0, 100);
}

void DisplayController::renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3,
                                       const String& custom) {
  int percent1 = 0;
  int percent2 = 0;
  int percent3 = 0;

  if (ch1 > AppConfig::Display::kVisibleSignalMinInMicroSec &&
      ch1 < AppConfig::Display::kVisibleSignalMaxInMicroSec) {
    percent1 = toPercent(ch1);
  }
  if (ch2 > AppConfig::Display::kVisibleSignalMinInMicroSec &&
      ch2 < AppConfig::Display::kVisibleSignalMaxInMicroSec) {
    percent2 = toPercent(ch2);
  }
  if (ch3 > AppConfig::Display::kVisibleSignalMinInMicroSec &&
      ch3 < AppConfig::Display::kVisibleSignalMaxInMicroSec) {
    percent3 = toPercent(ch3);
  }

  display_.clearDisplay();
  display_.setCursor(0, 0);
  display_.print(AppConfig::Text::kMainScreenTitle);

  display_.setCursor(0, 16);
  display_.print(AppConfig::Text::kChannel1Label);
  display_.print(ch1);
  display_.print(" ");
  display_.print(percent1);
  display_.print("%");
  if (custom.length() > 0) {
    display_.print(" ");
    display_.println(custom);
  } else {
    display_.println(" ");
  }

  display_.setCursor(0, 32);
  display_.print(AppConfig::Text::kChannel2Label);
  display_.print(ch2);
  display_.print(" ");
  display_.print(percent2);
  display_.print("%");
  display_.println(" ");

  display_.setCursor(0, 48);
  display_.print(AppConfig::Text::kChannel3Label);
  display_.print(ch3);
  display_.print(" ");
  display_.print(percent3);
  display_.print("%");
  display_.println(" ");
  
  display_.display();
}
