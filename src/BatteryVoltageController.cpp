#include "BatteryVoltageController.h"

BatteryVoltageController::BatteryVoltageController(uint8_t pin) : pin_(pin) {}

void BatteryVoltageController::begin() {
  pinMode(pin_, INPUT);
}

BatteryVoltageSnapshot BatteryVoltageController::read() const {
  unsigned long adcSum = 0;

  for (uint8_t sampleIndex = 0; sampleIndex < AppConfig::Battery::kSampleCount; ++sampleIndex) {
    adcSum += analogRead(pin_);
  }

  int adcValue = adcSum / AppConfig::Battery::kSampleCount;

  if (adcValue < AppConfig::Battery::kMinDetectedAdcValue) {
    return {false, 0.0f};
  }

  float measuredVoltage =
      (static_cast<float>(adcValue) / 1023.0f) * AppConfig::Battery::kReferenceVoltage;
  float dividerRatio =
      (AppConfig::Battery::kUpperDividerResistorInOhm +
       AppConfig::Battery::kLowerDividerResistorInOhm) /
      AppConfig::Battery::kLowerDividerResistorInOhm;

  return {true, measuredVoltage * dividerRatio};
}
