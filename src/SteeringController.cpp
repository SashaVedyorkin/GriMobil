#include "SteeringController.h"

namespace {
static const unsigned long kSubMicroScale = 16;

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
      currentPulseFixedInSubMicroSec_(AppConfig::Steering::kOutputCenterInMicroSec *
                                      kSubMicroScale),
      latchedTargetPulseInMicroSec_(AppConfig::Steering::kOutputCenterInMicroSec),
      lastRampUpdateInMilliSec_(0),
      latchedDirection_(kCenterDirection) {}

void SteeringController::begin() {
  servo_.attach(pin_);
  currentPulseInMicroSec_ = AppConfig::Steering::kOutputCenterInMicroSec;
  currentPulseFixedInSubMicroSec_ = currentPulseInMicroSec_ * kSubMicroScale;
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

  unsigned long targetPulseFixedInSubMicroSec = targetPulseInMicroSec * kSubMicroScale;
  long deltaFixedInSubMicroSec =
      (long)targetPulseFixedInSubMicroSec - (long)currentPulseFixedInSubMicroSec_;
  unsigned long deltaInMicroSec = (unsigned long)(abs(deltaFixedInSubMicroSec) / kSubMicroScale);

  unsigned long rampStepInMicroSec = calculateRampStep(deltaInMicroSec);
  unsigned long rampStepFixedInSubMicroSec = rampStepInMicroSec * kSubMicroScale;

  if ((unsigned long)abs(deltaFixedInSubMicroSec) <= rampStepFixedInSubMicroSec) {
    currentPulseFixedInSubMicroSec_ = targetPulseFixedInSubMicroSec;
    return targetPulseInMicroSec;
  }

  long easedStepFixedInSubMicroSec =
      deltaFixedInSubMicroSec / (long)AppConfig::Steering::kOutputRampEaseDivisor;

  if (easedStepFixedInSubMicroSec == 0) {
    easedStepFixedInSubMicroSec = deltaFixedInSubMicroSec > 0 ? 1 : -1;
  }

  if (easedStepFixedInSubMicroSec > (long)rampStepFixedInSubMicroSec) {
    easedStepFixedInSubMicroSec = (long)rampStepFixedInSubMicroSec;
  }

  if (easedStepFixedInSubMicroSec < -(long)rampStepFixedInSubMicroSec) {
    easedStepFixedInSubMicroSec = -(long)rampStepFixedInSubMicroSec;
  }

  currentPulseFixedInSubMicroSec_ += (long)easedStepFixedInSubMicroSec;
  return (currentPulseFixedInSubMicroSec_ + (kSubMicroScale / 2)) / kSubMicroScale;
}

unsigned long SteeringController::calculateRampStep(unsigned long deltaInMicroSec) const {
  unsigned long maxStepInMicroSec = AppConfig::Steering::kOutputRampMaxStepInMicroSec;

  if (deltaInMicroSec == 0) {
    return 0;
  }

  unsigned long rampStepInMicroSec =
      deltaInMicroSec / AppConfig::Steering::kOutputRampEaseDivisor;

  if (rampStepInMicroSec == 0) {
    rampStepInMicroSec = 1;
  }

  if (rampStepInMicroSec > maxStepInMicroSec) {
    return maxStepInMicroSec;
  }

  return rampStepInMicroSec;
}
