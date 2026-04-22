#pragma once

#include <Arduino.h>

namespace AppConfig {
namespace Pins {
static const uint8_t kChannel1 = 2;
static const uint8_t kChannel2 = 3;
static const uint8_t kChannel3 = 4;
static const uint8_t kEscSignal = 9;
}  // namespace Pins

namespace Rc {
static const unsigned long kDefaultPulseInMicroSec = 1500;
static const unsigned long kPulseMinInMicroSec = 750;
static const unsigned long kPulseMaxInMicroSec = 2250;
static const unsigned long kPulseTimeoutInMicroSec = 25000;
static const unsigned long kSignalTimeoutInMicroSec = 100000;
static const unsigned long kSignalMinInMicroSec = 900;
static const unsigned long kSignalMaxInMicroSec = 2200;
}  // namespace Rc

namespace Esc {
static const int kNeutralPulseInMicroSec = 1500;
static const int kSignalMinInMicroSec = 1000;
static const int kSignalMaxInMicroSec = 2000;
static const int kStopEnterDeadbandInMicroSec = 10;
static const int kStopExitDeadbandInMicroSec = 35;
static const unsigned long kStartupDelayInMilliSec = 3000;
}  // namespace Esc

namespace Display {
static const int kScreenWidth = 128;
static const int kScreenHeight = 64;
static const int kOledReset = -1;
static const int kI2cAddress = 0x3C;
static const unsigned long kStartupDelayInMilliSec = 1000;
static const unsigned long kPercentMinPulseInMicroSec = 1000;
static const unsigned long kPercentMaxPulseInMicroSec = 2000;
static const unsigned long kVisibleSignalMinInMicroSec = 900;
static const unsigned long kVisibleSignalMaxInMicroSec = 2200;
}  // namespace Display

namespace Serial {
static const unsigned long kBaudRate = 9600;
}  // namespace Serial

namespace Text {
static const char kStartupMonitorTitle[] = "RC Monitor";
static const char kMainScreenTitle[] = "GriMobil 1.0";
static const char kChannel1Label[] = "CH1: ";
static const char kChannel2Label[] = "CH2: ";
static const char kChannel3Label[] = "CH3: ";
static const char kRcMonitorStarted[] = "RC Monitor started";
static const char kOledNotFound[] = "OLED not found";
static const char kEscStop[] = "STOP";
static const char kEscReverse[] = "REV";
static const char kEscNoSignal[] = "NOSIG";
}  // namespace Text

namespace MainLoop {
static const unsigned long kDelayInMilliSec = 100;
}  // namespace MainLoop
}
