#include "RcChannelReadStrategy.h"

namespace {
InterruptChannelReadStrategy* g_interruptStrategies[2] = {nullptr, nullptr};
PinChangeInterruptChannelReadStrategy* g_pinChangeInterruptStrategy = nullptr;

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

ISR(PCINT2_vect) {
  if (g_pinChangeInterruptStrategy != nullptr) {
    g_pinChangeInterruptStrategy->handleInterrupt();
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

PinChangeInterruptChannelReadStrategy::PinChangeInterruptChannelReadStrategy(uint8_t pin)
    : pin_(pin),
      inputRegister_(nullptr),
      bitMask_(digitalPinToBitMask(pin)),
      lastSignalHigh_(false),
      riseInMicroSec_(0),
      pulseInMicroSec_(AppConfig::Rc::kDefaultPulseInMicroSec),
      lastPulseInMicroSec_(0) {}

void PinChangeInterruptChannelReadStrategy::begin() {
  pinMode(pin_, INPUT);
  inputRegister_ = portInputRegister(digitalPinToPort(pin_));
  lastSignalHigh_ = ((*inputRegister_) & bitMask_) != 0;
  g_pinChangeInterruptStrategy = this;

  *digitalPinToPCICR(pin_) |= _BV(digitalPinToPCICRbit(pin_));
  *digitalPinToPCMSK(pin_) |= _BV(digitalPinToPCMSKbit(pin_));
}

RcChannelMeasurement PinChangeInterruptChannelReadStrategy::read() {
  RcChannelMeasurement measurement = {0, 0};
  unsigned long nowInMicroSec = micros();

  noInterrupts();
  measurement.pulseInMicroSec = pulseInMicroSec_;
  measurement.pulseAgeInMicroSec = nowInMicroSec - lastPulseInMicroSec_;
  interrupts();

  return measurement;
}

void PinChangeInterruptChannelReadStrategy::handleInterrupt() {
  bool signalHigh = ((*inputRegister_) & bitMask_) != 0;

  if (signalHigh == lastSignalHigh_) {
    return;
  }

  lastSignalHigh_ = signalHigh;
  unsigned long nowInMicroSec = micros();

  if (signalHigh) {
    riseInMicroSec_ = nowInMicroSec;
    return;
  }

  unsigned long measuredPulseInMicroSec = nowInMicroSec - riseInMicroSec_;
  if (measuredPulseInMicroSec >= AppConfig::Rc::kPulseMinInMicroSec &&
      measuredPulseInMicroSec <= AppConfig::Rc::kPulseMaxInMicroSec) {
    pulseInMicroSec_ = measuredPulseInMicroSec;
    lastPulseInMicroSec_ = nowInMicroSec;
  }
}
