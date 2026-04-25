#include "SteeringController.h"

namespace {
unsigned long constrainValue(unsigned long value, unsigned long minValue, unsigned long maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}
}  // namespace

SteeringController::SteeringController(uint8_t pin)
    : pin_(pin),
      currentPulseInMicroSec_(AppConfig::Steering::kOutputCenterInMicroSec),
      latchedTargetPulseInMicroSec_(AppConfig::Steering::kOutputCenterInMicroSec),
      lastRampUpdateInMilliSec_(0),
      latchedDirection_(kCenterDirection) {}

void SteeringController::begin() {
  servo_.attach(pin_);
  currentPulseInMicroSec_ = AppConfig::Steering::kOutputCenterInMicroSec;
  latchedTargetPulseInMicroSec_ = AppConfig::Steering::kOutputCenterInMicroSec;
  latchedDirection_ = kCenterDirection;
  lastRampUpdateInMilliSec_ = millis();
  servo_.writeMicroseconds(currentPulseInMicroSec_);
  delay(AppConfig::Steering::kStartupDelayInMilliSec);
}

unsigned long SteeringController::update(unsigned long ch2PulseInMicroSec) {
  unsigned long targetPulseInMicroSec = AppConfig::Steering::kOutputCenterInMicroSec;

  if (ch2PulseInMicroSec == 0) {
    latchedTargetPulseInMicroSec_ = AppConfig::Steering::kOutputCenterInMicroSec;
    latchedDirection_ = kCenterDirection;
    currentPulseInMicroSec_ = smoothServoPulse(AppConfig::Steering::kOutputCenterInMicroSec);
    servo_.writeMicroseconds((int)currentPulseInMicroSec_);
    return currentPulseInMicroSec_;
  }

  targetPulseInMicroSec = resolveTargetPulse(mapInputToServo(ch2PulseInMicroSec));
  currentPulseInMicroSec_ = smoothServoPulse(targetPulseInMicroSec);
  servo_.writeMicroseconds((int)currentPulseInMicroSec_);
  return currentPulseInMicroSec_;
}

unsigned long SteeringController::mapInputToServo(unsigned long ch2PulseInMicroSec) const {
  unsigned long deltaFromCenterInMicroSec =
      ch2PulseInMicroSec > AppConfig::Steering::kInputCenterInMicroSec
          ? ch2PulseInMicroSec - AppConfig::Steering::kInputCenterInMicroSec
          : AppConfig::Steering::kInputCenterInMicroSec - ch2PulseInMicroSec;

  if (deltaFromCenterInMicroSec <= AppConfig::Steering::kInputDeadZoneInMicroSec) {
    return AppConfig::Steering::kOutputCenterInMicroSec;
  }

  if (ch2PulseInMicroSec >= AppConfig::Steering::kInputCenterInMicroSec) {
    unsigned long constrainedInput = constrainValue(
        ch2PulseInMicroSec, AppConfig::Steering::kInputCenterInMicroSec,
        AppConfig::Steering::kInputLeftMaxInMicroSec);

    return map(constrainedInput, AppConfig::Steering::kInputCenterInMicroSec,
               AppConfig::Steering::kInputLeftMaxInMicroSec,
               AppConfig::Steering::kOutputCenterInMicroSec,
               AppConfig::Steering::kOutputLeftMaxInMicroSec);
  }

  unsigned long constrainedInput =
      constrainValue(ch2PulseInMicroSec, AppConfig::Steering::kInputRightMaxInMicroSec,
                     AppConfig::Steering::kInputCenterInMicroSec);

  return map(constrainedInput, AppConfig::Steering::kInputRightMaxInMicroSec,
             AppConfig::Steering::kInputCenterInMicroSec,
             AppConfig::Steering::kOutputRightMaxInMicroSec,
             AppConfig::Steering::kOutputCenterInMicroSec);
}

unsigned long SteeringController::resolveTargetPulse(unsigned long requestedPulseInMicroSec) {
  unsigned long centerPulseInMicroSec = AppConfig::Steering::kOutputCenterInMicroSec;
  SteeringDirection requestedDirection = kCenterDirection;

  if (requestedPulseInMicroSec > centerPulseInMicroSec) {
    requestedDirection = kPositiveDirection;
  } else if (requestedPulseInMicroSec < centerPulseInMicroSec) {
    requestedDirection = kNegativeDirection;
  }

  if (requestedDirection == kCenterDirection) {
    latchedTargetPulseInMicroSec_ = centerPulseInMicroSec;
    latchedDirection_ = kCenterDirection;
    return latchedTargetPulseInMicroSec_;
  }

  if (latchedDirection_ != requestedDirection) {
    latchedDirection_ = requestedDirection;
    latchedTargetPulseInMicroSec_ = requestedPulseInMicroSec;
    return latchedTargetPulseInMicroSec_;
  }

  if (requestedDirection == kPositiveDirection &&
      requestedPulseInMicroSec > latchedTargetPulseInMicroSec_) {
    latchedTargetPulseInMicroSec_ = requestedPulseInMicroSec;
  }

  if (requestedDirection == kNegativeDirection &&
      requestedPulseInMicroSec < latchedTargetPulseInMicroSec_) {
    latchedTargetPulseInMicroSec_ = requestedPulseInMicroSec;
  }

  if (currentPulseInMicroSec_ == latchedTargetPulseInMicroSec_) {
    latchedTargetPulseInMicroSec_ = requestedPulseInMicroSec;
  }

  return latchedTargetPulseInMicroSec_;
}

unsigned long SteeringController::smoothServoPulse(unsigned long targetPulseInMicroSec) {
  unsigned long nowInMilliSec = millis();

  if (nowInMilliSec - lastRampUpdateInMilliSec_ <
      AppConfig::Steering::kOutputRampIntervalInMilliSec) {
    return currentPulseInMicroSec_;
  }

  lastRampUpdateInMilliSec_ = nowInMilliSec;

  unsigned long deltaInMicroSec =
      targetPulseInMicroSec > currentPulseInMicroSec_
          ? targetPulseInMicroSec - currentPulseInMicroSec_
          : currentPulseInMicroSec_ - targetPulseInMicroSec;

  unsigned long rampStepInMicroSec = calculateRampStep(deltaInMicroSec);

  if (deltaInMicroSec <= rampStepInMicroSec) {
    return targetPulseInMicroSec;
  }

  if (targetPulseInMicroSec > currentPulseInMicroSec_) {
    return currentPulseInMicroSec_ + rampStepInMicroSec;
  }

  return currentPulseInMicroSec_ - rampStepInMicroSec;
}

unsigned long SteeringController::calculateRampStep(unsigned long deltaInMicroSec) const {
  unsigned long minStepInMicroSec = AppConfig::Steering::kOutputRampMinStepInMicroSec;
  unsigned long maxStepInMicroSec = AppConfig::Steering::kOutputRampMaxStepInMicroSec;

  if (deltaInMicroSec <= minStepInMicroSec) {
    return minStepInMicroSec;
  }

  if (deltaInMicroSec >= 200) {
    return maxStepInMicroSec;
  }

  return map(deltaInMicroSec, minStepInMicroSec, 200, minStepInMicroSec, maxStepInMicroSec);
}
