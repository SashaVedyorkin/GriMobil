#include "RcInputController.h"

RcInputController* RcInputController::instance_ = nullptr;

RcInputController::RcInputController(uint8_t ch1Pin, uint8_t ch2Pin)
    : ch1Pin_(ch1Pin),
      ch2Pin_(ch2Pin),
      ch1BitMask_(digitalPinToBitMask(ch1Pin)),
      ch2BitMask_(digitalPinToBitMask(ch2Pin)),
      ch1InputRegister_(portInputRegister(digitalPinToPort(ch1Pin))),
      ch2InputRegister_(portInputRegister(digitalPinToPort(ch2Pin))),
      ch1RiseInMicroSec_(0),
      ch2RiseInMicroSec_(0),
      ch1PulseInMicroSec_(1500),
      ch2PulseInMicroSec_(1500),
      ch1LastPulseInMicroSec_(0),
      ch2LastPulseInMicroSec_(0),
      ch1HistoryA_(1500),
      ch1HistoryB_(1500),
      ch1HistoryC_(1500),
      ch2HistoryA_(1500),
      ch2HistoryB_(1500),
      ch2HistoryC_(1500) {}

void RcInputController::begin() {
  pinMode(ch1Pin_, INPUT);
  pinMode(ch2Pin_, INPUT);

  instance_ = this;
  attachInterrupt(digitalPinToInterrupt(ch1Pin_), handleCh1Change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ch2Pin_), handleCh2Change, CHANGE);
}

RcChannelSnapshot RcInputController::read() const {
  RcChannelSnapshot snapshot = {0, 0, 0, 0};
  unsigned long nowInMicroSec = micros();

  noInterrupts();
  snapshot.ch1PulseInMicroSec = ch1PulseInMicroSec_;
  snapshot.ch2PulseInMicroSec = ch2PulseInMicroSec_;
  snapshot.ch1PulseAgeInMicroSec = nowInMicroSec - ch1LastPulseInMicroSec_;
  snapshot.ch2PulseAgeInMicroSec = nowInMicroSec - ch2LastPulseInMicroSec_;
  interrupts();

  return snapshot;
}

void RcInputController::handleCh1Change() {
  if (instance_ != nullptr) {
    instance_->handleChannelChange(
        instance_->ch1InputRegister_, instance_->ch1BitMask_, instance_->ch1RiseInMicroSec_,
        instance_->ch1PulseInMicroSec_, instance_->ch1LastPulseInMicroSec_, instance_->ch1HistoryA_,
        instance_->ch1HistoryB_,
        instance_->ch1HistoryC_);
  }
}

void RcInputController::handleCh2Change() {
  if (instance_ != nullptr) {
    instance_->handleChannelChange(
        instance_->ch2InputRegister_, instance_->ch2BitMask_, instance_->ch2RiseInMicroSec_,
        instance_->ch2PulseInMicroSec_, instance_->ch2LastPulseInMicroSec_, instance_->ch2HistoryA_,
        instance_->ch2HistoryB_,
        instance_->ch2HistoryC_);
  }
}

void RcInputController::handleChannelChange(volatile uint8_t* inputRegister, uint8_t bitMask,
                                            volatile unsigned long& riseInMicroSec,
                                            volatile unsigned long& pulseInMicroSec,
                                            volatile unsigned long& lastPulseInMicroSec,
                                            volatile unsigned long& historyA,
                                            volatile unsigned long& historyB,
                                            volatile unsigned long& historyC) {
  unsigned long nowInMicroSec = micros();

  if ((*inputRegister & bitMask) != 0) {
    riseInMicroSec = nowInMicroSec;
  } else {
    unsigned long measuredPulseInMicroSec = nowInMicroSec - riseInMicroSec;

    if (measuredPulseInMicroSec >= kPulseMinInMicroSec &&
        measuredPulseInMicroSec <= kPulseMaxInMicroSec) {
      historyA = historyB;
      historyB = historyC;
      historyC = measuredPulseInMicroSec;
      pulseInMicroSec = median3(historyA, historyB, historyC);
      lastPulseInMicroSec = nowInMicroSec;
    }
  }
}

unsigned long RcInputController::median3(unsigned long a, unsigned long b, unsigned long c) const {
  if (a > b) {
    unsigned long temp = a;
    a = b;
    b = temp;
  }

  if (b > c) {
    unsigned long temp = b;
    b = c;
    c = temp;
  }

  if (a > b) {
    unsigned long temp = a;
    a = b;
    b = temp;
  }

  return b;
}
