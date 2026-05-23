#include "OledAnnouncementController.h"

OledAnnouncementController::OledAnnouncementController()
    : persistentMessage_(""), temporaryMessage_(""), temporaryMessageUntilInMilliSec_(0) {}

void OledAnnouncementController::showTemporary(const char* message, unsigned long nowInMilliSec,
                                               unsigned long durationInMilliSec) {
  temporaryMessage_ = message;
  temporaryMessageUntilInMilliSec_ = nowInMilliSec + durationInMilliSec;
}

void OledAnnouncementController::setPersistent(const char* message) {
  persistentMessage_ = message;
}

void OledAnnouncementController::clearPersistent() {
  persistentMessage_ = "";
}

const char* OledAnnouncementController::currentMessage(unsigned long nowInMilliSec) const {
  if (temporaryMessage_[0] != '\0' &&
      static_cast<long>(temporaryMessageUntilInMilliSec_ - nowInMilliSec) > 0) {
    return temporaryMessage_;
  }

  return persistentMessage_;
}
