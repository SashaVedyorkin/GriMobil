#include "RcSignalValidator.h"

RcSignalValidator::RcSignalValidator(unsigned long signalTimeoutInMicroSec,
                                     unsigned long signalMinInMicroSec,
                                     unsigned long signalMaxInMicroSec)
    : signalTimeoutInMicroSec_(signalTimeoutInMicroSec),
      signalMinInMicroSec_(signalMinInMicroSec),
      signalMaxInMicroSec_(signalMaxInMicroSec),
      lastValidCh1InMicroSec_(0),
      lastValidCh2InMicroSec_(0),
      lastValidCh3InMicroSec_(0) {}

RcChannelState RcSignalValidator::validate(const RcChannelSnapshot& snapshot) {
  RcChannelState state;
  state.ch1 = validateChannel(snapshot.ch1PulseInMicroSec, snapshot.ch1PulseAgeInMicroSec,
                              lastValidCh1InMicroSec_);
  state.ch2 = validateChannel(snapshot.ch2PulseInMicroSec, snapshot.ch2PulseAgeInMicroSec,
                              lastValidCh2InMicroSec_);
  state.ch3 = validateChannel(snapshot.ch3PulseInMicroSec, snapshot.ch3PulseAgeInMicroSec,
                              lastValidCh3InMicroSec_);
  return state;
}

unsigned long RcSignalValidator::validateChannel(unsigned long pulseInMicroSec,
                                                 unsigned long pulseAgeInMicroSec,
                                                 unsigned long& lastValidPulseInMicroSec) {
  if (pulseAgeInMicroSec <= signalTimeoutInMicroSec_ &&
      pulseInMicroSec > signalMinInMicroSec_ &&
      pulseInMicroSec < signalMaxInMicroSec_) {
    lastValidPulseInMicroSec = pulseInMicroSec;
    return pulseInMicroSec;
  }

  if (pulseAgeInMicroSec <= signalTimeoutInMicroSec_ && lastValidPulseInMicroSec != 0) {
    return lastValidPulseInMicroSec;
  }

  lastValidPulseInMicroSec = 0;
  return 0;
}
