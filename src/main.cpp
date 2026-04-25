#include <Arduino.h>
#include <Wire.h>
#include "AppConfig.h"
#include "DisplayController.h"
#include "DriveController.h"
#include "EscController.h"
#include "RcInputController.h"
#include "RcSignalValidator.h"
#include "SerialController.h"
#include "SteeringController.h"

const byte ch1Pin = AppConfig::Pins::kChannel1;
const byte ch2Pin = AppConfig::Pins::kChannel2;
const byte ch3Pin = AppConfig::Pins::kChannel3;
const byte ch4Pin = AppConfig::Pins::kChannel4;
const byte driveSafetyTogglePin = AppConfig::Pins::kDriveSafetyToggle;

unsigned long ch1 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch2 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch3 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch4 = AppConfig::Rc::kDefaultPulseInMicroSec;
bool isCarSteeringEnabled = true;
bool wasReceiverSteeringLockActive = false;
bool isRemoteRequiredForDrive = false;

SerialController serialMonitor;
DisplayController display;
DriveController drive;
EscController esc(AppConfig::Pins::kEscSignal);
SteeringController steering(AppConfig::Pins::kSteeringSignal);
RcInputController rcInput(ch1Pin, ch2Pin, ch3Pin, ch4Pin);
RcSignalValidator rcSignalValidator;

unsigned long lastMotorUpdateInMilliSec = 0;
unsigned long lastDisplayUpdateInMilliSec = 0;
unsigned long lastSerialUpdateInMilliSec = 0;

namespace {
unsigned long readSteeringPotentiometerPulseInMicroSec() {
  int potentiometerValue = analogRead(AppConfig::Pins::kSteeringPotentiometer);
  return map(potentiometerValue, AppConfig::Potentiometer::kAnalogMin,
             AppConfig::Potentiometer::kAnalogMax, AppConfig::Steering::kInputRightMaxInMicroSec,
             AppConfig::Steering::kInputLeftMaxInMicroSec);
}

long getSteeringOffsetFromCenterInMicroSec(unsigned long pulseInMicroSec) {
  return static_cast<long>(pulseInMicroSec) -
         static_cast<long>(AppConfig::Steering::kInputCenterInMicroSec);
}

unsigned long getSteeringMagnitudeInMicroSec(long offsetInMicroSec) {
  return offsetInMicroSec >= 0 ? static_cast<unsigned long>(offsetInMicroSec)
                               : static_cast<unsigned long>(-offsetInMicroSec);
}

bool useReceiverSteering(unsigned long potentiometerPulseInMicroSec,
                         unsigned long receiverPulseInMicroSec) {
  if (receiverPulseInMicroSec == 0) {
    return false;
  }

  long receiverOffsetInMicroSec = getSteeringOffsetFromCenterInMicroSec(receiverPulseInMicroSec);
  unsigned long receiverDeltaFromCenterInMicroSec =
      getSteeringMagnitudeInMicroSec(receiverOffsetInMicroSec);

  if (receiverDeltaFromCenterInMicroSec < AppConfig::Steering::kReceiverOverrideDeltaInMicroSec) {
    return false;
  }

  long potentiometerOffsetInMicroSec =
      getSteeringOffsetFromCenterInMicroSec(potentiometerPulseInMicroSec);
  unsigned long potentiometerDeltaFromCenterInMicroSec =
      getSteeringMagnitudeInMicroSec(potentiometerOffsetInMicroSec);

  bool isSameDirection =
      (receiverOffsetInMicroSec > 0 && potentiometerOffsetInMicroSec > 0) ||
      (receiverOffsetInMicroSec < 0 && potentiometerOffsetInMicroSec < 0);

  if (isSameDirection &&
      receiverDeltaFromCenterInMicroSec < potentiometerDeltaFromCenterInMicroSec) {
    return false;
  }

  return true;
}

bool isReceiverSteeringLockActive(unsigned long channel4PulseInMicroSec) {
  return channel4PulseInMicroSec != 0 &&
         channel4PulseInMicroSec <= AppConfig::Steering::kReceiverLockThresholdInMicroSec;
}

void updateSteeringControlMode(unsigned long channel4PulseInMicroSec) {
  bool isReceiverSteeringLockActiveNow =
      isReceiverSteeringLockActive(channel4PulseInMicroSec);

  if (isReceiverSteeringLockActiveNow && !wasReceiverSteeringLockActive) {
    isCarSteeringEnabled = !isCarSteeringEnabled;
  }

  wasReceiverSteeringLockActive = isReceiverSteeringLockActiveNow;
}

bool readDriveSafetyToggle() {
  return digitalRead(driveSafetyTogglePin) == HIGH;
}

bool isRemoteDriveSignalAvailable(unsigned long throttlePulseInMicroSec,
                                  unsigned long directionPulseInMicroSec) {
  return throttlePulseInMicroSec != 0 && directionPulseInMicroSec != 0;
}

}


void setup() {
  serialMonitor.begin();
  Wire.begin();
  pinMode(AppConfig::Pins::kSteeringPotentiometer, INPUT);
  pinMode(driveSafetyTogglePin, INPUT);
  rcInput.begin();
  esc.begin();
  steering.begin();
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

  ch1 = rcState.ch1;
  ch2 = rcState.ch2;
  ch3 = rcState.ch3;
  ch4 = rcState.ch4;
  isRemoteRequiredForDrive = readDriveSafetyToggle();

  bool isRemoteDriveSignalAvailableNow = isRemoteDriveSignalAvailable(ch1, ch3);
  DriveUpdateResult driveResult = drive.update(ch1, ch3);

  if (isRemoteRequiredForDrive && !isRemoteDriveSignalAvailableNow) {
    driveResult = {AppConfig::Text::kDriveNoSignal, AppConfig::Esc::kNeutralPulseInMicroSec,
                   false};
  }

  updateSteeringControlMode(ch4);
  unsigned long steeringInputPulseInMicroSec = AppConfig::Steering::kInputCenterInMicroSec;

  if (!isCarSteeringEnabled) {
    if (ch2 != 0) {
      steeringInputPulseInMicroSec = ch2;
    }
  } else {
    steeringInputPulseInMicroSec = readSteeringPotentiometerPulseInMicroSec();
    if (useReceiverSteering(steeringInputPulseInMicroSec, ch2)) {
      steeringInputPulseInMicroSec = ch2;
    }
  }
  unsigned long steeringPulseInMicroSec = steering.update(steeringInputPulseInMicroSec);
  EscUpdateResult escResult = {AppConfig::Esc::kNeutralPulseInMicroSec};

  if (nowInMilliSec - lastMotorUpdateInMilliSec >=
      AppConfig::MainLoop::kMotorUpdateIntervalInMilliSec) {
    lastMotorUpdateInMilliSec = nowInMilliSec;
    escResult = esc.update(driveResult.escPulseInMicroSec);
  }

  if (nowInMilliSec - lastSerialUpdateInMilliSec >=
      AppConfig::MainLoop::kSerialUpdateIntervalInMilliSec) {
    lastSerialUpdateInMilliSec = nowInMilliSec;

    if (driveResult.isSignalValid) {
      //serialMonitor.logValue(escResult.value);
    } else {
      //serialMonitor.logLine(driveResult.statusMessage);
    }
  }

  if (nowInMilliSec - lastDisplayUpdateInMilliSec >=
      AppConfig::MainLoop::kDisplayUpdateIntervalInMilliSec) {
    lastDisplayUpdateInMilliSec = nowInMilliSec;
    display.renderChannels(ch1, ch2, ch3, isCarSteeringEnabled, isRemoteRequiredForDrive,
                           escResult.value,
                           steeringPulseInMicroSec,
                           driveResult.statusMessage);
  }
}
