// load the keybaord for the given profile pressed
void loadKeyBoard(const uint8_t& a_profileNum) {
  Serial.println("loaded keyboard");
  beep();
  if (a_profileNum > 0)
    device::profileInputNumber = a_profileNum;
  device::intputPosition = 0;
  display::showKeyboard = true;
  device::updateKeyboardInput = true;
  display::refreshPage = true;
  clearPage();
}

// This is the function requiered to display the keybaord,
// note the touch events is handeled in the "touch" tab
void keyBoard() {
  tft.setFont(&akashi_36px_Regular);
  tft.setFontScale(1);
  tft.setTextColor(RA8875_BLACK);
  tft.fillRect(120, 195, 650, 230, RA8875_WHITE);
  int keyboardColumn = 135;
  int keyboardRow = 260;
  if (device::charType != device::SPECIAL) {
    tft.print(keyboardColumn + 10, keyboardRow - 60, ".");
    keyboardColumn += 60;
    for (uint8_t i = 0; i < 10; i++) {
      tft.print(keyboardColumn, keyboardRow - 60, i);
      keyboardColumn += 60;
    }
  } 
  else {   
    tft.print(keyboardColumn + 10, keyboardRow - 60, device::specialSymbols[0]);
    keyboardColumn += 60;
    for (byte i = 1; i < 11; i++) {
      tft.print(keyboardColumn, keyboardRow - 60, device::specialSymbols[i]);
      if (i == 5)
        keyboardColumn += 5;
      keyboardColumn += 60;
    }
  }
  keyboardColumn = 135;
  keyboardRow = 260;
  char character = 'a';
  if (device::charType != device::LOWER)
    character = 'A';
  for (uint8_t i = 0; i < 26; i++) {
    tft.print(keyboardColumn, keyboardRow, character);
    character++;
    if (i == 10 || i == 21) {
      keyboardColumn = 135;
      keyboardRow += 60;
    }
    else {
      keyboardColumn += 60;
    }
  }
  tft.print(keyboardColumn, keyboardRow, "_"); // " " for space
  keyboardColumn += 60;
  tft.print(keyboardColumn, keyboardRow, "del");
  keyboardColumn += 90;
  tft.print(keyboardColumn, keyboardRow, "enter");
  keyboardColumn += 130;
  tft.setCursor(keyboardColumn, keyboardRow);
  if (device::charType == device::LOWER)
    tft.print("upper");
  else if (device::charType == device::UPPER)
    tft.print("spec");
  else 
    tft.print("lower");
}

// the function to diaply the input box
void keyBoardInput(const char* a_text, const int a_arrayLen) {
  tft.setFont(&akashi_36px_Regular);
  tft.setFontScale(1);
  tft.setTextColor(RA8875_BLACK);
  tft.fillRect(262, 110, 346, 42, RA8875_WHITE);
  tft.drawRect(260, 108, 350, 46, RA8875_BLACK);
  tft.drawRect(261, 109, 348, 44, RA8875_BLACK);
  tft.print(272, 110, a_text);
  display::cursourPosition = tft.getFontX() + 4;
  tft.setFont(&myriadPro_32px_Regular);
  tft.setTextColor(RA8875_BLACK);
  tft.print(330, 155, a_arrayLen - 1);
  tft.print(" characters max");
}

// the blinking cursor within the input box
void showCursor() {
  static bool blinkCursor = true;
  static unsigned long inputPreviousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - inputPreviousMillis >= 500UL) {
    blinkCursor = !blinkCursor;
    inputPreviousMillis = currentMillis;
  }
  if (blinkCursor) {
    tft.drawLine(display::cursourPosition - 3, 111, display::cursourPosition - 3, 148, RA8875_BLACK);
    tft.drawLine(display::cursourPosition - 2, 111, display::cursourPosition - 2, 148, RA8875_BLACK);
  }
  else {
    tft.drawLine(display::cursourPosition - 3, 111, display::cursourPosition - 3, 148, RA8875_WHITE);
    tft.drawLine(display::cursourPosition - 2, 111, display::cursourPosition - 2, 148, RA8875_WHITE);
  }
}
