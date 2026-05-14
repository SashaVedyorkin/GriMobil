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

bool DisplayController::isSignalVisible(unsigned long pulse) const {
  return pulse > AppConfig::Display::kVisibleSignalMinInMicroSec &&
         pulse < AppConfig::Display::kVisibleSignalMaxInMicroSec;
}

const char* DisplayController::getModeDisplayLabel(bool isRemoteRequiredForDrive) const {
  return isRemoteRequiredForDrive ? AppConfig::Text::kDisplayModeRemoteCheck
                                  : AppConfig::Text::kDisplayModeNoCheck;
}

void DisplayController::render(const OledDisplaySnapshot& snapshot) {
  display_.clearDisplay();
  display_.setCursor(0, 0);
  display_.print(AppConfig::Text::kLcdStateLabel);
  display_.print(" ");
  display_.print(snapshot.stateText);

  display_.setCursor(0, 16);
  display_.print(AppConfig::Text::kChannel1Label);
  if (isSignalVisible(snapshot.ch1PulseInMicroSec)) {
    display_.print(snapshot.ch1PulseInMicroSec);
  } else {
    display_.print(snapshot.throttleSourceLabel);
  }
  display_.print(" ");
  display_.print(snapshot.escPulseInMicroSec);
  display_.print(" ");
  display_.println(snapshot.driveDisplayLabel);

  display_.setCursor(0, 32);
  display_.print(AppConfig::Text::kChannel2Label);
  if (isSignalVisible(snapshot.ch2PulseInMicroSec)) {
    display_.print(snapshot.ch2PulseInMicroSec);
    display_.print(" ");
    display_.print(snapshot.steeringPulseInMicroSec);
    display_.print(" ");
    display_.println(snapshot.isEffectiveCarSteeringEnabled
                         ? AppConfig::Text::kDisplayCarSteering
                         : AppConfig::Text::kDisplayReceiverSteering);
  } else {
    display_.print(AppConfig::Text::kShortNoSignal);
    display_.print(" ");
    display_.print(snapshot.steeringPulseInMicroSec);
    display_.print(" ");
    display_.println(snapshot.isEffectiveCarSteeringEnabled
                         ? AppConfig::Text::kDisplayCarSteering
                         : AppConfig::Text::kDisplayReceiverSteering);
  }

  display_.setCursor(0, 48);
  display_.print(AppConfig::Text::kHallLabel);
  display_.print(snapshot.hallThrottleRawValue);
  display_.setCursor(110, 48);
  display_.print(getModeDisplayLabel(snapshot.isRemoteRequiredForDrive));

  display_.display();
}
