#include "Lcd1602Controller.h"

#include <string.h>

Lcd1602Controller::Lcd1602Controller()
    : lcd_(AppConfig::Lcd1602::kI2cAddress, AppConfig::Lcd1602::kColumns,
           AppConfig::Lcd1602::kRows) {
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
  char speedLine[17];
  char speedValue[8];
  char voltageLine[17];

  dtostrf(snapshot.speedInKiloMetersPerHour, 4, 2, speedValue);
  snprintf(speedLine, sizeof(speedLine), "V:%s km/h      ", speedValue);

  if (snapshot.batteryVoltage.isDefined) {
    char voltageValue[8];
    dtostrf(snapshot.batteryVoltage.voltageInVolts, 4, 2, voltageValue);
    snprintf(voltageLine, sizeof(voltageLine), "%s%s%s      ", AppConfig::Text::kVoltageLabel,
             voltageValue, AppConfig::Text::kVoltageUnit);
  } else {
    snprintf(voltageLine, sizeof(voltageLine), "%s %s         ",
             AppConfig::Text::kVoltageLabel, AppConfig::Text::kVoltageNoValue);
  }

  if (strcmp(speedLine, lastSpeedLine_) != 0) {
    lcd_.setCursor(0, 0);
    lcd_.print(speedLine);
    strncpy(lastSpeedLine_, speedLine, sizeof(lastSpeedLine_));
    lastSpeedLine_[sizeof(lastSpeedLine_) - 1] = '\0';
  }

  if (strcmp(voltageLine, lastVoltageLine_) != 0) {
    lcd_.setCursor(0, 1);
    lcd_.print(voltageLine);
    strncpy(lastVoltageLine_, voltageLine, sizeof(lastVoltageLine_));
    lastVoltageLine_[sizeof(lastVoltageLine_) - 1] = '\0';
  }
}
