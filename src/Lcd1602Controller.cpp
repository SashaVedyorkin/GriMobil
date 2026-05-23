#include "Lcd1602Controller.h"

#include <string.h>

Lcd1602Controller::Lcd1602Controller()
    : lcd_(AppConfig::Lcd1602::kI2cAddress, AppConfig::Lcd1602::kColumns,
           AppConfig::Lcd1602::kRows),
      activeStrategy_(AppConfig::Display::kDebugMode
                          ? static_cast<const Lcd1602DisplayStrategy*>(&debugStrategy_)
                          : static_cast<const Lcd1602DisplayStrategy*>(&userStrategy_)) {
  lastSpeedLine_[0] = '\0';
  lastVoltageLine_[0] = '\0';
}

void Lcd1602Controller::begin() {
  lcd_.init();
  lcd_.backlight();
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print(AppConfig::Text::kStartupReadyLine1);
  lcd_.setCursor(0, 1);
  lcd_.print(AppConfig::Text::kStartupReadyLine2);
}

void Lcd1602Controller::render(const Lcd1602Snapshot& snapshot) {
  char line1[17];
  char line2[17];

  activeStrategy_->buildLines(snapshot, line1, line2);

  if (strcmp(line1, lastSpeedLine_) != 0) {
    lcd_.setCursor(0, 0);
    lcd_.print(line1);
    strncpy(lastSpeedLine_, line1, sizeof(lastSpeedLine_));
    lastSpeedLine_[sizeof(lastSpeedLine_) - 1] = '\0';
  }

  if (strcmp(line2, lastVoltageLine_) != 0) {
    lcd_.setCursor(0, 1);
    lcd_.print(line2);
    strncpy(lastVoltageLine_, line2, sizeof(lastVoltageLine_));
    lastVoltageLine_[sizeof(lastVoltageLine_) - 1] = '\0';
  }
}
