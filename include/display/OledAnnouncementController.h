#pragma once

#include <Arduino.h>

#include "AppConfig.h"

class OledAnnouncementController {
public:
  OledAnnouncementController();

  void showTemporary(const char* message, unsigned long nowInMilliSec,
                     unsigned long durationInMilliSec =
                         AppConfig::Announcement::kTemporaryDisplayDurationInMilliSec);
  void setPersistent(const char* message);
  void clearPersistent();
  const char* currentMessage(unsigned long nowInMilliSec) const;

private:
  const char* persistentMessage_;
  const char* temporaryMessage_;
  unsigned long temporaryMessageUntilInMilliSec_;
};
