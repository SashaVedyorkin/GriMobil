#pragma once

#include <Arduino.h>

struct RcChannelSnapshot {
  unsigned long ch1PulseInMicroSec;
  unsigned long ch2PulseInMicroSec;
  unsigned long ch3PulseInMicroSec;
  unsigned long ch1PulseAgeInMicroSec;
  unsigned long ch2PulseAgeInMicroSec;
  unsigned long ch3PulseAgeInMicroSec;
};

class RcInputController {
public:
  RcInputController(uint8_t ch1Pin, uint8_t ch2Pinб, uint8_t ch3Pin);

  void begin();
  RcChannelSnapshot read() const;

private:
  static const unsigned long kPulseMinInMicroSec = 750;
  static const unsigned long kPulseMaxInMicroSec = 2250;

  static void handleCh1Change();
  static void handleCh2Change();
  static void handleCh3Change();

  void handleChannelChange(volatile uint8_t* inputRegister, uint8_t bitMask,
                           volatile unsigned long& riseInMicroSec,
                           volatile unsigned long& pulseInMicroSec,
                           volatile unsigned long& lastPulseInMicroSec,
                           volatile unsigned long& historyA,
                           volatile unsigned long& historyB,
                           volatile unsigned long& historyC);
  unsigned long median3(unsigned long a, unsigned long b, unsigned long c) const;

  static RcInputController* instance_;

  uint8_t ch1Pin_;
  uint8_t ch2Pin_;
  uint8_t ch3Pin_;
  uint8_t ch1BitMask_;
  uint8_t ch2BitMask_;
  uint8_t ch3BitMask_;
  volatile uint8_t* ch1InputRegister_;
  volatile uint8_t* ch2InputRegister_;
  volatile uint8_t* ch3InputRegister_;

  volatile unsigned long ch1RiseInMicroSec_;
  volatile unsigned long ch2RiseInMicroSec_;
  volatile unsigned long ch3RiseInMicroSec_;
  volatile unsigned long ch1PulseInMicroSec_;
  volatile unsigned long ch2PulseInMicroSec_;
  volatile unsigned long ch3PulseInMicroSec_;
  volatile unsigned long ch1LastPulseInMicroSec_;
  volatile unsigned long ch2LastPulseInMicroSec_;
  volatile unsigned long ch3LastPulseInMicroSec_;
  volatile unsigned long ch1HistoryA_;
  volatile unsigned long ch1HistoryB_;
  volatile unsigned long ch1HistoryC_;
  volatile unsigned long ch2HistoryA_;
  volatile unsigned long ch2HistoryB_;
  volatile unsigned long ch2HistoryC_;
  volatile unsigned long ch3HistoryA_;
  volatile unsigned long ch3HistoryB_;
  volatile unsigned long ch3HistoryC_;
};
