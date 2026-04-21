#include "RcSignalValidator.h"

RcSignalValidator::RcSignalValidator(unsigned long signalTimeoutInMicroSec,
                                     unsigned long signalMinInMicroSec,
                                     unsigned long signalMaxInMicroSec)
    : signalTimeoutInMicroSec_(signalTimeoutInMicroSec),
      signalMinInMicroSec_(signalMinInMicroSec),
      signalMaxInMicroSec_(signalMaxInMicroSec) {}

RcChannelState RcSignalValidator::validate(const RcChannelSnapshot& snapshot) const {
  return {validateChannel(snapshot.ch1PulseInMicroSec, snapshot.ch1PulseAgeInMicroSec),
          validateChannel(snapshot.ch2PulseInMicroSec, snapshot.ch2PulseAgeInMicroSec)};
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
