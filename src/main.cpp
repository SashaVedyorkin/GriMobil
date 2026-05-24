#include <Arduino.h>
#include <Wire.h>
#include "AppConfig.h"
#include "BatteryVoltageController.h"
#include "BrakeLightController.h"
#include "DisplayController.h"
#include "DisplaySnapshot.h"
#include "DriveController.h"
#include "DriveModeButtonController.h"
#include "EscController.h"
#include "Lcd1602Controller.h"
#include "LightController.h"
#include "MedianFilter.h"
#include "OledAnnouncementController.h"
#include "RcInputController.h"
#include "RcSignalValidator.h"
#include "SerialController.h"
#include "SpeedSensorController.h"
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
LightController frontLight(AppConfig::Pins::kFrontLightOutput);
LightController rearLight(AppConfig::Pins::kRearLightOutput);
BrakeLightController brakeLight;
SpeedSensorController speedSensor;
BatteryVoltageController batteryVoltage(AppConfig::Pins::kBatteryVoltage);
SteeringInputController steeringInput(AppConfig::Pins::kSteeringPotentiometer);
DriveModeButtonController driveModeButton(AppConfig::Pins::kDriveForwardButton,
                                          AppConfig::Pins::kDriveReverseButton);
RcInputController rcInput(AppConfig::Pins::kChannel1, AppConfig::Pins::kChannel2,
                          AppConfig::Pins::kChannel3, AppConfig::Pins::kChannel4);
RcSignalValidator rcSignalValidator;
SystemStateController systemState;
OledAnnouncementController oledAnnouncementController;

unsigned long lastMotorUpdateInMilliSec = 0;
unsigned long lastDisplayUpdateInMilliSec = 0;
unsigned long lastBatteryUpdateInMilliSec = 0;
unsigned long lastAppliedEscPulseInMicroSec = AppConfig::Esc::kNeutralPulseInMicroSec;
BatteryVoltageSnapshot lastBatteryVoltageSnapshot = {false, 0.0f};
bool lastFrontLightEnabled = false;
bool lastRemoteRequiredForDrive = false;
DriveModeController::DriveMode lastDriveMode = DriveModeController::kNeutral;
MedianFilter<AppConfig::Rc::kMedianFilterSampleCount> ch1MedianFilter;
MedianFilter<AppConfig::Rc::kMedianFilterSampleCount> ch2MedianFilter;

namespace {
bool readDriveSafetyToggle() {
  return digitalRead(AppConfig::Pins::kDriveSafetyToggle) == HIGH;
}

bool readFrontLightToggle() {
  return digitalRead(AppConfig::Pins::kFrontLightToggle) == LOW;
}

uint8_t calculateThrottlePercent(unsigned long escPulseInMicroSec) {
  unsigned long neutralPulse = AppConfig::Esc::kNeutralPulseInMicroSec;
  unsigned long delta = escPulseInMicroSec > neutralPulse ? escPulseInMicroSec - neutralPulse
                                                          : neutralPulse - escPulseInMicroSec;
  if (delta >= 500) {
    return 100;
  }

  return static_cast<uint8_t>((delta * 100UL) / 500UL);
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
  pinMode(AppConfig::Pins::kFrontLightToggle, INPUT_PULLUP);
  steeringInput.begin();
  driveModeButton.begin();
  speedSensor.begin();
  batteryVoltage.begin();
  rcInput.begin();
  esc.begin();
  steering.begin();
  frontLight.begin();
  rearLight.begin();
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
  frontLight.setEnabled(readFrontLightToggle());
  if (frontLight.isEnabled() != lastFrontLightEnabled) {
    if (frontLight.isEnabled()) {
      oledAnnouncementController.showTemporary(AppConfig::Text::kAnnouncementLightOn,
                                               nowInMilliSec);
    } else {
      oledAnnouncementController.showTemporary(AppConfig::Text::kAnnouncementLightOff,
                                               nowInMilliSec);
    }
    lastFrontLightEnabled = frontLight.isEnabled();
  }
  if (isRemoteRequiredForDrive && !lastRemoteRequiredForDrive) {
    oledAnnouncementController.showTemporary(AppConfig::Text::kAnnouncementRccMode,
                                             nowInMilliSec);
  }
  lastRemoteRequiredForDrive = isRemoteRequiredForDrive;

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
    driveResult = {driveResult.driveMode, AppConfig::Text::kDriveNoSignal,
                   driveResult.driveDisplayLabel, driveResult.throttleSourceLabel,
                   AppConfig::Esc::kNeutralPulseInMicroSec, false};
  }
  if (driveResult.driveMode != lastDriveMode) {
    if (driveResult.driveMode == DriveModeController::kForward) {
      oledAnnouncementController.showTemporary(AppConfig::Text::kAnnouncementDrive,
                                               nowInMilliSec);
    } else if (driveResult.driveMode == DriveModeController::kReverse) {
      oledAnnouncementController.showTemporary(AppConfig::Text::kAnnouncementReverse,
                                               nowInMilliSec);
    } else if (driveResult.driveMode == DriveModeController::kNeutral) {
      oledAnnouncementController.showTemporary(AppConfig::Text::kAnnouncementNeutral,
                                               nowInMilliSec);
    }
    lastDriveMode = driveResult.driveMode;
  }

  unsigned long steeringPulseInMicroSec = steering.update(steeringInputResult.pulseInMicroSec);

  if (nowInMilliSec - lastMotorUpdateInMilliSec >=
      AppConfig::MainLoop::kMotorUpdateIntervalInMilliSec) {
    lastMotorUpdateInMilliSec = nowInMilliSec;
    EscUpdateResult escResult = esc.update(driveResult.escPulseInMicroSec);
    lastAppliedEscPulseInMicroSec = escResult.value;
  }
  rearLight.setEnabled(
      brakeLight.update(lastAppliedEscPulseInMicroSec, speedSnapshot.speedInKiloMetersPerHour));

  if (nowInMilliSec - lastBatteryUpdateInMilliSec >=
      AppConfig::MainLoop::kBatteryUpdateIntervalInMilliSec) {
    lastBatteryUpdateInMilliSec = nowInMilliSec;
    lastBatteryVoltageSnapshot = batteryVoltage.read();
  }

  uint8_t batteryPercent =
      BatteryVoltageController::calculateChargePercent(lastBatteryVoltageSnapshot);
  if (isRemoteRequiredForDrive && !isRemoteDriveSignalAvailableNow) {
    oledAnnouncementController.setPersistent(AppConfig::Text::kAnnouncementNoRemote);
  } else if (lastBatteryVoltageSnapshot.isDefined && batteryPercent < 10) {
    oledAnnouncementController.setPersistent(AppConfig::Text::kAnnouncementLowBattery);
  } else {
    oledAnnouncementController.clearPersistent();
  }

  if (nowInMilliSec - lastDisplayUpdateInMilliSec >=
      AppConfig::MainLoop::kDisplayUpdateIntervalInMilliSec) {
    lastDisplayUpdateInMilliSec = nowInMilliSec;
    const char* lcdStateText = systemState.resolveLcdStateText(
        isRemoteRequiredForDrive, isRemoteThrottleSignalAvailableNow, nowInMilliSec);
    uint8_t throttlePercent = calculateThrottlePercent(lastAppliedEscPulseInMicroSec);

    OledDisplaySnapshot oledSnapshot = {
        ch1,
        ch2,
        ch3,
        steeringInputResult.potentiometerPulseInMicroSec,
        steeringInputResult.isEffectiveCarSteeringEnabled,
        isRemoteRequiredForDrive,
        hallThrottleRawValue,
        lcdStateText,
        lastAppliedEscPulseInMicroSec,
        steeringPulseInMicroSec,
        driveResult.driveDisplayLabel,
        driveResult.throttleSourceLabel,
        speedSnapshot.speedInKiloMetersPerHour,
        lastBatteryVoltageSnapshot,
        throttlePercent,
        frontLight.isEnabled(),
        oledAnnouncementController.currentMessage(nowInMilliSec)};
    display.render(oledSnapshot);

    Lcd1602Snapshot lcdSnapshot = {speedSnapshot.speedInKiloMetersPerHour,
                                   lastBatteryVoltageSnapshot,
                                   steeringInputResult.isEffectiveCarSteeringEnabled,
                                   isRemoteRequiredForDrive,
                                   throttlePercent,
                                   lcdStateText};
    lcd1602.render(lcdSnapshot);
  }
}
