#pragma once

#include <Arduino.h>

namespace AppConfig {
namespace Pins {
static const uint8_t kChannel1 = 2;
static const uint8_t kChannel2 = 3;
static const uint8_t kChannel3 = 18;
static const uint8_t kChannel4 = 19;
static const uint8_t kDriveSafetyToggle = 22;
static const uint8_t kEscSignal = 9;
static const uint8_t kSteeringSignal = 8;
static const uint8_t kSteeringPotentiometer = A0;
}  // namespace Pins

namespace Rc {
static const unsigned long kDefaultPulseInMicroSec = 1500;
static const unsigned long kPulseMinInMicroSec = 750;
static const unsigned long kPulseMaxInMicroSec = 2250;
static const unsigned long kPulseTimeoutInMicroSec = 25000;
static const unsigned long kSignalTimeoutInMicroSec = 100000;
static const unsigned long kSignalMinInMicroSec = 1100;
static const unsigned long kSignalMaxInMicroSec = 2000;
}  // namespace Rc

namespace Esc {
static const int kNeutralPulseInMicroSec = 1500;
static const int kSignalMinInMicroSec = 1000;
static const int kSignalMaxInMicroSec = 2000;
static const unsigned long kStartupDelayInMilliSec = 1000;
}  // namespace Esc

namespace Drive {
static const unsigned long kForwardSwitchThresholdInMicroSec = 1700;
static const unsigned long kReverseSwitchThresholdInMicroSec = 1200;
static const unsigned long kThrottleInputMinInMicroSec = 1240;
static const unsigned long kThrottleInputMaxInMicroSec = 2000;
static const unsigned long kForwardEscMaxInMicroSec = 2000;
static const unsigned long kReverseEscMinInMicroSec = 1000;
static const unsigned long kEscStepInMicroSec = 10;
static const unsigned long kEscHysteresisInMicroSec = 8;
static const unsigned long kEscRampStepInMicroSec = 8;
static const unsigned long kEscFastStopRampStepInMicroSec = 40;
}  // namespace Drive

namespace Steering {
static const unsigned long kInputCenterInMicroSec = 1580;
static const unsigned long kInputLeftMaxInMicroSec = 1900;
static const unsigned long kInputRightMaxInMicroSec = 1200;
static const unsigned long kInputDeadZoneInMicroSec = 0;
static const unsigned long kReceiverOverrideDeltaInMicroSec = 50;
static const unsigned long kReceiverLockThresholdInMicroSec = 1200;
static const unsigned long kOutputCenterInMicroSec = 1500;
static const unsigned long kOutputLeftMaxInMicroSec = 2000;
static const unsigned long kOutputRightMaxInMicroSec = 1000;
static const unsigned long kOutputRampMinStepInMicroSec = 18;
static const unsigned long kOutputRampMaxStepInMicroSec = 70;
static const unsigned long kOutputRampIntervalInMilliSec = 2;
static const unsigned long kStartupDelayInMilliSec = 300;
}  // namespace Steering

namespace Potentiometer {
static const int kAnalogMin = 0;
static const int kAnalogMax = 1023;
}  // namespace Potentiometer

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
static const char kChannel4Label[] = "CH4: ";
static const char kRcMonitorStarted[] = "RC Monitor started";
static const char kOledNotFound[] = "OLED not found";
static const char kEscStop[] = "STOP";
static const char kEscNoSignal[] = "NOSIG";
static const char kDriveNeutral[] = "N";
static const char kDriveForward[] = "FWD";
static const char kDriveReverse[] = "REV";
static const char kDriveNoSignal[] = "NOSIG";
static const char kDisplayDriveForward[] = "D";
static const char kDisplayDriveNeutral[] = "N";
static const char kDisplayDriveReverse[] = "R";
static const char kDisplayCarSteering[] = "<-->";
static const char kDisplayReceiverSteering[] = "<X>";
static const char kDisplayModeLabel[] = "Mode: ";
static const char kDisplayModeNoCheck[] = "NC";
static const char kDisplayModeRemoteCheck[] = "RCC";
static const char kShortNoSignal[] = "NS";
}  // namespace Text

namespace MainLoop {
static const unsigned long kMotorUpdateIntervalInMilliSec = 5;
static const unsigned long kDisplayUpdateIntervalInMilliSec = 50;
static const unsigned long kSerialUpdateIntervalInMilliSec = 100;
}  // namespace MainLoop
}
