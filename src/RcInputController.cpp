#include "RcInputController.h"

RcInputController::RcInputController(uint8_t ch1Pin, uint8_t ch2Pin, uint8_t ch3Pin,
                                     uint8_t ch4Pin)
    : ch1Strategy_(ch1Pin, 0),
      ch2Strategy_(ch2Pin, 1),
      ch3Strategy_(ch3Pin, 2),
      ch4Strategy_(ch4Pin, 3) {}

void RcInputController::begin() {
  ch1Strategy_.begin();
  ch2Strategy_.begin();
  ch3Strategy_.begin();
  ch4Strategy_.begin();
}

RcChannelSnapshot RcInputController::read() {
  RcChannelMeasurement ch1Measurement = ch1Strategy_.read();
  RcChannelMeasurement ch2Measurement = ch2Strategy_.read();
  RcChannelMeasurement ch3Measurement = ch3Strategy_.read();
  RcChannelMeasurement ch4Measurement = ch4Strategy_.read();

  return {ch1Measurement.pulseInMicroSec,     ch2Measurement.pulseInMicroSec,
          ch3Measurement.pulseInMicroSec,     ch4Measurement.pulseInMicroSec,
          ch1Measurement.pulseAgeInMicroSec,  ch2Measurement.pulseAgeInMicroSec,
          ch3Measurement.pulseAgeInMicroSec,  ch4Measurement.pulseAgeInMicroSec};
}
