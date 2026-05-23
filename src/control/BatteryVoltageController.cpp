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

uint8_t BatteryVoltageController::calculateChargePercent(
    const BatteryVoltageSnapshot& batteryVoltage) {
  if (!batteryVoltage.isDefined) {
    return 0;
  }

  float voltageInVolts = batteryVoltage.voltageInVolts;
  if (voltageInVolts <= AppConfig::Battery::kChargeMinVoltageInVolts) {
    return 0;
  }
  if (voltageInVolts >= AppConfig::Battery::kChargeMaxVoltageInVolts) {
    return 100;
  }

  float rangeInVolts = AppConfig::Battery::kChargeMaxVoltageInVolts -
                       AppConfig::Battery::kChargeMinVoltageInVolts;
  float normalizedValue =
      (voltageInVolts - AppConfig::Battery::kChargeMinVoltageInVolts) / rangeInVolts;
  return static_cast<uint8_t>(normalizedValue * 100.0f + 0.5f);
}
