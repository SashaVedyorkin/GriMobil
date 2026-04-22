#include "SerialController.h"

SerialController::SerialController(unsigned long baudRate) : baudRate_(baudRate) {}

void SerialController::begin() {
  Serial.begin(baudRate_);
}

void SerialController::logLine(const char* message) {
  Serial.println(message);
}

void SerialController::logLine(const __FlashStringHelper* message) {
  Serial.println(message);
}

void SerialController::logLine(const String& message) {
  Serial.println(message);
}

void SerialController::logValue(unsigned long value) {
  Serial.println(value);
}
