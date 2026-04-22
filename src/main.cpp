#include <Arduino.h>
#include <Wire.h>
#include "AppConfig.h"
#include "DisplayController.h"
#include "EscController.h"
#include "RcInputController.h"
#include "RcSignalValidator.h"
#include "SerialController.h"

const byte ch1Pin = AppConfig::Pins::kChannel1;
const byte ch2Pin = AppConfig::Pins::kChannel2;
const byte ch3Pin = AppConfig::Pins::kChannel3;

unsigned long ch1 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch2 = AppConfig::Rc::kDefaultPulseInMicroSec;
unsigned long ch3 = AppConfig::Rc::kDefaultPulseInMicroSec;

SerialController serialMonitor;
DisplayController display;
EscController esc(AppConfig::Pins::kEscSignal);
RcInputController rcInput(ch1Pin, ch2Pin, ch3Pin);
RcSignalValidator rcSignalValidator;



void setup() {
  serialMonitor.begin();
  Wire.begin();
  rcInput.begin();
  esc.begin();
  DisplayBeginResult displayStatus = display.begin();

  if (!displayStatus.ok) {
    serialMonitor.logLine(displayStatus.message);
    while (true) {}
  }

  serialMonitor.logLine(AppConfig::Text::kRcMonitorStarted);
}

void loop() {
  RcChannelSnapshot rcSnapshot = rcInput.read();
  RcChannelState rcState = rcSignalValidator.validate(rcSnapshot);

  ch1 = rcState.ch1;
  ch2 = rcState.ch2;
  ch3 = rcState.ch3;

  EscUpdateResult escResult = esc.update(ch1);
  if (escResult.isSuccess) {
    serialMonitor.logValue(escResult.value);
  } else if (escResult.message != nullptr) {
    serialMonitor.logLine(escResult.message);
  }

  display.renderChannels(ch1, ch2, ch3, escResult.message);

  delay(AppConfig::MainLoop::kDelayInMilliSec);
}
