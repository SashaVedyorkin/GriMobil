#include "OledDisplayStrategy.h"

#include <stdio.h>

bool DebugOledDisplayStrategy::isSignalVisible(unsigned long pulse) const {
  return pulse > AppConfig::Display::kVisibleSignalMinInMicroSec &&
         pulse < AppConfig::Display::kVisibleSignalMaxInMicroSec;
}

const char* DebugOledDisplayStrategy::getModeDisplayLabel(bool isRemoteRequiredForDrive) const {
  return isRemoteRequiredForDrive ? AppConfig::Text::kDisplayModeRemoteCheck
                                  : AppConfig::Text::kDisplayModeNoCheck;
}

void DebugOledDisplayStrategy::render(Adafruit_SSD1306& display,
                                      const OledDisplaySnapshot& snapshot) const {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(AppConfig::Text::kLcdStateLabel);
  display.print(" ");
  display.print(snapshot.stateText);

  display.setCursor(0, 16);
  display.print(AppConfig::Text::kChannel1Label);
  if (isSignalVisible(snapshot.ch1PulseInMicroSec)) {
    display.print(snapshot.ch1PulseInMicroSec);
  } else {
    display.print(snapshot.throttleSourceLabel);
  }
  display.print(" ");
  display.print(snapshot.escPulseInMicroSec);
  display.print(" ");
  display.println(snapshot.driveDisplayLabel);

  display.setCursor(0, 32);
  display.print(AppConfig::Text::kChannel2Label);
  if (isSignalVisible(snapshot.ch2PulseInMicroSec)) {
    display.print(snapshot.ch2PulseInMicroSec);
    display.print(" ");
    display.print(snapshot.steeringPotentiometerPulseInMicroSec);
    display.print(" ");
    display.println(snapshot.isEffectiveCarSteeringEnabled
                        ? AppConfig::Text::kDisplayCarSteering
                        : AppConfig::Text::kDisplayReceiverSteering);
  } else {
    display.print(AppConfig::Text::kShortNoSignal);
    display.print(" ");
    display.print(snapshot.steeringPotentiometerPulseInMicroSec);
    display.print(" ");
    display.println(snapshot.isEffectiveCarSteeringEnabled
                        ? AppConfig::Text::kDisplayCarSteering
                        : AppConfig::Text::kDisplayReceiverSteering);
  }

  display.setCursor(0, 48);
  display.print(AppConfig::Text::kHallLabel);
  display.print(snapshot.hallThrottleRawValue);
  display.setCursor(110, 48);
  display.print(getModeDisplayLabel(snapshot.isRemoteRequiredForDrive));
  display.display();
}

void UserOledDisplayStrategy::render(Adafruit_SSD1306& display,
                                     const OledDisplaySnapshot& snapshot) const {
  char speedBuffer[8];
  char voltageBuffer[16];
  char voltageValueBuffer[8];
  char batteryPercentBuffer[4];

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (snapshot.batteryVoltage.isDefined) {
    uint8_t batteryPercent = BatteryVoltageController::calculateChargePercent(snapshot.batteryVoltage);
    snprintf(batteryPercentBuffer, sizeof(batteryPercentBuffer), "%02u%%", batteryPercent);
    dtostrf(snapshot.batteryVoltage.voltageInVolts, 4, 2, voltageValueBuffer);
    snprintf(voltageBuffer, sizeof(voltageBuffer), "%s%s%s", AppConfig::Text::kVoltageLabel,
             voltageValueBuffer, AppConfig::Text::kVoltageUnit);
  } else {
    snprintf(batteryPercentBuffer, sizeof(batteryPercentBuffer), "%s", "--%");
    snprintf(voltageBuffer, sizeof(voltageBuffer), "%s %s", AppConfig::Text::kVoltageLabel,
             AppConfig::Text::kVoltageNoValue);
  }
  display.print(voltageBuffer);
  display.setCursor(104, 0);
  display.print(batteryPercentBuffer);

  dtostrf(snapshot.speedInKiloMetersPerHour, 4, 2, speedBuffer);
  display.setTextSize(3);
  display.setCursor(16, 24);
  display.print(speedBuffer);
  display.setTextSize(1);
  display.setCursor(104, 22);
  display.print("km");
  display.setCursor(104, 30);
  display.print("--");
  display.setCursor(108, 38);
  display.print("h");
  announcementPanel_.render(display, snapshot.isFrontLightEnabled, snapshot.announcementText,
                            snapshot.driveDisplayLabel);
  display.display();
}
