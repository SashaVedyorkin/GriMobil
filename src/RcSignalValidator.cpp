#include "RcSignalValidator.h"

RcSignalValidator::RcSignalValidator(unsigned long signalTimeoutInMicroSec,
                                     unsigned long signalMinInMicroSec,
                                     unsigned long signalMaxInMicroSec)
    : signalTimeoutInMicroSec_(signalTimeoutInMicroSec),
      signalMinInMicroSec_(signalMinInMicroSec),
      signalMaxInMicroSec_(signalMaxInMicroSec) {}

RcChannelState RcSignalValidator::validate(const RcChannelSnapshot& snapshot) const {
  RcChannelState state;
  state.ch1 = validateChannel(snapshot.ch1PulseInMicroSec, snapshot.ch1PulseAgeInMicroSec);
  state.ch2 = validateChannel(snapshot.ch2PulseInMicroSec, snapshot.ch2PulseAgeInMicroSec);
  state.ch3 = validateChannel(snapshot.ch3PulseInMicroSec, snapshot.ch3PulseAgeInMicroSec);
  return state;
}

unsigned long RcSignalValidator::validateChannel(unsigned long pulseInMicroSec,
                                                 unsigned long pulseAgeInMicroSec) const {
  if (pulseAgeInMicroSec <= signalTimeoutInMicroSec_ &&
      pulseInMicroSec > signalMinInMicroSec_ &&
      pulseInMicroSec < signalMaxInMicroSec_) {
    return pulseInMicroSec;
  }

  return 0;
}
