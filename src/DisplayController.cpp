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

void DisplayController::renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3,
                                       bool isCarSteeringEnabled,
                                       bool isRemoteRequiredForDrive) {
  renderChannels(ch1, ch2, ch3, isCarSteeringEnabled, isRemoteRequiredForDrive, 0, 0, "");
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

bool DisplayController::isSignalVisible(unsigned long pulse) const {
  return pulse > AppConfig::Display::kVisibleSignalMinInMicroSec &&
         pulse < AppConfig::Display::kVisibleSignalMaxInMicroSec;
}

const char* DisplayController::getDriveDisplayLabel(const char* status) const {
  if (status == nullptr || status[0] == '\0') {
    return AppConfig::Text::kDisplayDriveNeutral;
  }

  if (strcmp(status, AppConfig::Text::kDriveForward) == 0) {
    return AppConfig::Text::kDisplayDriveForward;
  }

  if (strcmp(status, AppConfig::Text::kDriveReverse) == 0) {
    return AppConfig::Text::kDisplayDriveReverse;
  }

  if (strcmp(status, AppConfig::Text::kDriveNoSignal) == 0) {
    return AppConfig::Text::kDriveNoSignal;
  }

  return AppConfig::Text::kDisplayDriveNeutral;
}

void DisplayController::renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3,
                                       bool isCarSteeringEnabled,
                                       bool isRemoteRequiredForDrive,
                                       unsigned long escPulseInMicroSec,
                                       unsigned long steeringPulseInMicroSec, const char* custom) {
  display_.clearDisplay();
  display_.setCursor(0, 0);
  display_.print(AppConfig::Text::kMainScreenTitle);

  display_.setCursor(0, 16);
  display_.print(AppConfig::Text::kChannel1Label);
  if (isSignalVisible(ch1)) {
    display_.print(ch1);
    display_.print(" ");
    display_.print(escPulseInMicroSec);
    display_.print(" ");
    display_.println(getDriveDisplayLabel(custom));
  } else {
    display_.println(AppConfig::Text::kDriveNoSignal);
  }

  display_.setCursor(0, 32);
  display_.print(AppConfig::Text::kChannel2Label);
  if (isSignalVisible(ch2)) {
    display_.print(ch2);
    display_.print(" ");
    display_.print(steeringPulseInMicroSec);
    display_.print(" ");
    display_.println(isCarSteeringEnabled ? AppConfig::Text::kDisplayCarSteering
                                          : AppConfig::Text::kDisplayReceiverSteering);
  } else {
    display_.print(AppConfig::Text::kShortNoSignal);
    display_.print(" ");
    display_.print(steeringPulseInMicroSec);
    display_.print(" ");
    display_.println(isCarSteeringEnabled ? AppConfig::Text::kDisplayCarSteering
                                          : AppConfig::Text::kDisplayReceiverSteering);
  }

  display_.setCursor(0, 48);
  display_.print(AppConfig::Text::kDisplayModeLabel);
  display_.println(isRemoteRequiredForDrive ? AppConfig::Text::kDisplayModeRemoteCheck
                                            : AppConfig::Text::kDisplayModeNoCheck);
  
  display_.display();
}
