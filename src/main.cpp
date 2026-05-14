#include <Arduino.h>
#include <Wire.h>
#include "AppConfig.h"
#include "BatteryVoltageController.h"
#include "DisplayController.h"
#include "DisplaySnapshot.h"
#include "DriveController.h"
#include "DriveModeButtonController.h"
#include "EscController.h"
#include "Lcd1602Controller.h"
#include "MedianFilter.h"
#include "RcInputController.h"
#include "RcSignalValidator.h"
#include "SerialController.h"
#include "SpeedSensorController.h"
#include "SteeringCorrector.h"
#include "SteeringController.h"
#include "SteeringInputController.h"
#include "SystemStateController.h"

unsigned long ch1 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch2 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch3 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch4 = AppConfig::Rc::kDefaultPulseInMicroSec;
bool isRemoteRequiredForDrive = false;
int hallThrottleRawValue = 0;

SerialController serialMonitor;
DisplayController display;
Lcd1602Controller lcd1602;
DriveController drive;
EscController esc(AppConfig::Pca9685::kEscChannel);
SteeringController steering(AppConfig::Pca9685::kSteeringChannel);
SpeedSensorController speedSensor;
BatteryVoltageController batteryVoltage(AppConfig::Pins::kBatteryVoltage);
SteeringCorrector steeringCorrector;
SteeringInputController steeringInput(AppConfig::Pins::kSteeringPotentiometer,
                                      steeringCorrector);
DriveModeButtonController driveModeButton(AppConfig::Pins::kDriveForwardButton,
                                          AppConfig::Pins::kDriveReverseButton);
RcInputController rcInput(AppConfig::Pins::kChannel1, AppConfig::Pins::kChannel2,
                          AppConfig::Pins::kChannel3, AppConfig::Pins::kChannel4);
RcSignalValidator rcSignalValidator;
SystemStateController systemState;

unsigned long lastMotorUpdateInMilliSec = 0;
unsigned long lastDisplayUpdateInMilliSec = 0;
unsigned long lastBatteryUpdateInMilliSec = 0;
unsigned long lastAppliedEscPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
BatteryVoltageSnapshot lastBatteryVoltageSnapshot = {false, 0.0f};
MedianFilter<AppConfig::Rc::kMedianFilterSampleCount> ch1MedianFilter;
MedianFilter<AppConfig::Rc::kMedianFilterSampleCount> ch2MedianFilter;

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
}

void loop() {
  unsigned long nowInMilliSec = millis();
  RcChannelSnapshot rcSnapshot = rcInput.read();
  RcChannelState rcState = rcSignalValidator.validate(rcSnapshot);
  SpeedSensorSnapshot speedSnapshot = speedSensor.read();

  ch1 = rcState.ch1;
  ch2 = rcState.ch2;
  ch3 = rcState.ch3;
  ch4 = rcState.ch4;
  ch1 = ch1MedianFilter.apply(ch1);
  ch2 = ch2MedianFilter.apply(ch2);
  isRemoteRequiredForDrive = readDriveSafetyToggle();
  hallThrottleRawValue = analogRead(AppConfig::Pins::kHallThrottleSensor);

  SteeringInputResult steeringInputResult =
      steeringInput.resolve(ch2, ch4, isRemoteRequiredForDrive);
  bool isLocalChildControlsEnabled = steeringInputResult.isEffectiveCarSteeringEnabled;

  unsigned long driveDirectionButtonPulseInMicroSec =
      driveModeButton.readOverridePulseInMicroSec();
  if (isLocalChildControlsEnabled && driveDirectionButtonPulseInMicroSec != 0) {
    ch3 = driveDirectionButtonPulseInMicroSec;
  }

  bool isRemoteDriveSignalAvailableNow = isRemoteDriveSignalAvailable(ch1, ch3);
  bool isRemoteThrottleSignalAvailableNow = ch1 != 0;
  DriveUpdateResult driveResult =
      drive.update(hallThrottleRawValue, ch1, ch3, isLocalChildControlsEnabled);

  if (isRemoteRequiredForDrive && !isRemoteDriveSignalAvailableNow) {
    driveResult = {AppConfig::Text::kDriveNoSignal, driveResult.driveDisplayLabel,
                   driveResult.throttleSourceLabel,
                   AppConfig::Esc::kNeutralPulseInMicroSec, false};
  }

  unsigned long steeringPulseInMicroSec = steering.update(steeringInputResult.pulseInMicroSec);

  if (nowInMilliSec - lastMotorUpdateInMilliSec >=
      AppConfig::MainLoop::kMotorUpdateIntervalInMilliSec) {
    lastMotorUpdateInMilliSec = nowInMilliSec;
    EscUpdateResult escResult = esc.update(driveResult.escPulseInMicroSec);
    lastAppliedEscPulseInMicroSec = escResult.value;
  }

  if (nowInMilliSec - lastBatteryUpdateInMilliSec >=
      AppConfig::MainLoop::kBatteryUpdateIntervalInMilliSec) {
    lastBatteryUpdateInMilliSec = nowInMilliSec;
    lastBatteryVoltageSnapshot = batteryVoltage.read();
  }

  if (nowInMilliSec - lastDisplayUpdateInMilliSec >=
      AppConfig::MainLoop::kDisplayUpdateIntervalInMilliSec) {
    lastDisplayUpdateInMilliSec = nowInMilliSec;
    const char* lcdStateText = systemState.resolveLcdStateText(
        isRemoteRequiredForDrive, isRemoteThrottleSignalAvailableNow, nowInMilliSec);

    OledDisplaySnapshot oledSnapshot = {
        ch1,
        ch2,
        ch3,
        steeringInputResult.isEffectiveCarSteeringEnabled,
        isRemoteRequiredForDrive,
        hallThrottleRawValue,
        lcdStateText,
        lastAppliedEscPulseInMicroSec,
        steeringPulseInMicroSec,
        driveResult.driveDisplayLabel,
        driveResult.throttleSourceLabel};
    display.render(oledSnapshot);

    Lcd1602Snapshot lcdSnapshot = {speedSnapshot.speedInKiloMetersPerHour,
                                   lastBatteryVoltageSnapshot};
    lcd1602.render(lcdSnapshot);
  }
}
