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

unsigned long ch1 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch2 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch3 = AppConfig::Rc::kDefaultPulseInMicroSec;

SerialController serialMonitor;
DisplayController display;
DriveController drive;
EscController esc(AppConfig::Pins::kEscSignal);
SteeringController steering(AppConfig::Pins::kSteeringSignal);
RcInputController rcInput(ch1Pin, ch2Pin, ch3Pin);
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

bool useReceiverSteering(unsigned long receiverPulseInMicroSec) {
  if (receiverPulseInMicroSec == 0) {
    return false;
  }

  unsigned long deltaFromCenterInMicroSec =
      receiverPulseInMicroSec > AppConfig::Steering::kInputCenterInMicroSec
          ? receiverPulseInMicroSec - AppConfig::Steering::kInputCenterInMicroSec
          : AppConfig::Steering::kInputCenterInMicroSec - receiverPulseInMicroSec;

  return deltaFromCenterInMicroSec >= AppConfig::Steering::kReceiverOverrideDeltaInMicroSec;
}
}  // namespace


void setup() {
  serialMonitor.begin();
  Wire.begin();
  pinMode(AppConfig::Pins::kSteeringPotentiometer, INPUT);
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

  DriveUpdateResult driveResult = drive.update(ch1, ch3);
  unsigned long steeringInputPulseInMicroSec = readSteeringPotentiometerPulseInMicroSec();
  if (useReceiverSteering(ch2)) {
    steeringInputPulseInMicroSec = ch2;
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
    display.renderChannels(ch1, ch2, ch3, escResult.value, steeringPulseInMicroSec,
                           driveResult.statusMessage);
  }
}
