#include "RcChannelReadStrategy.h"

namespace {
InterruptChannelReadStrategy* g_interruptStrategies[2] = {nullptr, nullptr};

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
  } else {
    attachInterrupt(digitalPinToInterrupt(pin_), handleInterruptSlot1, CHANGE);
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

PulseInChannelReadStrategy::PulseInChannelReadStrategy(uint8_t pin)
    : pin_(pin), pulseInMicroSec_(AppConfig::Rc::kDefaultPulseInMicroSec), lastPulseInMicroSec_(0) {}

void PulseInChannelReadStrategy::begin() {
  pinMode(pin_, INPUT);
}

RcChannelMeasurement PulseInChannelReadStrategy::read() {
  unsigned long measuredPulseInMicroSec =
      pulseIn(pin_, HIGH, AppConfig::Rc::kPulseTimeoutInMicroSec);

  if (measuredPulseInMicroSec >= AppConfig::Rc::kPulseMinInMicroSec &&
      measuredPulseInMicroSec <= AppConfig::Rc::kPulseMaxInMicroSec) {
    pulseInMicroSec_ = measuredPulseInMicroSec;
    lastPulseInMicroSec_ = micros();
  }

  return {pulseInMicroSec_, micros() - lastPulseInMicroSec_};
}
