#include <Arduino.h>
#include <Wire.h>
#include "AppConfig.h"
#include "BatteryVoltageController.h"
#include "DisplayController.h"
#include "DriveController.h"
#include "DriveModeButtonController.h"
#include "EscController.h"
#include "Lcd1602Controller.h"
#include "RcInputController.h"
#include "RcSignalValidator.h"
#include "SerialController.h"
#include "SpeedSensorController.h"
#include "SteeringCorrector.h"
#include "SteeringController.h"
#include "SteeringInputController.h"

unsigned long ch1 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch2 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch3 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch4 = AppConfig::Rc::kDefaultPulseInMicroSec;
bool isRemoteRequiredForDrive = false;
bool isTrackSensorActive = false;
float currentSpeedInKiloMetersPerHour = 0.0f;
int hallThrottleRawValue = 0;
unsigned long lastCh1NoSignalAtInMilliSec = 0;

SerialController serialMonitor;
DisplayController display;
Lcd1602Controller lcd1602;
DriveController drive;
EscController esc(AppConfig::Pca9685::kEscChannel);
SteeringController steering(AppConfig::Pca9685::kSteeringChannel);
SpeedSensorController speedSensor(AppConfig::Pins::kTrackSensor);
BatteryVoltageController batteryVoltage(AppConfig::Pins::kBatteryVoltage);
SteeringCorrector steeringCorrector;
SteeringInputController steeringInput(AppConfig::Pins::kSteeringPotentiometer,
                                      steeringCorrector);
DriveModeButtonController driveModeButton(AppConfig::Pins::kDriveForwardButton,
                                          AppConfig::Pins::kDriveReverseButton);
RcInputController rcInput(AppConfig::Pins::kChannel1, AppConfig::Pins::kChannel2,
                          AppConfig::Pins::kChannel3, AppConfig::Pins::kChannel4);
RcSignalValidator rcSignalValidator;

unsigned long lastMotorUpdateInMilliSec = 0;
unsigned long lastDisplayUpdateInMilliSec = 0;
unsigned long lastSerialUpdateInMilliSec = 0;

namespace {
bool readDriveSafetyToggle() {
  return digitalRead(AppConfig::Pins::kDriveSafetyToggle) == HIGH;
}

bool isRemoteDriveSignalAvailable(unsigned long throttlePulseInMicroSec,
                                  unsigned long directionPulseInMicroSec) {
  return throttlePulseInMicroSec != 0 && directionPulseInMicroSec != 0;
}
}

void setup() {
  serialMonitor.begin();
  Wire.begin();
  pinMode(AppConfig::Pins::kHallThrottleSensor, INPUT);
  pinMode(AppConfig::Pins::kDriveSafetyToggle, INPUT);
  steeringInput.begin();
  driveModeButton.begin();
  speedSensor.begin();
  batteryVoltage.begin();
  rcInput.begin();
  esc.begin();
  steering.begin();
  lcd1602.begin();
  DisplayBeginResult displayStatus = display.begin();

  if (!displayStatus.ok) {
    serialMonitor.logLine(displayStatus.message);
    while (true) {}
  }

  serialMonitor.logLine(AppConfig::Text::kRcMonitorStarted);
}

void loop() {
  unsigned long nowInMilliSec = millis();
  RcChannelSnapshot rcSnapshot = rcInput.read();
  RcChannelState rcState = rcSignalValidator.validate(rcSnapshot);
  SpeedSensorSnapshot speedSnapshot = speedSensor.read();
  BatteryVoltageSnapshot batteryVoltageSnapshot = batteryVoltage.read();

  ch1 = rcState.ch1;
  ch2 = rcState.ch2;
  ch3 = rcState.ch3;
  ch4 = rcState.ch4;
  isRemoteRequiredForDrive = readDriveSafetyToggle();
  isTrackSensorActive = speedSnapshot.isActive;
  currentSpeedInKiloMetersPerHour = speedSnapshot.speedInKiloMetersPerHour;
  hallThrottleRawValue = analogRead(AppConfig::Pins::kHallThrottleSensor);

  unsigned long driveDirectionButtonPulseInMicroSec = driveModeButton.readOverridePulseInMicroSec();
  if (driveDirectionButtonPulseInMicroSec != 0) {
    ch3 = driveDirectionButtonPulseInMicroSec;
  }

  if (ch1 == 0) {
    lastCh1NoSignalAtInMilliSec = nowInMilliSec;
  }

  bool isRemoteDriveSignalAvailableNow = isRemoteDriveSignalAvailable(ch1, ch3);
  DriveUpdateResult driveResult = drive.update(ch1, ch3);

  if (isRemoteRequiredForDrive && !isRemoteDriveSignalAvailableNow) {
    driveResult = {AppConfig::Text::kDriveNoSignal, AppConfig::Esc::kNeutralPulseInMicroSec,
                   false};
  }

  SteeringInputResult steeringInputResult = steeringInput.resolve(ch2, ch4, isRemoteRequiredForDrive);
  unsigned long steeringPulseInMicroSec = steering.update(steeringInputResult.pulseInMicroSec);
  EscUpdateResult escResult = {AppConfig::Esc::kNeutralPulseInMicroSec};

  if (nowInMilliSec - lastMotorUpdateInMilliSec >=
      AppConfig::MainLoop::kMotorUpdateIntervalInMilliSec) {
    lastMotorUpdateInMilliSec = nowInMilliSec;
    escResult = esc.update(driveResult.escPulseInMicroSec);
  }

  if (nowInMilliSec - lastDisplayUpdateInMilliSec >=
      AppConfig::MainLoop::kDisplayUpdateIntervalInMilliSec) {
    const char* lcdStateText = AppConfig::Text::kLcdStateCorrect;

    if (!isRemoteRequiredForDrive) {
      lcdStateText = AppConfig::Text::kLcdStateNoCheckMode;
    } else if (ch1 == 0 ||
               (lastCh1NoSignalAtInMilliSec != 0 &&
                nowInMilliSec - lastCh1NoSignalAtInMilliSec <=
                    AppConfig::Lcd1602::kCh1NoSignalHoldInMilliSec)) {
      lcdStateText = AppConfig::Text::kLcdStateCh1NoSignal;
    }

    lastDisplayUpdateInMilliSec = nowInMilliSec;
    display.renderChannels(ch1, ch2, ch3,
                           steeringInputResult.isEffectiveCarSteeringEnabled,
                           isRemoteRequiredForDrive,
                           hallThrottleRawValue, lcdStateText,
                           escResult.value,
                           steeringPulseInMicroSec,
                           driveResult.statusMessage);
    lcd1602.render(currentSpeedInKiloMetersPerHour, batteryVoltageSnapshot);
  }
}
