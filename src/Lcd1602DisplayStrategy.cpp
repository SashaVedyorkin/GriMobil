#include "Lcd1602DisplayStrategy.h"

#include <stdio.h>

void DebugLcd1602DisplayStrategy::buildLines(const Lcd1602Snapshot& snapshot, char line1[17],
                                             char line2[17]) const {
  char speedValue[8];
  dtostrf(snapshot.speedInKiloMetersPerHour, 4, 2, speedValue);
  snprintf(line1, 17, "V:%s km/h      ", speedValue);

  if (snapshot.batteryVoltage.isDefined) {
    char voltageValue[8];
    dtostrf(snapshot.batteryVoltage.voltageInVolts, 4, 2, voltageValue);
    snprintf(line2, 17, "%s%s%s      ", AppConfig::Text::kVoltageLabel, voltageValue,
             AppConfig::Text::kVoltageUnit);
  } else {
    snprintf(line2, 17, "%s %s         ", AppConfig::Text::kVoltageLabel,
             AppConfig::Text::kVoltageNoValue);
  }
}

void UserLcd1602DisplayStrategy::buildLines(const Lcd1602Snapshot& snapshot, char line1[17],
                                            char line2[17]) const {
  snprintf(line1, 17, "%-4s THR:%02u%% %3s",
           snapshot.isEffectiveCarSteeringEnabled ? AppConfig::Text::kDisplayCarSteering
                                                  : AppConfig::Text::kDisplayReceiverSteering,
           snapshot.throttlePercent,
           snapshot.isRemoteRequiredForDrive ? AppConfig::Text::kDisplayModeRemoteCheck
                                             : AppConfig::Text::kDisplayModeNoCheck);
  snprintf(line2, 17, "%-16s", snapshot.stateText);
}
