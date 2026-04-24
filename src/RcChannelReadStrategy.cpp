#include "RcChannelReadStrategy.h"

namespace {
InterruptChannelReadStrategy* g_interruptStrategies[4] = {nullptr, nullptr, nullptr, nullptr};

void handleInterruptSlot0() {
  if (g_interruptStrategies[0] != nullptr) {
    g_interruptStrategies[0]->handleInterrupt();
  }
}

void handleInterruptSlot1() {
  if (g_interruptStrategies[1] != nullptr) {
    g_interruptStrategies[1]->handleInterrupt();
  }
}

void handleInterruptSlot2() {
  if (g_interruptStrategies[2] != nullptr) {
    g_interruptStrategies[2]->handleInterrupt();
  }
}

void handleInterruptSlot3() {
  if (g_interruptStrategies[3] != nullptr) {
    g_interruptStrategies[3]->handleInterrupt();
  }
}
}  // namespace

InterruptChannelReadStrategy::InterruptChannelReadStrategy(uint8_t pin, uint8_t slot)
    : pin_(pin),
      slot_(slot),
      riseInMicroSec_(0),
      pulseInMicroSec_(AppConfig::Rc::kDefaultPulseInMicroSec),
      lastPulseInMicroSec_(0) {}

void InterruptChannelReadStrategy::begin() {
  pinMode(pin_, INPUT);
  g_interruptStrategies[slot_] = this;

  if (slot_ == 0) {
    attachInterrupt(digitalPinToInterrupt(pin_), handleInterruptSlot0, CHANGE);
  } else if (slot_ == 1) {
    attachInterrupt(digitalPinToInterrupt(pin_), handleInterruptSlot1, CHANGE);
  } else if (slot_ == 2) {
    attachInterrupt(digitalPinToInterrupt(pin_), handleInterruptSlot2, CHANGE);
  } else {
    attachInterrupt(digitalPinToInterrupt(pin_), handleInterruptSlot3, CHANGE);
  }
}

RcChannelMeasurement InterruptChannelReadStrategy::read() {
  RcChannelMeasurement measurement = {0, 0};
  unsigned long nowInMicroSec = micros();

  noInterrupts();
  measurement.pulseInMicroSec = pulseInMicroSec_;
  measurement.pulseAgeInMicroSec = nowInMicroSec - lastPulseInMicroSec_;
  interrupts();

  return measurement;
}

void InterruptChannelReadStrategy::handleInterrupt() {
  unsigned long nowInMicroSec = micros();

  if (digitalRead(pin_) == HIGH) {
    riseInMicroSec_ = nowInMicroSec;
  } else {
    unsigned long measuredPulseInMicroSec = nowInMicroSec - riseInMicroSec_;

    if (measuredPulseInMicroSec >= AppConfig::Rc::kPulseMinInMicroSec &&
        measuredPulseInMicroSec <= AppConfig::Rc::kPulseMaxInMicroSec) {
      pulseInMicroSec_ = measuredPulseInMicroSec;
      lastPulseInMicroSec_ = nowInMicroSec;
    }
  }
}
