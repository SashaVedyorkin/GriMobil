#include "Lcd1602Controller.h"

#include <string.h>

Lcd1602Controller::Lcd1602Controller()
    : lcd_(AppConfig::Lcd1602::kI2cAddress, AppConfig::Lcd1602::kColumns,
           AppConfig::Lcd1602::kRows) {
  lastSpeedLine_[0] = '\0';
  lastModeLine_[0] = '\0';
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

void Lcd1602Controller::render(float speedInKiloMetersPerHour, bool isRemoteRequiredForDrive) {
  char speedLine[17];
  char speedValue[8];
  char modeLine[17];

  dtostrf(speedInKiloMetersPerHour, 4, 2, speedValue);
  snprintf(speedLine, sizeof(speedLine), "V:%s km/h      ", speedValue);
  snprintf(modeLine, sizeof(modeLine), "MODE: %-3s      ",
           isRemoteRequiredForDrive ? AppConfig::Text::kDisplayModeRemoteCheck
                                    : AppConfig::Text::kDisplayModeNoCheck);

  if (strcmp(speedLine, lastSpeedLine_) != 0) {
    lcd_.setCursor(0, 0);
    lcd_.print(speedLine);
    strncpy(lastSpeedLine_, speedLine, sizeof(lastSpeedLine_));
    lastSpeedLine_[sizeof(lastSpeedLine_) - 1] = '\0';
  }

  if (strcmp(modeLine, lastModeLine_) != 0) {
    lcd_.setCursor(0, 1);
    lcd_.print(modeLine);
    strncpy(lastModeLine_, modeLine, sizeof(lastModeLine_));
    lastModeLine_[sizeof(lastModeLine_) - 1] = '\0';
  }
}
