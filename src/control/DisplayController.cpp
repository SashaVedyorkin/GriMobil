#include "DisplayController.h"

DisplayController::DisplayController()
    : display_(AppConfig::Display::kScreenWidth, AppConfig::Display::kScreenHeight, &Wire,
               AppConfig::Display::kOledReset),
      activeStrategy_(AppConfig::Display::kDebugMode
                          ? static_cast<const OledDisplayStrategy*>(&debugStrategy_)
                          : static_cast<const OledDisplayStrategy*>(&userStrategy_)) {}

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
void DisplayController::render(const OledDisplaySnapshot& snapshot) {
  activeStrategy_->render(display_, snapshot);
}
