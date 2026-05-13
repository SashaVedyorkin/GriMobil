#include "SpeedSensorController.h"

SpeedSensorController* SpeedSensorController::instance_ = nullptr;

namespace {
float calculateSpeedInKiloMetersPerHour(unsigned long pulseIntervalInMicroSec) {
  if (pulseIntervalInMicroSec == 0) {
    return 0.0f;
  }

  float revolutionsPerHour =
      (3600000000.0f / pulseIntervalInMicroSec) /
      AppConfig::Speedometer::kPulsesPerWheelRotation;
  float wheelCircumferenceInKiloMeter =
      AppConfig::Speedometer::kWheelCircumferenceInCentiMeter / 100000.0f;

  return wheelCircumferenceInKiloMeter * revolutionsPerHour;
}
}  // namespace

SpeedSensorController::SpeedSensorController(uint8_t pin)
    : pin_(pin),
      isActive_(false),
      lastLevelHigh_(true),
      lastPulseAtInMicroSec_(0),
      lastIntervalInMicroSec_(0) {}

void SpeedSensorController::begin() {
  instance_ = this;
  pinMode(pin_, INPUT_PULLUP);
  lastLevelHigh_ = (PINK & _BV(PK1)) != 0;
  PCICR |= _BV(PCIE2);
  PCMSK2 |= _BV(PCINT17);
}

SpeedSensorSnapshot SpeedSensorController::read() const {
  unsigned long nowInMicroSec = micros();
  unsigned long lastPulseAtInMicroSec = 0;
  unsigned long lastIntervalInMicroSec = 0;
  bool isActive = false;

  noInterrupts();
  isActive = isActive_;
  lastPulseAtInMicroSec = lastPulseAtInMicroSec_;
  lastIntervalInMicroSec = lastIntervalInMicroSec_;
  interrupts();

  float speedInKiloMetersPerHour = 0.0f;

  if (lastIntervalInMicroSec != 0) {
    speedInKiloMetersPerHour = calculateSpeedInKiloMetersPerHour(lastIntervalInMicroSec);
  }

  if (lastPulseAtInMicroSec != 0 &&
      nowInMicroSec - lastPulseAtInMicroSec >
          AppConfig::Speedometer::kKeepLastSpeedTimeoutInMicroSec) {
    return {isActive, 0.0f};
  }

  return {isActive, speedInKiloMetersPerHour};
}

void SpeedSensorController::handleInterrupt() {
  if (instance_ == nullptr) {
    return;
  }

  bool isLevelHigh = (PINK & _BV(PK1)) != 0;
  unsigned long nowInMicroSec = micros();

  instance_->isActive_ = !isLevelHigh;

  if (instance_->lastLevelHigh_ && !isLevelHigh) {
    if (instance_->lastPulseAtInMicroSec_ != 0) {
      instance_->lastIntervalInMicroSec_ = nowInMicroSec - instance_->lastPulseAtInMicroSec_;
    }
    instance_->lastPulseAtInMicroSec_ = nowInMicroSec;
  }

  instance_->lastLevelHigh_ = isLevelHigh;
}

ISR(PCINT2_vect) {
  SpeedSensorController::handleInterrupt();
}
