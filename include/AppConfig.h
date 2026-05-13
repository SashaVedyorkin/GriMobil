#pragma once

#include <Arduino.h>

namespace AppConfig {
namespace Pins {
static const uint8_t kChannel1 = 2;
static const uint8_t kChannel2 = 3;
static const uint8_t kChannel3 = 18;
static const uint8_t kChannel4 = 19;
static const uint8_t kDriveSafetyToggle = A11;
static const uint8_t kTrackSensor = A9;
static const uint8_t kBatteryVoltage = A10;
static const uint8_t kHallThrottleSensor = A12;
static const uint8_t kDriveForwardButton = A13;
static const uint8_t kDriveReverseButton = A14;
static const uint8_t kSteeringPotentiometer = A8;
}  // namespace Pins

namespace Pca9685 {
static const uint8_t kI2cAddress = 0x40;
static const uint8_t kSteeringChannel = 0;
static const uint8_t kEscChannel = 1;
static const uint16_t kPwmFrequencyInHertz = 50;
static const uint16_t kPwmResolution = 4096;
}  // namespace Pca9685

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
static const unsigned long kThrottleInputLowPointInMicroSec = 1240;
static const unsigned long kThrottleInputHighPointInMicroSec = 1860;
static const unsigned long kForwardEscMaxInMicroSec = 2000;
static const unsigned long kReverseEscMinInMicroSec = 1000;
static const unsigned long kEscStepInMicroSec = 10;
static const unsigned long kEscHysteresisInMicroSec = 8;
static const unsigned long kEscRampStepInMicroSec = 8;
static const unsigned long kEscFastStopRampStepInMicroSec = 40;
}  // namespace Drive

namespace HallThrottle {
static const int kUpperPointRaw = 509;
static const int kLowerPointRaw = 350;
}  // namespace HallThrottle

namespace Steering {
static const unsigned long kInputCenterInMicroSec = 1580;
static const unsigned long kInputLeftMaxInMicroSec = 1900;
static const unsigned long kInputRightMaxInMicroSec = 1200;
static const unsigned long kInputDeadZoneInMicroSec = 0;
static const uint8_t kCorrectorSampleCount = 10;
static const uint8_t kCorrectorNeutralMajorityCount = 6;
static const unsigned long kCorrectorNeutralWindowInMicroSec = 50;
static const unsigned long kReceiverOverrideDeltaInMicroSec = 50;
static const unsigned long kReceiverLockThresholdInMicroSec = 1200;
static const unsigned long kOutputCenterInMicroSec = 1500;
static const unsigned long kOutputLeftMaxInMicroSec = 2000;
static const unsigned long kOutputRightMaxInMicroSec = 1000;
static const unsigned long kOutputRampMaxStepInMicroSec = 70;
static const unsigned long kOutputRampEaseDivisor = 3;
static const unsigned long kOutputRampIntervalInMilliSec = 2;
static const unsigned long kStartupDelayInMilliSec = 300;
}  // namespace Steering

namespace Potentiometer {
static const int kAnalogMin = 0;
static const int kAnalogMax = 1023;
}  // namespace Potentiometer

namespace Battery {
static const float kReferenceVoltage = 5.0f;
static const float kUpperDividerResistorInOhm = 30000.0f;
static const float kLowerDividerResistorInOhm = 10000.0f;
static const int kMinDetectedAdcValue = 5;
static const uint8_t kSampleCount = 16;
}  // namespace Bat

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
}  // namespace OLED

namespace Lcd1602 {
static const uint8_t kColumns = 16;
static const uint8_t kRows = 2;
static const uint8_t kI2cAddress = 0x27;
static const unsigned long kCh1NoSignalHoldInMilliSec = 5000;
}  // namespace 1602

namespace Speedometer {
static const float kWheelCircumferenceInCentiMeter = 74.0f;
static const uint8_t kPulsesPerWheelRotation = 6;
static const unsigned long kKeepLastSpeedTimeoutInMicroSec = 1500000;
}  // namespace Speedometer

namespace Serial {
static const unsigned long kBaudRate = 9600;
}  // namespace Serial Debug

namespace Text {
static const char kStartupMonitorTitle[] = "GriMobil started";
static const char kStartupReadyLine1[] = "GRIMOBIL";
static const char kStartupReadyLine2[] = "READY!!!";
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
static const char kSpeedLabel[] = "V: ";
static const char kSpeedUnit[] = " km/h";
static const char kHallLabel[] = "H: ";
static const char kVoltageLabel[] = "U:";
static const char kVoltageUnit[] = " V";
static const char kVoltageNoValue[] = "NO_BAT";
static const char kLcdStateLabel[] = "State:";
static const char kLcdStateCorrect[] = "Correct";
static const char kLcdStateCh1NoSignal[] = "nosig";
static const char kLcdStateNoCheckMode[] = "no-check mode";
static const char kShortNoSignal[] = "NS";
}  // Text

namespace MainLoop {
static const unsigned long kMotorUpdateIntervalInMilliSec = 5;
static const unsigned long kDisplayUpdateIntervalInMilliSec = 50;
static const unsigned long kSerialUpdateIntervalInMilliSec = 100;
}  // Main
}
