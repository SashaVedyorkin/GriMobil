#include "DisplayController.h"

DisplayController::DisplayController()
    : display_(kScreenWidth, kScreenHeight, &Wire, kOledReset) {}

DisplayBeginResult DisplayController::begin() {
  if (!display_.begin(SSD1306_SWITCHCAPVCC, kI2cAddress)) {
    return {false, F("OLED not found")};
  }

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);
  display_.println("RC Monitor");
  display_.display();
  delay(1000);

  return {true, nullptr};
}

void DisplayController::renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3) {
  renderChannels(ch1, ch2, ch3, "");
}

int DisplayController::toPercent(unsigned long pulse) {
  if (pulse < 1000) pulse = 1000;
  if (pulse > 2000) pulse = 2000;
  return map(pulse, 1000, 2000, 0, 100);
}

void DisplayController::renderChannels(unsigned long ch1, unsigned long ch2, unsigned long ch3, const String& custom) {

  if (ch1 > 900 && ch1 < 2200) percent1 = toPercent(ch1);
  if (ch2 > 900 && ch2 < 2200) percent2 = toPercent(ch2);
  if (ch3 > 900 && ch3 < 2200) percent3 = toPercent(ch3);

  display_.clearDisplay();
  
  display_.setCursor(0, 0);
  display_.print("GriMobil 1.0");
  
  display_.setCursor(0, 16);
  display_.print("CH1: ");
  display_.print(ch1);
  display_.print(" ");
  display_.print(percent1);
  display_.print("%");
  if (custom.length() > 0) {
    display_.setCursor(0, 48);
    display_.print(" ");
    display_.println(custom);
  }
  else{
    display_.println(" ");
  }

  display_.setCursor(0, 32);
  display_.print("CH2: ");
  display_.print(ch2);
  display_.print(" ");
  display_.print(percent2);
  display_.print("%");
  display_.println(" ");
if(ch3!=0)
  display_.setCursor(0, 48);
  display_.print("CH3: ");
  display_.print(ch2);
  display_.print(" ");
  display_.print(percent3);
  display_.print("%");
  display_.println(" ");

  display_.display();
}
