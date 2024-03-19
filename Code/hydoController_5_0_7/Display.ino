// Draw the given page
void drawPages() {
  if (!display::displayIsOff) {
    if (display::showInfoDialog)
      infoMessage(message::infoMessageArray, message::infoPos);
    else if (display::page == 0)
      homePage();
    else if (display::page == 1)
      graphsPage();
    else if (display::page == 2)
      maxminsPage();
    else if (display::page == 3)
      profilesPage();
    else if (display::page == 4)
      settingsPage();
    else if (display::page == 5)
      user::convertToTds ? tdsPage() : ecPage();
    else if (display::page == 6)
      phPage();
    else if (display::page == 7)
      co2Page();
    else if (display::page == 8)
      waterPage();
    else if (display::page == 9)
      dosersPage();
    else if (display::page == 10)
      timerPage();
    else if (display::page == 11)
      fansPage();
    else if (display::page == 12)
      warningsPage();
    display::refreshPage = false;
    device::newGraphData = false;
  }
  screenSaver();
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
// Even though the pages are only called from one place
// it is much easier to seperate each page into its
// own function rather than combing thousands of lines.
void homePage() {
  if (millis() - device::slideShowpreviousMillis >= 10000UL) {
    display::homePage < 9 ? display::homePage++ : display::homePage = 0;
    device::slideShowpreviousMillis = millis();
    clearPage();
    display::refreshPage = true;
  }
  //
  if (display::refreshPage) {
    // Draw slideShow icons
    drawSlideIcons(110, 112, display::homePage, 10);
    // print the title
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    if (display::homePage == 0) {
      tft.print(140, 140, F("TDS sensor"));
    }
    else if (display::homePage == 1) {
      tft.print(160, 140, F("EC sensor"));
    }
    else if (display::homePage == 2) {
      tft.print(160, 140, F("PH sensor"));
    }
    else if (display::homePage == 3) {
      tft.print(140, 140, F("Co2 sensor"));
    }
    else if (display::homePage == 4) {
      tft.print(130, 140, F("Water temp"));
      tft.setCursor(tft.getFontX() + 6, 130);
      !user::convertToF ? tft.print(F("c")) : tft.print(F("f"));
    }
    else if (display::homePage == 5) {
      !user::convertToInches ? tft.setCursor(100, 140) : tft.setCursor(120, 140);
      tft.print(F("Water height"));
      tft.setCursor(tft.getFontX() + 6, 140);
      !user::convertToInches ? tft.print(F("cm")) : tft.print(F("\""));
    }
    else if (display::homePage == 6) {
      tft.print(110, 140, F("Fan one speed"));
    }
    else if (display::homePage == 7) {
      tft.print(110, 140, F("Fan two speed"));
    }
    else if (display::homePage == 8) {
      tft.print(170, 140, F("Air temp"));
      tft.setCursor(tft.getFontX() + 6, 130);
      !user::convertToF ? tft.print(F("c")) : tft.print(F("f"));
    }
    else if (display::homePage == 9) {
      tft.print(180, 140, F("Humidity"));
    }
  }
  // RTC clock
  static uint8_t previousRTCMinute;
  rtc.refresh();
  if (display::refreshPage || rtc.minute() != previousRTCMinute) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    static int16_t statusBarStartX = 0, statusBarWidth = 0;
    tft.fillRect(statusBarStartX, 92, statusBarWidth, 34, user::backgroundColor);
    char timeStr[16]{0};
    strcpy(timeStr, rtc.getTimeStr());
    strcat(timeStr, (rtc.hour() <= 12 ? " AM" : " PM"));
    int16_t timeStrOffset = tft.getStringWidth(timeStr);
    tft.print(795-timeStrOffset, 92, timeStr);
    // Wifi symbol
    tft.drawXBMP(795 - (timeStrOffset + 45), 99, miniWifiIcon, 97, 30, 26, wifi::wifiEnabled ? RA8875_BLUE : display::RA8875_DARKGREY, user::backgroundColor, 1);
    // Light symbol
    tft.drawXBMP(795 - (timeStrOffset + 72), 99, miniLightIcon, 64, 19, 27, RA8875_BLACK, user::backgroundColor, 1);
    if (device::lightOn) {
      uint16_t iconX = 803 - (timeStrOffset + 72);
      uint16_t iconY = 107;
      tft.fillCircle(iconX + 1, iconY + 1, 7, RA8875_YELLOW);
      iconX = 802 - (timeStrOffset + 74);
      iconY = 112;
      tft.fillRect(iconX, iconY, 1, 3, RA8875_BLACK);
      tft.fillRect(iconX + 1, iconY - 1, 2, 1, RA8875_BLACK);
      tft.fillRect(iconX + 3, iconY, 3, 1, RA8875_BLACK);
      tft.fillRect(iconX + 6, iconY - 1, 2, 1, RA8875_BLACK);
      tft.fillRect(iconX + 8, iconY, 1, 3, RA8875_BLACK);
    }
    // Warning symbol
    tft.drawXBMP(795 - (timeStrOffset + 112), 95, miniWarningIcon, 116, 31, 30, device::globalErrorState == device::MAJOR_WARNING ? RA8875_RED : RA8875_YELLOW, user::backgroundColor, 1);
    statusBarStartX = 795 - (timeStrOffset + 99);
    statusBarWidth = tft.width() - (timeStrOffset + 99);
    previousRTCMinute = rtc.minute();
  }
  // draw values and graph
  //if (display::refreshPage || device::newGraphData) {
    if (display::homePage == 0) {
      drawSensorSlide(sensor::tds, user::targetMinTds, user::targetMaxTds, sensor::tdsArray, 0);
    }
    else if (display::homePage == 1) {
      drawSensorSlide(sensor::ec, user::targetMinEc, user::targetMaxEc, sensor::ecArray, 2);
    }
    else if (display::homePage == 2) {
      drawSensorSlide(sensor::ph, user::targetMinPh, user::targetMaxPh, sensor::phArray, 2);
    }
    else if (display::homePage == 3) {
      drawSensorSlide(sensor::co2, user::targetCo2, user::targetCo2, sensor::co2Array, 0);
    }
    else if (display::homePage == 4) {
      if (user::convertToF)
        drawSensorSlide(convertToF(sensor::waterTemp), user::targetMinWaterTempF, user::targetMaxWaterTempF, sensor::waterTemperatureArrayF, 1);
      else
        drawSensorSlide(sensor::waterTemp, user::targetMinWaterTemp, user::targetMaxWaterTemp, sensor::waterTemperatureArray, 1);
    }
    else if (display::homePage == 5) {
      if (user::convertToInches)
        drawSensorSlide(convertToInches(sensor::waterLevel), user::targetMinWaterHeightInches, user::targetMaxWaterHeightInches, sensor::waterLevelArrayInInches, 1);
      else
        drawSensorSlide(sensor::waterLevel, user::targetMinWaterHeight, user::targetMaxWaterHeight, sensor::waterLevelArray, 1);
    }
    else if (display::homePage == 6) {
      drawSensorSlide(device::fanOneSpeed, user::targetMinFanOneSpeed, user::targetMaxFanOneSpeed, sensor::fanOneSpeedArray, 0);
    }
    else if (display::homePage == 7) {
      drawSensorSlide(device::fanTwoSpeed, user::targetMinFanTwoSpeed, user::targetMaxFanTwoSpeed, sensor::fanTwoSpeedArray, 0);
    }
    else if (display::homePage == 8) {
      if (device::lightOn) {
        if (user::convertToF)
          drawSensorSlide(convertToF(sensor::airTemp), user::targetDayMinAirTempF, user::targetDayMaxAirTempF, sensor::airTemperatureArrayF, 1);
        else
          drawSensorSlide(sensor::airTemp, user::targetDayMinAirTemp, user::targetDayMaxAirTemp, sensor::airTemperatureArray, 1);
      }
      else {
         if (user::convertToF)
          drawSensorSlide(convertToF(sensor::airTemp), user::targetNightMinAirTempF, user::targetNightMaxAirTempF, sensor::airTemperatureArrayF, 1);
        else
          drawSensorSlide(sensor::airTemp, user::targetNightMinAirTemp, user::targetNightMaxAirTemp, sensor::airTemperatureArray, 1);       
      }
    }
    else if (display::homePage == 9) {
      if (device::lightOn)
        drawSensorSlide(sensor::humidity, user::targetDayMinHumidity, user::targetDayMaxHumidity, sensor::humidityArray, 1);
      else
        drawSensorSlide(sensor::humidity, user::targetNightMinHumidity, user::targetNightMaxHumidity, sensor::humidityArray, 1);
    }
  //}
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void graphsPage() {
  if (display::refreshPage || device::newGraphData) {
    if (display::refreshPage) {
      drawGraphIcons();
    }
    
    int16_t numOfGraphs = 0;
    float* graphArrays[10] {};
    uint16_t graphColors[10] {};

    if (display::showTdsGraph) {
      graphArrays[numOfGraphs] = sensor::tdsArray;
      graphColors[numOfGraphs++] = display::RA8875_PURPLE;
    }
    if (display::showPhGraph) {
      graphArrays[numOfGraphs] = sensor::phArray;
      graphColors[numOfGraphs++] = display::RA8875_LIGHT_GREEN;
    }
    if (display::showEcGraph) {
      graphArrays[numOfGraphs] = sensor::ecArray;
      graphColors[numOfGraphs++] = RA8875_YELLOW;
    }
    if (display::showCo2Graph) {
      graphArrays[numOfGraphs] = sensor::co2Array;
      graphColors[numOfGraphs++] = RA8875_MAGENTA;
    }
    if (display::showWaterTempGraph) {
      graphArrays[numOfGraphs] = (user::convertToF ? sensor::waterTemperatureArrayF : sensor::waterTemperatureArray);
      graphColors[numOfGraphs++] = display::RA8875_ORANGE;
    }
    if (display::showWaterLvlGraph) {
      graphArrays[numOfGraphs] = (user::convertToInches ? sensor::waterLevelArrayInInches : sensor::waterLevelArray);
      graphColors[numOfGraphs++] = RA8875_BLUE;
    }
    if (display::showFanOneGraph) {
      graphArrays[numOfGraphs] = sensor::fanOneSpeedArray;
      graphColors[numOfGraphs++] = RA8875_WHITE;
    }
    if (display::showFanTwoGraph) {
      graphArrays[numOfGraphs] = sensor::fanTwoSpeedArray;
      graphColors[numOfGraphs++] = RA8875_BLACK;
    }
    if (display::showAirTempGraph) {
      graphArrays[numOfGraphs] = (user::convertToF ? sensor::airTemperatureArrayF : sensor::airTemperatureArray);
      graphColors[numOfGraphs++] = RA8875_RED;
    }
    if (display::showHumidityGraph) {
      graphArrays[numOfGraphs] = sensor::humidityArray;
      graphColors[numOfGraphs++] = display::RA8875_SEABLUE;
    }
    if (numOfGraphs > 0 && display::refreshPage || device::newGraphData)
      drawGraph(95, 100, 364, tft.width() - 155, 10, graphArrays, numOfGraphs, sensor::sensorArrayPos, 0, graphColors);
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void maxminsPage() {
  uint16_t color = 0;
  uint16_t color2 = 0;
  if (display::refreshPage) {
    drawSlideIcons(110, 112, display::maxMinsPage, 10);
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(176, 170, F("Minimum"));
    tft.print(530, 170, F("Maximum"));
    infoButton(770, 120);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
  }
  if (display::maxMinsPage == 0) {
    if (display::refreshPage)
      tft.print(330, 126, F("PPM history"));
    drawTwoValues(285, device::minTds, RA8875_BLACK, 0, 650, device::maxTds, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::maxMinsPage == 1) {
    if (display::refreshPage)
      tft.print(340, 126, F("EC history"));
    drawTwoValues(285, device::minEc, RA8875_BLACK, 2, 650, device::maxEc, RA8875_BLACK, 2, NULL, 00);
  }
  else if (display::maxMinsPage == 2) {
    if (display::refreshPage)
      tft.print(340, 126, F("PH history"));
    drawTwoValues(285, device::minPh, RA8875_BLACK, 2, 650, device::maxPh, RA8875_BLACK, 2, NULL, 00);
  }
  else if (display::maxMinsPage == 3) {
    if (display::refreshPage)
      tft.print(330, 126, F("Co2 history"));
    drawTwoValues(285, device::minCo2, RA8875_BLACK, 0, 650, device::maxCo2, RA8875_BLACK, 0, NULL, 00);
  }
  else if (display::maxMinsPage == 4) {
    if (display::refreshPage)
      tft.print(250, 126, F("Water temp history"));
    if (user::convertToF)
      drawTwoValues(285, convertToF(device::minWaterTemp), RA8875_BLACK, 1, 650, convertToF(device::maxWaterTemp), RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, device::minWaterTemp, RA8875_BLACK, 1, 650, device::maxWaterTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::maxMinsPage == 5) {
    if (display::refreshPage)
      tft.print(234, 126, F("Water height history"));
    if (user::convertToInches) {
      drawTwoValues(285, convertToInches(device::minWaterLevel), RA8875_BLACK, 0, 650, convertToInches(device::maxWaterLevel), RA8875_BLACK, 0, "\"", 0);
    }
    else {
      drawTwoValues(285, device::minWaterLevel, RA8875_BLACK, 0, 650, device::maxWaterLevel, RA8875_BLACK, 0, "CM", 50);
    }
  }
  else if (display::maxMinsPage == 6) {
    if (display::refreshPage)
      tft.print(214, 126, F("Fan one speed history"));
    drawTwoValues(285, device::minFanOneSpeed, RA8875_BLACK, 0, 650, device::maxFanOneSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::maxMinsPage == 7) {
    if (display::refreshPage)
      tft.print(214, 126, F("Fan two speed history"));
    drawTwoValues(285, device::minFanTwoSpeed, RA8875_BLACK, 0, 650, device::maxFanTwoSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::maxMinsPage == 8) {
    if (display::refreshPage)
      tft.print(286, 126, F("Air temp history"));
    if (user::convertToF)
      drawTwoValues(285, convertToF(device::minAirTemp), RA8875_BLACK, 1, 650, convertToF(device::maxAirTemp), RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, device::minAirTemp, RA8875_BLACK, 1, 650, device::maxAirTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::maxMinsPage == 9) {
    if (display::refreshPage)
      tft.print(282, 126, F("Humidity history"));
    drawTwoValues(285, device::minHumidity, color, 1, 650, device::maxHumidity, color2, 1, "%", 50);
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void profilesPage() {
  if (display::refreshPage && display::showSaveDialog && !display::showKeyboard) {
    continueMessage(message::saveProfile, device::userProfile + 1, 0, true, true, false);
  }
  else if (display::refreshPage && !display::showKeyboard) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(200, 100, F("Save and load profiles"));
    // Input box and buttons for profile 1
    inputBox(130, 160, user::profileOneName, device::userProfile == 0 ? RA8875_WHITE : display::RA8875_LIGHTGREY);
    saveButton(493, 160);
    loadButton(631, 160);
    // Input box and buttons for profile 2
    inputBox(130, 222, user::profileTwoName, device::userProfile == 1 ? RA8875_WHITE : display::RA8875_LIGHTGREY);
    saveButton(493, 222);
    loadButton(631, 222);
    // Input box and buttons for profile 3
    inputBox(130, 284, user::profileThreeName, device::userProfile == 2 ? RA8875_WHITE : display::RA8875_LIGHTGREY);
    saveButton(493, 284);
    loadButton(631, 284);
    // Input box and buttons for profile 4
    inputBox(130, 346, user::profileFourName, device::userProfile == 3 ? RA8875_WHITE : display::RA8875_LIGHTGREY);
    saveButton(493, 346);
    loadButton(631, 346);
    // Input box and buttons for profile 5
    inputBox(130, 408, user::profileFiveName, device::userProfile == 4 ? RA8875_WHITE : display::RA8875_LIGHTGREY);
    saveButton(493, 408);
    loadButton(631, 408);
    infoButton(770, 120);
  }
  else if (display::refreshPage && display::showKeyboard) {
    // draw the keybaord
    keyBoard();
  }
  if ((display::refreshPage && display::showKeyboard) || (display::showKeyboard && device::updateKeyboardInput)) {
    // draw input box
    if (device::profileInputNumber == 1)
      keyBoardInput(user::profileOneName, 16);
    else if (device::profileInputNumber == 2)
      keyBoardInput(user::profileTwoName, 16);
    else if (device::profileInputNumber == 3)
      keyBoardInput(user::profileThreeName, 16);
    else if (device::profileInputNumber == 4)
      keyBoardInput(user::profileFourName, 16);
    else if (device::profileInputNumber == 5)
      keyBoardInput(user::profileFiveName, 16);
    device::updateKeyboardInput = false;
  }
  if (display::showKeyboard)
    showCursor();
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void settingsPage() {
  tft.setFont(&akashi_36px_Regular);
  tft.setFontScale(1);
  // Draw page navigation buttons
  if (display::refreshPage && !display::showingDialog)
    drawSlideIcons(110, 112, display::settingsPage, 5);
  // settings slider pages
  if (display::settingsPage == 0)
    drawSettingsPageZero();
  else if (display::settingsPage == 1)
    drawSettingsPageOne();
  else if (display::settingsPage == 2)
    drawSettingsPageTwo();
  else if (display::settingsPage == 3)
    drawSettingsPageThree();
  else if (display::settingsPage == 4)
    drawSettingsPageFour();
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void tdsPage() {
  if (display::refreshPage) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(300, 116, F("Target PPM"));
    tft.print(176, 170, F("Minimum"));
    tft.print(530, 170, F("Maximum"));
    drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
    drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
    infoButton(770, 120);
  }
  drawTwoValues(285, user::targetMinTds, RA8875_BLACK, 0, 650, user::targetMaxTds, RA8875_BLACK, 0, NULL, 0);
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void ecPage() {
  if (display::refreshPage) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(326, 116, F("Target EC"));
    tft.print(176, 170, F("Minimum"));
    tft.print(530, 170, F("Maximum"));
    drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
    drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
    infoButton(770, 120);
  }
  drawTwoValues(285, user::targetMinEc, RA8875_BLACK, 2, 650, user::targetMaxEc, RA8875_BLACK, 2, NULL, 0);
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void phPage() {
  if (display::refreshPage) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(326, 116, F("Target PH"));
    tft.print(176, 170, F("Minimum"));
    tft.print(530, 170, F("Maximum"));
    drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
    drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
    infoButton(770, 120);
  }
  drawTwoValues(285, user::targetMinPh, RA8875_BLACK, 2, 650, user::targetMaxPh, RA8875_BLACK, 2, NULL, 0);
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void co2Page() {
  drawSlideIcons(110, 112, display::co2PageScrollPos, 4);
  if (display::co2PageScrollPos == 0) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(330, 116, F("Target Co2"));
      tft.print(156, 166, F("Target PPM"));
      tft.print(530, 166, F("Tolerance"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetCo2, RA8875_BLACK, 0, 650, user::co2Offset, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::co2PageScrollPos == 1) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(330, 116, F("Dimensions"));
      tft.print(160, 166, F("Length"));
      tft.print(380, 166, F("Width"));
      tft.print(590, 166, F("Height"));
      drawThreeUpDownButtons();
      infoButton(770, 120);
    }
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    // ROOM LENGTH
    static int16_t roomLengthStartPos = 0, roomLengthEndPos = 0;
    static float previousRoomLength;
    float roomLength = user::convertToInches ? user::roomLengthInches : user::roomLengthCm;
    if (display::refreshPage || hasChanged(roomLength, previousRoomLength, 2)) {
      tft.fillRect(roomLengthStartPos, 236, roomLengthEndPos - roomLengthStartPos, 50, user::backgroundColor);
      if (user::convertToInches) {
        roomLengthStartPos = 225 - ((tft.getStringWidth(roomLength, 1) / 2) + 8);
        tft.print(roomLengthStartPos, 240, roomLength, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 230, F("\""));
      }
      else {
        roomLengthStartPos = 225 - ((tft.getStringWidth(roomLength, 0) / 2) + 23);
        tft.print(roomLengthStartPos, 240, roomLength, 0);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 250, F("cm"));
      }
      roomLengthEndPos = tft.getFontX();
      previousRoomLength = roomLength;
    }
    // ROOM WIDTH
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int16_t roomWidthStartPos = 0, roomWidthEndPos = 0;
    static float previousRoomWidth;
    float roomWidth = user::convertToInches ? user::roomWidthInches : user::roomWidthCm;
    if (display::refreshPage || hasChanged(roomWidth, previousRoomWidth, 2)) {
      tft.fillRect(roomWidthStartPos, 236, roomWidthEndPos - roomWidthStartPos, 50, user::backgroundColor);
      if (user::convertToInches) {
        roomWidthStartPos = 430 - ((tft.getStringWidth(roomWidth, 1) / 2) + 8);
        tft.print(roomWidthStartPos, 240, roomWidth, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 230, F("\""));
      }
      else {
        roomWidthStartPos = 430 - ((tft.getStringWidth(roomWidth, 0) / 2) + 23);
        tft.print(roomWidthStartPos, 240, roomWidth, 0);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 250, F("cm"));
      }
      roomWidthEndPos = tft.getFontX();
      previousRoomWidth = roomWidth;
    }
    // ROOM HEIGHT
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int16_t roomHeightStartPos = 0, roomHeightEndPos = 0;
    static float previousRoomHeight;
    float roomHeight = user::convertToInches ? user::roomHeightInches : user::roomHeightCm;
    if (display::refreshPage || hasChanged(roomHeight, previousRoomHeight, 2)) {
      tft.fillRect(roomHeightStartPos, 236, roomHeightEndPos - roomHeightStartPos, 50, user::backgroundColor);
      if (user::convertToInches) {
        roomHeightStartPos = 640 - ((tft.getStringWidth(roomHeight, 1) / 2) + 8);
        tft.print(roomHeightStartPos, 240, roomHeight, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 230, F("\""));
      }
      else {
        roomHeightStartPos = 640 - ((tft.getStringWidth(roomHeight, 0) / 2) + 23);
        tft.print(roomHeightStartPos, 240, roomHeight, 0);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 250, F("cm"));
      }
      roomHeightEndPos = tft.getFontX();
      previousRoomHeight = roomHeight;
    }
  }
  else if (display::co2PageScrollPos == 2) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(330, 116, F("Co2 settings"));
      tft.print(130, 166, F("Flow rate"));
      tft.print(380, 166, F("Time"));
      tft.print(580, 166, F("Fans off"));
      drawThreeUpDownButtons();
      infoButton(770, 120);
    }
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    // CO2 FLOW RATE LTR/MIN
    static int16_t co2FlowrateStartPos = 0, co2FlowrateEndPos = 0;
    static float previousCo2Flowrate;
    float co2Flowrate = user::convertToInches ? user::co2FlowrateFeet3 : user::co2FlowrateLtrs;
    if (display::refreshPage || hasChanged(co2Flowrate, previousCo2Flowrate, 2)) {
      tft.fillRect(co2FlowrateStartPos, 238, co2FlowrateEndPos - co2FlowrateStartPos, 50, user::backgroundColor);
      if (co2Flowrate < 10)
        co2FlowrateStartPos = 140;
      else
        co2FlowrateStartPos = 120;
      tft.setCursor(co2FlowrateStartPos, 240);
      if (user::convertToInches) {
        tft.print(co2Flowrate, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.setCursor(tft.getFontX() + 4, 236);
        tft.print(F("Ft3"));
      }
      else {
        tft.print(co2Flowrate, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.setCursor(tft.getFontX() + 4, 250);
        co2Flowrate == 1 ? tft.print(F("ltr")) : tft.print(F("ltrs"));
      }
      co2FlowrateEndPos = tft.getFontX();
      previousCo2Flowrate = co2Flowrate;
    }
    // CO2 CHECK TIME
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int16_t co2CheckTimePosition;
    static int16_t previousCo2CheckTimeMinute;
    if (display::refreshPage || user::co2CheckTimeMinute != previousCo2CheckTimeMinute) {
      tft.fillRect(338, 238, co2CheckTimePosition - 336, 50, user::backgroundColor);
      tft.setCursor(350, 240);
      if (user::co2CheckTimeHour < 10)
        tft.print(F("0"));
      tft.print(user::co2CheckTimeHour);
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.print(F(":"));
      tft.setFont(&HallfeticaLargenum_42px_Regular);
      tft.setFontScale(1);
      if (user::co2CheckTimeMinute < 10)
        tft.print(F("0"));
      tft.print(user::co2CheckTimeMinute);
      co2CheckTimePosition = tft.getFontX();
      previousCo2CheckTimeMinute = user::co2CheckTimeMinute;
    }
    // DISABLE FANS TIMER
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int16_t disableFansTimerStartPos = 0, disableFansTimerEndPos = 0;
    static int16_t previousDisableFansTimer;

    if (display::refreshPage || user::disableFansTimer != previousDisableFansTimer) {
      tft.fillRect(disableFansTimerStartPos, 238, disableFansTimerEndPos - disableFansTimerStartPos, 50, user::backgroundColor);
      if (user::disableFansTimer < 10)
        disableFansTimerStartPos = 600;
      else if (user::disableFansTimer >= 10 && user::disableFansTimer < 100)
        disableFansTimerStartPos = 580;
      else
        disableFansTimerStartPos = 570;
      tft.setCursor(disableFansTimerStartPos, 240);
      if (user::disableFansTimer == 0) {
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(F("Disabled"));
      }
      else {
        tft.print(user::disableFansTimer);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.setCursor(tft.getFontX() + 6, 250);
        tft.print(F("mins"));
      }
      disableFansTimerEndPos = tft.getFontX();
      previousDisableFansTimer = user::disableFansTimer;
    }
  }
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(330, 116, F("Co2 settings"));
      tft.print(142, 166, F("Manual"));
      tft.print(130, 196, F("duration"));
      tft.print(380, 166, F("Gas"));
      tft.print(358, 196, F("duration"));
      tft.print(592, 166, F("Disable"));
      tft.print(624, 196, F("Co2"));
      infoButton(770, 120);
      tft.setFont(&akashi_36px_Regular);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    }
    // ENABLE MANUAL CO2 DURATION
    static bool previousenableManualCo2Duration;
    if (display::refreshPage || user::enableManualCo2Duration != previousenableManualCo2Duration) {
      tft.fillRoundRect(139, 309, 152, 42, 5, RA8875_BLACK);
      if (!user::enableManualCo2Duration) {
        tft.fillRoundRect(140, 310, 75, 40, 5, RA8875_RED);
        tft.fillRoundRect(215, 310, 75, 40, 5, display::RA8875_DARKGREY);
      }
      else {
        tft.fillRoundRect(140, 310, 75, 40, 5, display::RA8875_DARKGREY);
        tft.fillRoundRect(215, 310, 75, 40, 5, RA8875_GREEN);
      }
      if (user::enableManualCo2Duration)
        drawUpDownButtons(390, 410, 478, 410, RA8875_BLUE);
      else
        drawUpDownButtons(390, 410, 478, 410, display::RA8875_DARKGREY);
      previousenableManualCo2Duration = user::enableManualCo2Duration;
    }
    // DISABLE CO2 CONTROL
    static bool previousDisableCo2Control;
    if (display::refreshPage || user::disableCo2Control != previousDisableCo2Control) {
      tft.fillRoundRect(584, 309, 152, 42, 5, RA8875_BLACK);
      if (!user::disableCo2Control) {
        tft.fillRoundRect(585, 310, 75, 40, 5, RA8875_RED);
        tft.fillRoundRect(660, 310, 75, 40, 5, display::RA8875_DARKGREY);
      }
      else {
        tft.fillRoundRect(585, 310, 75, 40, 5, display::RA8875_DARKGREY);
        tft.fillRoundRect(660, 310, 75, 40, 5, RA8875_GREEN);
      }
      previousDisableCo2Control = user::disableCo2Control;
    }
    // CO2 MANUAL GAS DURATION TIMER
    static int16_t manualCo2GasDurationStartPos = 0, manualCo2GasDurationEndPos = 0;
    static float previousManualCo2GasDuration;
    if (display::refreshPage || user::manualCo2GasDuration != previousManualCo2GasDuration) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.setFont(&HallfeticaLargenum_42px_Regular);
      tft.setFontScale(1);
      tft.fillRect(manualCo2GasDurationStartPos, 306, manualCo2GasDurationEndPos - manualCo2GasDurationStartPos, 50, user::backgroundColor);
      if (user::manualCo2GasDuration < 10)
        manualCo2GasDurationStartPos = 376;
      else
        manualCo2GasDurationStartPos = 356;
      tft.print(manualCo2GasDurationStartPos, 308, user::manualCo2GasDuration);
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setCursor(tft.getFontX() + 4, 316);
      user::manualCo2GasDuration == 1 ? tft.print(F("min")) : tft.print(F("mins"));
      manualCo2GasDurationEndPos = tft.getFontX();
      previousManualCo2GasDuration = user::manualCo2GasDuration;
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void waterPage() {
  if (display::refreshPage) {
    drawSlideIcons(110, 112, display::waterPageScrollPos, 5);
  }
  if (display::waterPageScrollPos == 0) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(260, 116, F("Target water level"));
      tft.print(176, 170, F("Minimum"));
      tft.print(530, 170, F("Maximum"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    if (user::convertToInches)
      drawTwoValues(285, user::targetMinWaterHeightInches, RA8875_BLACK, 0, 650, user::targetMaxWaterHeightInches, RA8875_BLACK, 0, "\"", 0);
    else
      drawTwoValues(285, user::targetMinWaterHeight, RA8875_BLACK, 0, 650, user::targetMaxWaterHeight, RA8875_BLACK, 0, "CM", 50);
  }
  else if (display::waterPageScrollPos == 1) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(260, 116, F("Target water temp"));
      tft.print(176, 170, F("Minimum"));
      tft.print(530, 170, F("Maximum"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    if (user::convertToF)
      drawTwoValues(285, user::targetMinWaterTempF, RA8875_BLACK, 1, 650, user::targetMaxWaterTempF, RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, user::targetMinWaterTemp, RA8875_BLACK, 1, 650, user::targetMaxWaterTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::waterPageScrollPos == 2) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(300, 116, F("Tank dimensions"));
      tft.print(180, 170, F("Length"));
      tft.print(540, 170, F("Width"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    if (user::convertToInches)
      drawTwoValues(285, user::waterTankLengthInches, RA8875_BLACK, 0, 650, user::waterTankWidthInches, RA8875_BLACK, 0, "\"", 0); 
    else
      drawTwoValues(285, user::waterTankLength, RA8875_BLACK, 0, 650, user::waterTankWidth, RA8875_BLACK, 0, "CM", 50);    
  }
  else if (display::waterPageScrollPos == 3) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(140, 140, F("Auto drain dates"));
      tft.print(510, 200, F("Disable drain"));
      tft.print(510, 250, F("and refill"));
      tft.print(100, 420, F("Time"));
      drawMiniButtonIncrements(385, 440);
      infoButton(770, 120);
    }
    if (display::refreshPage || display::refreshCalander) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      int16_t calanderX = 110, calanderY = 205;
      for (uint8_t i = 1; i < 32; i++) {
        if (i < 10)
          tft.setCursor(calanderX + 14, calanderY);
        else if (i < 19)
          tft.setCursor(calanderX + 6, calanderY);
        else if (i == 21 || i == 31)
          tft.setCursor(calanderX + 6, calanderY);
        else
          tft.setCursor(calanderX + 2, calanderY);
        if (bitRead(user::autoFillDays, i - 1)) {
          tft.fillRect(calanderX - 1, calanderY + 1, 52, 38, RA8875_BLUE);
          tft.setTextColor(RA8875_BLACK, RA8875_BLUE);
        }
        else {
          tft.fillRect(calanderX - 1, calanderY + 1, 52, 38, RA8875_WHITE);
          tft.setTextColor(RA8875_BLACK, RA8875_WHITE);
        }
        //tft.drawRect(calanderX - 2, calanderY, 54, 40, RA8875_BLACK);
        tft.print(i);
        calanderX += 54;
        if (i % 7 == 0) {
          calanderX = 110;
          calanderY += 40;
        }
      }
      display::refreshCalander = false;
    }
    static uint8_t previousAutoFillMinute;
    static int16_t autoFillTimePosition;
    if (display::refreshPage || user::autoFillMinute != previousAutoFillMinute) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.fillRect(204, 418, autoFillTimePosition - 204, 42, user::backgroundColor);
      tft.setCursor(206, 420);
      if (user::autoFillHour < 10)
        tft.print(F("0"));
      tft.print(user::autoFillHour);
      tft.print(F(":"));
      if (user::autoFillMinute < 10)
        tft.print(F("0"));
      tft.print(user::autoFillMinute);
      previousAutoFillMinute = user::autoFillMinute;
      autoFillTimePosition = tft.getFontX();
    }
    static bool previousdisableDrainAndRefill;
    if (display::refreshPage || user::disableDrainAndRefill != previousdisableDrainAndRefill) {
      drawRadioButton(565, 320, user::disableDrainAndRefill, false);
      previousdisableDrainAndRefill = user::disableDrainAndRefill;
    }
  }
  else if (display::waterPageScrollPos == 4) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(276, 110, F("Refill doser mls"));
      drawFourUpDownButtons(60);
      infoButton(770, 120);
    }
    if (display::refillDoserPageScrollPos > 0) {
      tft.fillTriangle(95, 240, 115, 200, 115, 280, RA8875_BLUE);
    }
    if (display::refillDoserPageScrollPos < user::numberOfDosers - 4) {
      tft.fillTriangle(779, 200, 799, 240, 779, 280, RA8875_BLUE);
    }
    int16_t startPosition = display::refillDoserPageScrollPos * 178;
    // The below method may look odd but it is much faster than a for loop "after testing"
    if (display::refillDoserPageScrollPos == 0) { // DOSER 1
      static uint8_t previousDoserOneMode;
      static int16_t doserOneMlsPosition, previousDoserOneMls;
      drawDoser(100 - startPosition, 150, 1, user::refillDoserOneMills, previousDoserOneMls, doserOneMlsPosition, 0, 0);
      drawEcPhButton(149 - startPosition, 300, user::doserOneMode, previousDoserOneMode, false);
    }
    if (display::refillDoserPageScrollPos <= 1) { // DOSER 2
      static uint8_t previousDoserTwoMode;
      static int16_t doserTwoMlsPosition, previousDoserTwoMls;
      drawDoser(278 - startPosition, 150, 2, user::refillDoserTwoMills, previousDoserTwoMls, doserTwoMlsPosition, 0, 0);
      drawEcPhButton(327 - startPosition, 300, user::doserTwoMode, previousDoserTwoMode, false);
    }
    if (display::refillDoserPageScrollPos <= 2) { // DOSER 3
      static uint8_t previousDoserThreeMode;
      static int16_t doserThreeMlsPosition, previousDoserThreeMls;
      drawDoser(456 - startPosition, 150, 3, user::refillDoserThreeMills, previousDoserThreeMls, doserThreeMlsPosition, 0, 0);
      drawEcPhButton(505 - startPosition, 300, user::doserThreeMode, previousDoserThreeMode, false);
    }
    if (display::refillDoserPageScrollPos <= 2) { // DOSER 4
      static uint8_t previousDoserFourMode;
      static int16_t doserFourMlsPosition, previousDoserFourMls;
      drawDoser(634 - startPosition, 150, 4, user::refillDoserFourMills, previousDoserFourMls, doserFourMlsPosition, 0, 0);
      drawEcPhButton(683 - startPosition, 300, user::doserFourMode, previousDoserFourMode, false);
    }
    if (display::refillDoserPageScrollPos >= 1) { // DOSER 5
      static uint8_t previousDoserFiveMode;
      static int16_t doserFiveMlsPosition, previousDoserFiveMls;
      drawDoser(812 - startPosition, 150, 5, user::refillDoserFiveMills, previousDoserFiveMls, doserFiveMlsPosition, 0, 0);
      drawEcPhButton(861 - startPosition, 300, user::doserFiveMode, previousDoserFiveMode, false);
    }
    if (display::refillDoserPageScrollPos == 2) { // DOSER 6
      static uint8_t previousDoserSixMode;
      static int16_t doserSixMlsPosition, previousDoserSixMls;
      drawDoser(990 - startPosition, 150, 6, user::refillDoserSixMills, previousDoserSixMls, doserSixMlsPosition, 0, 0);
      drawEcPhButton(1039 - startPosition, 300, user::doserSixMode, previousDoserSixMode, false);
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void dosersPage() {
  if (display::refreshPage) {
    drawFourUpDownButtons(0);
    infoButton(770, 120);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    if (user::numberOfDosers > 4) {
      if (display::doserPageScrollPos > 0) {
        tft.fillTriangle(95, 240, 115, 200, 115, 280, RA8875_BLUE);
      }
      if (display::doserPageScrollPos < user::numberOfDosers - 4) {
        tft.fillTriangle(779, 200, 799, 240, 779, 280, RA8875_BLUE);
      }
    }
  }
  static uint8_t prevDoserIsPriming = 0;
  int16_t startPosition = display::doserPageScrollPos * 178;
  if (display::doserPageScrollPos == 0) { // DOSER 1
    static uint8_t previousDoserOneMode;
    static int16_t doserOneMlsPosition, previousDoserOneMls;
    drawDoser(100 - startPosition, 90, 1, user::doserOneMills, previousDoserOneMls, doserOneMlsPosition, 0, 0);
    drawEcPhButton(149 - startPosition, 255, user::doserOneMode, previousDoserOneMode, true);
    if (display::refreshPage || bitRead(device::doserIsPriming, 0) != bitRead(prevDoserIsPriming, 0)) {
      bitRead(device::doserIsPriming, 0) ? stopButton(136 - startPosition, 443) : primeButton(136 - startPosition, 443);
      bitWrite(prevDoserIsPriming, 0, bitRead(device::doserIsPriming, 0));
    }
  }
  if (display::doserPageScrollPos <= 1) { // DOSER 2
    static uint8_t previousDoserTwoMode;
    static int16_t doserTwoMlsPosition, previousDoserTwoMls;
    drawDoser(278 - startPosition, 90, 2, user::doserTwoMills, previousDoserTwoMls, doserTwoMlsPosition, 0, 0);
    drawEcPhButton(327 - startPosition, 255, user::doserTwoMode, previousDoserTwoMode, true);
    if (display::refreshPage || bitRead(device::doserIsPriming, 1) != bitRead(prevDoserIsPriming, 1)) {
      bitRead(device::doserIsPriming, 1) ? stopButton(314 - startPosition, 443) : primeButton(314 - startPosition, 443);
      bitWrite(prevDoserIsPriming, 1, bitRead(device::doserIsPriming, 1));
    }
  }
  if (display::doserPageScrollPos <= 2) { // DOSER 3
    static uint8_t previousDoserThreeMode;
    static int16_t doserThreeMlsPosition, previousDoserThreeMls;
    drawDoser(456 - startPosition, 90, 3, user::doserThreeMills, previousDoserThreeMls, doserThreeMlsPosition, 0, 0);
    drawEcPhButton(505 - startPosition, 255, user::doserThreeMode, previousDoserThreeMode, true);
    if (display::refreshPage || bitRead(device::doserIsPriming, 2) != bitRead(prevDoserIsPriming, 2)) {
      bitRead(device::doserIsPriming, 2) ? stopButton(492 - startPosition, 443) : primeButton(492 - startPosition, 443);
      bitWrite(prevDoserIsPriming, 2, bitRead(device::doserIsPriming, 2));
    }
  }
  if (display::doserPageScrollPos <= 2) { // DOSER 4
    static uint8_t previousDoserFourMode;
    static int16_t doserFourMlsPosition, previousDoserFourMls;
    drawDoser(634 - startPosition, 90, 4, user::doserFourMills, previousDoserFourMls, doserFourMlsPosition, 0, 0);
    drawEcPhButton(683 - startPosition, 255, user::doserFourMode, previousDoserFourMode, true);
    if (display::refreshPage || bitRead(device::doserIsPriming, 3) != bitRead(prevDoserIsPriming, 3)) {
      bitRead(device::doserIsPriming, 3) ? stopButton(670 - startPosition, 443) : primeButton(670 - startPosition, 443);
      bitWrite(prevDoserIsPriming, 3, bitRead(device::doserIsPriming, 3));
    }
  }
  if (display::doserPageScrollPos >= 1) { // DOSER 5
    static uint8_t previousDoserFiveMode;
    static int16_t doserFiveMlsPosition, previousDoserFiveMls;
    drawDoser(812 - startPosition, 90, 5, user::doserFiveMills, previousDoserFiveMls, doserFiveMlsPosition, 0, 0);
    drawEcPhButton(861 - startPosition, 255, user::doserFiveMode, previousDoserFiveMode, true);
    if (display::refreshPage || bitRead(device::doserIsPriming, 4) != bitRead(prevDoserIsPriming, 4)) {
      bitRead(device::doserIsPriming, 4) ? stopButton(848 - startPosition, 443) : primeButton(848 - startPosition, 443);
      bitWrite(prevDoserIsPriming, 4, bitRead(device::doserIsPriming, 4));
    }
  }
  if (display::doserPageScrollPos >= 2) { // DOSER 6
    static uint8_t previousDoserSixMode;
    static int16_t doserSixMlsPosition, previousDoserSixMls;
    drawDoser(990 - startPosition, 90, 6, user::doserSixMills, previousDoserSixMls, doserSixMlsPosition, 0, 0);
    drawEcPhButton(1039 - startPosition, 255, user::doserSixMode, previousDoserSixMode, true);
    if (display::refreshPage || bitRead(device::doserIsPriming, 5) != bitRead(prevDoserIsPriming, 5)) {
      bitRead(device::doserIsPriming, 5) ? stopButton(1026 - startPosition, 443) : primeButton(1026 - startPosition, 443);
      bitWrite(prevDoserIsPriming, 5, bitRead(device::doserIsPriming, 5));
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void timerPage() {
  if (display::refreshPage)
    drawSlideIcons(110, 112, display::timerPagePageScrollPos, 4);
  if (display::timerPagePageScrollPos == 0) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(300, 116, F("Light settings"));
      tft.print(185, 166, F("Light on"));
      tft.print(565, 166, F("Light off"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    }
    if (user::lightMode == 0) {
      static int16_t textStartX = 0, textEndX =0;
      static int16_t previousLightOnDuration;
      drawCyclicTimer(textStartX, textEndX, 280, 260, user::lightOnDuration, previousLightOnDuration);  
      static int16_t textStartX2 = 0, textEndX2 =0;
      static int16_t previousLightOffDuration;
      drawCyclicTimer(textStartX2, textEndX2, 650, 260, user::lightOffDuration, previousLightOffDuration); 
    }
    else {
      static uint8_t previousOnTime;
      drawTimer(180, 260, previousOnTime, user::lightOnTimeHour, user::lightOnTimeMin);
      static uint8_t previousOffTime;
      drawTimer(560, 260, previousOffTime, user::lightOffTimeHour, user::lightOffTimeMin);
    }
  }
  // AUX RELAY 1 CYCLIC TIMER
  else if (display::timerPagePageScrollPos == 1) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(260, 116, F("Aux relay 1 settings"));
      tft.print(185, 166, F("Timer on"));
      tft.print(565, 166, F("Timer off"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    }
    if (user::auxRelayOneMode == 0) {
      static int16_t textStartX = 0, textEndX =0;
      static int16_t previousOnDuration;
      drawCyclicTimer(textStartX, textEndX, 280, 260, user::auxRelayOneOnDuration, previousOnDuration);  
      static int16_t textStartX2 = 0, textEndX2 =0;
      static int16_t previousOffDuration;
      drawCyclicTimer(textStartX2, textEndX2, 650, 260, user::auxRelayOneOffDuration, previousOffDuration); 
    }
    else {
      static uint8_t previousOnTime;
      drawTimer(180, 260, previousOnTime, user::auxRelayOneOnTimeHour, user::auxRelayOneOnTimeMin);
      static uint8_t previousOffTime;
      drawTimer(560, 260, previousOffTime, user::auxRelayOneOffTimeHour, user::auxRelayOneOffTimeMin);
    }
  }
  // AUX RELAY 2 CYCLIC TIMER
  else if (display::timerPagePageScrollPos == 2) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(260, 116, F("Aux relay 2 settings"));
      tft.print(185, 166, F("Timer on"));
      tft.print(565, 166, F("Timer off"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    }
    if (user::auxRelayTwoMode == 0) {
      static int16_t textStartX = 0, textEndX =0;
      static int16_t previousOnDuration;
      drawCyclicTimer(textStartX, textEndX, 280, 260, user::auxRelayTwoOnDuration, previousOnDuration);  
      static int16_t textStartX2 = 0, textEndX2 =0;
      static int16_t previousOffDuration;
      drawCyclicTimer(textStartX2, textEndX2, 650, 260, user::auxRelayTwoOffDuration, previousOffDuration); 
    }
    else {
      static uint8_t previousOnTime;
      drawTimer(180, 260, previousOnTime, user::auxRelayTwoOnTimeHour, user::auxRelayTwoOnTimeMin);
      static uint8_t previousOffTime;
      drawTimer(560, 260, previousOffTime, user::auxRelayTwoOffTimeHour, user::auxRelayTwoOffTimeMin);
    }
  }
  // Timer settings
  else {
    if (display::refreshPage) {   
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(320, 116, F("Timer settings"));
      tft.print(120, 330, F("Light cyclic/24hr timer"));
      tft.print(120, 380, F("Aux 1 cyclic/24hr timer"));
      tft.print(120, 430, F("Aux 2 cyclic/24hr timer"));
      infoButton(770, 120);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    }
    // Light buttons
    static uint8_t prevLightMode, previousLightState;    
    if (display::refreshPage || user::lightState != previousLightState || user::lightMode != prevLightMode) {   
      tft.fillRect(118, 180, 410, 34, user::backgroundColor);
      tft.print(120, 180, F("Light on/")); (user::lightMode ?  tft.print(F("cyclic")) : tft.print(F("timer"))); tft.print(F("/off"));
      drawTriStateButton(600, 180, user::lightState);
      drawRadioButton(630, 330, user::lightMode, true);
      prevLightMode = user::lightMode;
      previousLightState = user::lightState;
    }
    // Aux 1 buttons
    static uint8_t prevRelayOneMode, previousRelayOneState;
    if (display::refreshPage || user::auxRelayOneState != previousRelayOneState || user::auxRelayOneMode != prevRelayOneMode) {   
      tft.fillRect(118, 230, 410, 34, user::backgroundColor);
      tft.print(120, 230, F("Aux 1 on/")); (user::auxRelayOneMode ?  tft.print(F("cyclic")) : tft.print(F("timer"))); tft.print(F("/off"));
      drawTriStateButton(600, 230, user::auxRelayOneState);
      drawRadioButton(630, 380, user::auxRelayOneMode, true);
      prevRelayOneMode = user::auxRelayOneMode;
      previousRelayOneState = user::auxRelayOneState;
    }
    // Aux 2 buttons
    static uint8_t prevRelayTwoMode, previousRelayTwoState;
    if (display::refreshPage || user::auxRelayTwoState != previousRelayTwoState || user::auxRelayTwoMode != prevRelayTwoMode) {   
      tft.fillRect(118, 280, 410, 34, user::backgroundColor);
      tft.print(120, 280, F("Aux 2 on/")); (user::auxRelayTwoMode ?  tft.print(F("cyclic")) : tft.print(F("timer"))); tft.print(F("/off"));
      drawTriStateButton(600, 280, user::auxRelayTwoState);
      drawRadioButton(630, 430, user::auxRelayTwoMode, true);
      prevRelayTwoMode = user::auxRelayTwoMode;
      previousRelayTwoState = user::auxRelayTwoState;
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void fansPage() {
  if (display::refreshPage)
    drawSlideIcons(110, 112, display::fansPage, 8);
  if (display::fansPage == 0) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(300, 124, F("Fan one target"));
      tft.print(180, 166, F("Min speed"));
      tft.print(506, 166, F("Max speed"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetMinFanOneSpeed, RA8875_BLACK, 0, 620, user::targetMaxFanOneSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::fansPage == 1) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(300, 124, F("Fan two target"));
      tft.print(180, 166, F("Min speed"));
      tft.print(506, 166, F("Max speed"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetMinFanTwoSpeed, RA8875_BLACK, 0, 620, user::targetMaxFanTwoSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::fansPage == 2) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(266, 124, F("Target day air temp"));
      tft.print(176, 170, F("Minimum"));
      tft.print(530, 170, F("Maximum"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    if (user::convertToF)
      drawTwoValues(285, user::targetDayMinAirTempF, RA8875_BLACK, 1, 650, user::targetDayMaxAirTempF, RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, user::targetDayMinAirTemp, RA8875_BLACK, 1, 650, user::targetDayMaxAirTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::fansPage == 3) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(260, 124, F("Target day humidity"));
      tft.print(176, 170, F("Minimum"));
      tft.print(530, 170, F("Maximum"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetDayMinHumidity, RA8875_BLACK, 1, 650, user::targetDayMaxHumidity, RA8875_BLACK, 1, "%", 50);
  }
  else if (display::fansPage == 4) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(266, 124, F("Target night temp"));
      tft.print(176, 170, F("Minimum"));
      tft.print(530, 170, F("Maximum"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    if (user::convertToF)
      drawTwoValues(285, user::targetNightMinAirTempF, RA8875_BLACK, 1, 650, user::targetNightMaxAirTempF, RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, user::targetNightMinAirTemp, RA8875_BLACK, 1, 650, user::targetNightMaxAirTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::fansPage == 5) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(250, 124, F("Target night humidity"));
      tft.print(176, 170, F("Minimum"));
      tft.print(530, 170, F("Maximum"));
      drawUpDownButtons(210, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(580, 400, 680, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetNightMinHumidity, RA8875_BLACK, 1, 650, user::targetNightMaxHumidity, RA8875_BLACK, 1, "%", 50);
  }
  else if (display::fansPage == 6) {
    tft.setFont(&akashi_36px_Regular);
    if (display::refreshPage) {
      infoButton(720, 120);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(186, 126, F("Select your AC protocol"));
      testButton(350, 415);      
    }
    static int prevTitlePos = 0, prevSelectedIrPos = 0;
    if (display::refreshPage || display::irCodesPageScrollPos != prevTitlePos || user::selectedIrPos != prevSelectedIrPos) {
      drawVerticalSlider(756, 100, 370, map(display::irCodesPageScrollPos, 0, totalIRTitles - 6, 0, 100));
      //Serial.print(F("irCodesPageScrollPos =")); Serial.println(irCodesPageScrollPos);
      int xPos = 130, yPos = 172;
      for (int i = display::irCodesPageScrollPos; i < display::irCodesPageScrollPos + 5; i++) {
        if (i == user::selectedIrPos) {
          //Serial.print(F("IR command [")); Serial.print(i); Serial.print(F("]")); Serial.println(irTitles[i]);  
          tft.setTextColor(RA8875_BLUE, user::backgroundColor);                 
        }
        else {
          tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        }
        tft.print(xPos, yPos, irTitles[i]);
        drawMiniConfirmButton(650, yPos);
        yPos += 45;
      }
      prevSelectedIrPos = user::selectedIrPos;
      prevTitlePos = display::irCodesPageScrollPos;
    }
  }
    else if (display::fansPage == 7) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(276, 126, F("Fan and AC settings"));
      infoButton(770, 120);
      int scrollPercentage = map(display::airSettingsScrollPos, 0, 5, 0, 100);
      drawVerticalSlider(756, 100, 370, scrollPercentage);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    }

    int scrollMargin = display::airSettingsScrollPos * 50;
    if (display::airSettingsScrollPos == 0) {
      static bool previousFansControlTemperature;
      if (display::refreshPage || user::fansControlTemperature != previousFansControlTemperature) {
        tft.print(110, 180 - scrollMargin, F("Fans control temperature"));
        drawRadioButton(640, 180 - scrollMargin, user::fansControlTemperature, false);
        previousFansControlTemperature = user::fansControlTemperature;
      }
    }
    if (display::airSettingsScrollPos <= 1) {
      static bool previousFansControlHumidity;
      if (display::refreshPage || user::fansControlHumidity != previousFansControlHumidity) {
        tft.print(110, 230 - scrollMargin, F("Fans control humidity"));
        drawRadioButton(640, 230 - scrollMargin, user::fansControlHumidity, false);
        previousFansControlHumidity = user::fansControlHumidity;
      }
    }
    if (display::airSettingsScrollPos <= 2) {
      static bool previousFanOneFixedSpeed;
      if (display::refreshPage || user::fanOneFixedSpeed != previousFanOneFixedSpeed) {
        tft.print(110, 280 - scrollMargin, F("Fan one fixed speed"));
        drawRadioButton(640, 280 - scrollMargin, user::fanOneFixedSpeed, false);
        previousFanOneFixedSpeed = user::fanOneFixedSpeed;
      }
    }
    if (display::airSettingsScrollPos <= 3) {
      static bool previousFanTwoFixedSpeed;
      if (display::refreshPage || user::fanTwoFixedSpeed != previousFanTwoFixedSpeed) {
        tft.print(110, 330 - scrollMargin, F("Fan two fixed speed"));
        drawRadioButton(640, 330 - scrollMargin, user::fanTwoFixedSpeed, false);
        previousFanTwoFixedSpeed = user::fanTwoFixedSpeed;
      }
    }
    if (display::airSettingsScrollPos <= 4) {
      static bool previousFanOneEnabled;
      if (display::refreshPage || user::fanOneEnabled != previousFanOneEnabled) {
        tft.print(110, 380 - scrollMargin, F("Fan one enabled"));
        drawRadioButton(640, 380 - scrollMargin, user::fanOneEnabled, false);
        previousFanOneEnabled = user::fanOneEnabled;
      }
    }
    if (display::airSettingsScrollPos <= 5) {
      static bool previousFanTwoEnabled;
      if (display::refreshPage || user::fanTwoEnabled != previousFanTwoEnabled) {
        tft.print(110, 430 - scrollMargin, F("Fan two enabled"));
        drawRadioButton(640, 430 - scrollMargin, user::fanTwoEnabled, false);
        previousFanTwoEnabled = user::fanTwoEnabled;
      }
    }
    if (display::airSettingsScrollPos >= 1 && display::airSettingsScrollPos <= 6) {
      static bool previousAcControlTemp;
      if (display::refreshPage || user::acControlTemp != previousAcControlTemp) {
        tft.print(110, 480 - scrollMargin, F("AC control temperature"));
        drawRadioButton(640, 480 - scrollMargin, user::acControlTemp, false);
        previousAcControlTemp = user::acControlTemp;
      }
    }
    if (display::airSettingsScrollPos >= 2 && display::airSettingsScrollPos <= 67) {
      static bool previousAcControlHumidity;
      if (display::refreshPage || user::acControlHumidity != previousAcControlHumidity) {
        tft.print(110, 530 - scrollMargin, F("AC control humidity"));
        drawRadioButton(640, 530 - scrollMargin, user::acControlHumidity, false);
        previousAcControlHumidity = user::acControlHumidity;
      }
    }
    if (display::airSettingsScrollPos >= 3 && display::airSettingsScrollPos <= 8) {
      static bool previousAcSwingVertical;
      if (display::refreshPage || user::acSwingVertical != previousAcSwingVertical) {
        tft.print(110, 580 - scrollMargin, F("AC swing vertical"));
        drawRadioButton(640, 580 - scrollMargin, user::acSwingVertical, false);
        previousAcSwingVertical = user::acSwingVertical;
      }
    }
    if (display::airSettingsScrollPos >= 4 && display::airSettingsScrollPos <= 9) {
      static bool previousAcSwingHorizontal;
      if (display::refreshPage || user::acSwingHorizontal != previousAcSwingHorizontal) {
        tft.print(110, 630 - scrollMargin, F("AC swing horizontal"));
        drawRadioButton(640, 630 - scrollMargin, user::acSwingHorizontal, false);
        previousAcSwingHorizontal = user::acSwingHorizontal;
      }
    }
    if (display::airSettingsScrollPos >= 5 && display::airSettingsScrollPos <= 10) {
      static bool previousAcTurboMode;
      if (display::refreshPage || user::acTurboMode != previousAcTurboMode) {
        tft.print(110, 680 - scrollMargin, F("AC turbo mode"));
        drawRadioButton(640, 680 - scrollMargin, user::acTurboMode, false);
        previousAcTurboMode = user::acTurboMode;
      }
    }
  }
}

// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void warningsPage() {
  uint16_t color;
  if (display::refreshPage) {
    drawSlideIcons(110, 112, display::warningsPage, 8);
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(116, 170, F("Current reading"));
    tft.print(470, 170, F("Error margin"));
    drawUpDownButtons(554, 400, 664, 400, RA8875_BLUE);
    infoButton(770, 120);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
  }
  if (display::warningsPage == 0) {
    if (display::refreshPage)
      tft.print(310, 126, F("PPM warning"));
    color = setWarningColor(sensor::tds, user::targetMinTds, user::targetMaxTds, user::tdsErrorMargin);
    drawTwoValues(285, sensor::tds, color, 0, 620, user::tdsErrorMargin, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::warningsPage == 1) {
    if (display::refreshPage)
      tft.print(324, 126, F("EC warning"));
    color = setWarningColor(sensor::ec, user::targetMinEc, user::targetMaxEc, user::ecErrorMargin);
    drawTwoValues(285, sensor::ec, color, 2, 620, user::ecErrorMargin, RA8875_BLACK, 2, NULL, 0);
  }
  else if (display::warningsPage == 2) {
    if (display::refreshPage)
      tft.print(324, 126, F("PH warning"));
    color = setWarningColor(sensor::ph, user::targetMinPh, user::targetMaxPh, user::phErrorMargin);
    drawTwoValues(285, sensor::ph, color, 2, 620, user::phErrorMargin, RA8875_BLACK, 2, NULL, 0);
  }
  else if (display::warningsPage == 3) {
    if (display::refreshPage)
      tft.print(310, 126, F("Co2 warning"));
    color = setWarningColor(sensor::co2, user::targetCo2, user::targetCo2, user::co2ErrorMargin);
    drawTwoValues(285, sensor::co2, color, 0, 620, user::co2ErrorMargin, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::warningsPage == 4) {
    if (display::refreshPage)
      tft.print(246, 126, F("Water temp warning"));
    if (user::convertToF) {
      color = setWarningColor(convertToF(sensor::waterTemp), user::targetMinWaterTempF, user::targetMaxWaterTempF, user::waterTempErrorMarginF);
      drawTwoValues(285, convertToF(sensor::waterTemp), color, 1, 620, user::waterTempErrorMarginF, RA8875_BLACK, 1, "F", 0);
    }
    else {
      color = setWarningColor(sensor::waterTemp, user::targetMinWaterTemp, user::targetMaxWaterTemp, user::waterTempErrorMargin);
      drawTwoValues(285, sensor::waterTemp, color, 1, 620, user::waterTempErrorMargin, RA8875_BLACK, 1, "C", 0);
    }
  }
  else if (display::warningsPage == 5) {
    if (display::refreshPage)
      tft.print(224, 126, F("Water height warning"));
    if (user::convertToInches) {
      color = setWarningColor(convertToInches(sensor::waterLevel), user::targetMinWaterHeightInches, user::targetMaxWaterHeightInches, user::waterHeightErrorMarginInches);
      drawTwoValues(285, convertToInches(sensor::waterLevel), color, 1, 620, user::waterHeightErrorMarginInches, RA8875_BLACK, 1, "\"", 0);
    }
    else {
      color = setWarningColor(sensor::waterLevel, user::targetMinWaterHeight, user::targetMaxWaterHeight, user::waterHeightErrorMargin);
      drawTwoValues(285, sensor::waterLevel, color, 1, 620, user::waterHeightErrorMargin, RA8875_BLACK, 1, "CM", 0);
    }
  }
  else if (display::warningsPage == 6) {
    if (display::refreshPage)
      tft.print(284, 126, F("Air temp warning"));
    if (device::lightOn) {
      if (user::convertToF) {
        color = setWarningColor(convertToF(sensor::airTemp), user::targetDayMinAirTempF, user::targetDayMaxAirTempF, user::airTempErrorMarginF);
        drawTwoValues(285, convertToF(sensor::airTemp), color, 1, 620, user::airTempErrorMarginF, RA8875_BLACK, 1, "F", 0);
      }
      else {
        color = setWarningColor(sensor::airTemp, user::targetDayMinAirTemp, user::targetDayMaxAirTemp, user::airTempErrorMargin);
        drawTwoValues(285, sensor::airTemp, color, 1, 620, user::airTempErrorMargin, RA8875_BLACK, 1, "C", 0);
      }
    }
    else {
      if (user::convertToF) {
        color = setWarningColor(convertToF(sensor::airTemp), user::targetNightMinAirTempF, user::targetNightMaxAirTempF, user::airTempErrorMarginF);
        drawTwoValues(285, convertToF(sensor::airTemp), color, 1, 620, user::airTempErrorMarginF, RA8875_BLACK, 1, "F", 0);
      }
      else {
        color = setWarningColor(sensor::airTemp, user::targetNightMinAirTemp, user::targetNightMaxAirTemp, user::airTempErrorMargin);
        drawTwoValues(285, sensor::airTemp, color, 1, 620, user::airTempErrorMargin, RA8875_BLACK, 1, "C", 0);
      }
    }
  }
  else if (display::warningsPage == 7) {
    if (display::refreshPage)
      tft.print(280, 126, F("Humidity warning"));
    if (device::lightOn)
      color = setWarningColor(sensor::humidity, user::targetDayMinHumidity, user::targetDayMaxHumidity, user::humidityErrorMargin);
    else
      color = setWarningColor(sensor::humidity, user::targetNightMinHumidity, user::targetNightMaxHumidity, user::humidityErrorMargin);
    drawTwoValues(285, sensor::humidity, color, 1, 620, user::humidityErrorMargin, RA8875_BLACK, 1, "%", 50);
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
// settings pages
void drawSettingsPageZero() {
  if (display::showRTCtime)
    displaySetRTCTime();
  else if (display::showAfkTime) 
    displaySetAfkTime();
  else if (display::showBrightness) 
    displaySetBrightness();
  else if (display::showGraphInterval)
    displaySetGraphInterval();
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(110, 130, F("Display"));
      int16_t scrollPercentage = map(display::settingsPageZeroScrollPos, 0, 3, 0, 100);
      drawVerticalSlider(756, 100, 370, scrollPercentage);
    }
    int16_t scrollMargin = display::settingsPageZeroScrollPos * 50;
    if (display::settingsPageZeroScrollPos == 0) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 170 - scrollMargin, F("Background color"));
        backGroundColorButton(508, 168 - scrollMargin, display::RA8875_LIGHT_BLUE);
        backGroundColorButton(558, 168 - scrollMargin, display::RA8875_LIGHT_GREEN);
        backGroundColorButton(608, 168 - scrollMargin, RA8875_MAGENTA);
        backGroundColorButton(658, 168 - scrollMargin, display::RA8875_LIGHT_PINK);
        backGroundColorButton(708, 168 - scrollMargin, RA8875_WHITE);
      }
    }
    if (display::settingsPageZeroScrollPos <= 1) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 220 - scrollMargin,  F("Set display timeout"));
        drawMiniConfirmButton(672, 220 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 2) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 270 - scrollMargin,  F("Set display brightness"));
        drawMiniConfirmButton(672, 270 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 3) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 320 - scrollMargin,  F("Set graph interval"));
        drawMiniConfirmButton(672, 320 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 4) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 370 - scrollMargin,  F("Set datetime"));
        drawMiniConfirmButton(672, 370 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 5) {
      static bool previousTempInF;
      if (display::refreshPage || user::convertToF != previousTempInF) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 420 - scrollMargin,  F("Set temperature In F"));
        drawMiniRadioButton(652, 420 - scrollMargin, user::convertToF);
        previousTempInF = user::convertToF ;
      }
    }
    if (display::settingsPageZeroScrollPos >= 1) {
      static bool previousHeightInInches;
      if (display::refreshPage || user::convertToInches != previousHeightInInches) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 470 - scrollMargin,  F("Set size In Inches"));
        drawMiniRadioButton(652, 470 - scrollMargin, user::convertToInches);
        previousHeightInInches = user::convertToInches;
      }
    }
    if (display::settingsPageZeroScrollPos >= 2) {
      static bool previousConvertToTds;
      if (display::refreshPage || user::convertToTds != previousConvertToTds) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 520 - scrollMargin,  F("Set EC to TDS"));
        drawMiniRadioButton(652, 520 - scrollMargin, user::convertToTds);
        previousConvertToTds = user::convertToTds;
      }
    }
    if (display::settingsPageZeroScrollPos >= 3) {
      static uint8_t previousConversionType;
      if (display::refreshPage || previousConversionType != device::conversionType) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 570 - scrollMargin,  F("Set EC to TDS conversion"));
        drawEcConversionButton(672, 570 - scrollMargin);
        previousConversionType = device::conversionType;
      }
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void drawSettingsPageOne() {
  if (display::showSystemLogs) {
    displaySystemLogs();
  }
  else if (display::showWifiSsid) {
    displayWifiSsid();
  }
  else if (display::showWifiPassword) {
    displayWifiPassword();
  }
  else if (display::refreshPage && user::resetSettings) {
    continueMessage(message::resetDevice, -1, 0, true, true, false);
  }
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(110, 130, F("System"));
      int16_t scrollPercentage = map(display::settingsPageOneScrollPos, 0, 5, 0, 100);
      drawVerticalSlider(760, 100, 370, scrollPercentage);
    }
    int16_t scrollMargin = display::settingsPageOneScrollPos * 50;
    if (display::settingsPageOneScrollPos == 0) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 170 - scrollMargin, F("Show system logs"));
        drawMiniConfirmButton(672, 170 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos <= 1) {
      static bool previousEnableWifi;
      if (display::refreshPage || wifi::wifiEnabled != previousEnableWifi) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 220 - scrollMargin, F("Enable Wifi"));
        drawMiniRadioButton(652, 220 - scrollMargin, wifi::wifiEnabled);
        previousEnableWifi = wifi::wifiEnabled;
      }
    }
    if (display::settingsPageOneScrollPos <= 2) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 270 - scrollMargin, F("Set WiFi SSID"));
        drawMiniConfirmButton(672, 270 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos <= 3) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 320 - scrollMargin, F("Set WiFi password"));
        drawMiniConfirmButton(672, 320 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos <= 4) {
      static bool previousHiddenNetwork;
      if (display::refreshPage || wifi::hiddenNetwork != previousHiddenNetwork) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 370 - scrollMargin, F("Hide WiFi network"));
        drawMiniRadioButton(652, 370 - scrollMargin, wifi::hiddenNetwork);
        previousHiddenNetwork = wifi::hiddenNetwork;
      }
    }
    if (display::settingsPageOneScrollPos <= 5) {
      static bool previousDisableLED;
      if (display::refreshPage || user::disableLED != previousDisableLED) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 420 - scrollMargin, F("Disable LED"));
        drawMiniRadioButton(652, 420 - scrollMargin, user::disableLED);
        previousDisableLED = user::disableLED;
      }
    }
    if (display::settingsPageOneScrollPos >= 1 && display::settingsPageOneScrollPos <= 6) {
      static bool previousDisableBeeper;
      if (display::refreshPage || user::disableBeeper != previousDisableBeeper) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 470 - scrollMargin, F("Disable speaker"));
        drawMiniRadioButton(652, 470 - scrollMargin, user::disableBeeper);
        previousDisableBeeper = user::disableBeeper;
      }
    }
    if (display::settingsPageOneScrollPos >= 2 && display::settingsPageOneScrollPos <= 6) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 520 - scrollMargin, F("Clear graph history"));
        drawMiniConfirmButton(672, 520 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos >= 3 && display::settingsPageOneScrollPos <= 7) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 570 - scrollMargin, F("Clear max mins"));
        drawMiniConfirmButton(672,570 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos >= 4 && display::settingsPageOneScrollPos <= 8) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 620 - scrollMargin, F("Clear system logs"));
        drawMiniConfirmButton(672, 620 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos >= 5 && display::settingsPageOneScrollPos <= 10) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 670 - scrollMargin, F("Factory reset"));
        drawMiniConfirmButton(672, 670 - scrollMargin);
      }
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void drawSettingsPageTwo() {
  int16_t scrollMargin = display::settingsPageTwoScrollPos * 50;
  if (display::showNumberOfDosers) {
    displaySetNumberOfDosers();
  }
  else if (display::showDosingInterval) {
    displaySetDosingInterval();
  }
  else if (display::showEcDosingMode) {
    displaySetEcDosingMode();
  }
  else if (display::showPhDosingMode) {
    displaySetPhDosingMode();
  }
  else if (display::showEcTdsValue) {
    displaySetEcTdsValue();
  }
  else if (display::showPhDownUpValue) {
    displaySetPhDownUpValue();
  }
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(110, 130, F("Enviroment"));
      int16_t scrollPercentage = map(display::settingsPageTwoScrollPos, 0, 1, 0, 100);
      drawVerticalSlider(760, 100, 370, scrollPercentage);
    }
    if (display::settingsPageTwoScrollPos <= 0) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 170 - scrollMargin, F("Set number of dosers"));
        drawMiniConfirmButton(672, 170 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 1) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 220 - scrollMargin, F("Set dosing interval"));
        drawMiniConfirmButton(672, 220 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 2) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 270 - scrollMargin, F("Set EC dosing mode"));
        drawMiniConfirmButton(672, 270 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 3) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 320 - scrollMargin, F("Set PH dosing mode"));
        drawMiniConfirmButton(672, 320 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 4) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 370 - scrollMargin, F("Set EC/TDS value"));
        drawMiniConfirmButton(672, 370 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 5) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 420 - scrollMargin, F("Set PH down/up value"));
        drawMiniConfirmButton(672, 420 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos >= 1 && display::settingsPageTwoScrollPos <= 2) {
      static user::heightSensors previousUseEtapeSensor;
      if (display::refreshPage || user::heightSensor != previousUseEtapeSensor) { 
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 470 - scrollMargin, F("Water height sensor"));
        drawMiniEtapeButton(612, 470 - scrollMargin, user::heightSensor);
        previousUseEtapeSensor = user::heightSensor;
      }
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void drawSettingsPageThree() {
  if (display::refreshPage) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLUE, user::backgroundColor);
    tft.print(110, 130, F("Warnings"));
    int16_t scrollPercentage = map(display::settingsPageThreeScrollPos, 0, 2, 0, 100);
    drawVerticalSlider(760, 100, 370, scrollPercentage);
  }
  int16_t scrollMargin = display::settingsPageThreeScrollPos * 50;
  if (display::settingsPageThreeScrollPos == 0) {
    static bool previousDisablePpmWarnings;
    if (display::refreshPage || user::disablePpmWarnings != previousDisablePpmWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::ppmErrorState);
      tft.print(110, 170 - scrollMargin, F("Disable PPM"));
      drawMiniRadioButton(652, 170 - scrollMargin, user::disablePpmWarnings);
      previousDisablePpmWarnings = user::disablePpmWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 1) {
    static bool previousDisableEcWarnings;
    if (display::refreshPage || user::disableEcWarnings != previousDisableEcWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::ecErrorState);
      tft.print(110, 220 - scrollMargin, F("Disable EC"));
      drawMiniRadioButton(652, 220 - scrollMargin, user::disableEcWarnings);
      previousDisableEcWarnings = user::disableEcWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 2) {
    static bool previousDisablePhWarnings;
    if (display::refreshPage || user::disablePhWarnings != previousDisablePhWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::phErrorState);
      tft.print(110, 270 - scrollMargin, F("Disable PH"));
      drawMiniRadioButton(652, 270 - scrollMargin, user::disablePhWarnings);
      previousDisablePhWarnings = user::disablePhWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 3) {
    static bool previousDisableCo2Warnings;
    if (display::refreshPage || user::disableCo2Warnings != previousDisableCo2Warnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::co2ErrorState);
      tft.print(110, 320 - scrollMargin, F("Disable Co2"));
      drawMiniRadioButton(652, 320 - scrollMargin, user::disableCo2Warnings);
      previousDisableCo2Warnings = user::disableCo2Warnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 4) {
    static bool previousDisableWaterTempWarnings;
    if (display::refreshPage || user::disableWaterTempWarnings != previousDisableWaterTempWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::waterTempErrorState);
      tft.print(110, 370 - scrollMargin, F("Disable water temp"));
      drawMiniRadioButton(652, 370 - scrollMargin, user::disableWaterTempWarnings);
      previousDisableWaterTempWarnings = user::disableWaterTempWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 5) {
    static bool previousDisableWaterHeightWarnings;
    if (display::refreshPage || user::disableWaterHeightWarnings != previousDisableWaterHeightWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::waterLevelErrorState);
      tft.print(110, 420 - scrollMargin, F("Disable water height"));
      drawMiniRadioButton(652, 420 - scrollMargin, user::disableWaterHeightWarnings);
      previousDisableWaterHeightWarnings = user::disableWaterHeightWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos >= 1 && display::settingsPageThreeScrollPos <= 6) {
    static bool previousDisableAirTempWarnings;
    if (display::refreshPage || user::disableAirTempWarnings != previousDisableAirTempWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::airTempErrorState);
      tft.print(110, 470 - scrollMargin, F("Disable air temp"));
      drawMiniRadioButton(652, 470 - scrollMargin, user::disableAirTempWarnings);
      previousDisableAirTempWarnings = user::disableAirTempWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos >= 2 && display::settingsPageThreeScrollPos <= 7) {
    static bool previousDisableHumidityWarnings;
    if (display::refreshPage || user::disableHumidityWarnings != previousDisableHumidityWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::humidityErrorState);
      tft.print(110, 520 - scrollMargin, F("Disable humidity"));
      drawMiniRadioButton(652, 520 - scrollMargin, user::disableHumidityWarnings);
      previousDisableHumidityWarnings = user::disableHumidityWarnings;
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void drawSettingsPageFour() {
  tft.setTextColor(RA8875_BLACK, user::backgroundColor);
  if (display::showSr04HeightCalibration) {
    if (display::refreshPage)
      continueMessage(message::setWaterHeight, -1, 0, true, true, false);
  }
  else if (display::showEtapeCalibration == device::PAGE_ONE) {
    if (display::refreshPage)
      continueMessage(message::setEtapeMinReading, -1, 0, true, true, false);
  }
  else if (display::showEtapeCalibration == device::PAGE_TWO) {
    if (display::refreshPage)
      continueMessage(message::setEtapeMaxReading, -1, 0, true, true, false);
  }
  else if (display::showCo2Calibration) {
    if (display::refreshPage)
      continueMessage(message::calibrateCo2, -1, 0, true, true, false);
  }
  else if (display::showDoserCalibration) {
    if (display::calDoserPageScrollPos > 0) {
      tft.fillTriangle(95, 240, 115, 200, 115, 280, RA8875_BLUE);
    }
    if (display::calDoserPageScrollPos < user::numberOfDosers - 4) {
      tft.fillTriangle(779, 200, 799, 240, 779, 280, RA8875_BLUE);
    }
    static uint8_t prevDoserIsPriming = 0;
    int16_t startPosition = display::calDoserPageScrollPos * 178;
    if (display::calDoserPageScrollPos == 0) { // DOSER 1
      static int16_t doserOnSpeedPosition, previousDoserOneSpeed;
      drawDoser(100 - startPosition, 90, 1, user::doserOneSpeed, previousDoserOneSpeed, doserOnSpeedPosition, 1, -45);
      if (display::refreshPage || bitRead(device::doserIsPriming, 0) != bitRead(prevDoserIsPriming, 0)) {
        bitRead(device::doserIsPriming, 0) ? stopButton(136 - startPosition, 308) : primeButton(136 - startPosition, 308);
        bitWrite(prevDoserIsPriming, 0, bitRead(device::doserIsPriming, 0));
      }
    }
    if (display::calDoserPageScrollPos <= 1) { // DOSER 2
      static int16_t doserTwoSpeedPosition, previousDoserTwoSpeed;
      drawDoser(278 - startPosition, 90, 2, user::doserTwoSpeed, previousDoserTwoSpeed, doserTwoSpeedPosition, 1, -45);
      if (display::refreshPage || bitRead(device::doserIsPriming, 1) != bitRead(prevDoserIsPriming, 1)) {
        bitRead(device::doserIsPriming, 1) ? stopButton(314 - startPosition, 308) : primeButton(314 - startPosition, 308);
        bitWrite(prevDoserIsPriming, 1, bitRead(device::doserIsPriming, 1));
      }
    }
    if (display::calDoserPageScrollPos <= 2) { // DOSER 3
      static int16_t doserThreeSpeedPosition, previousDoserThreeSpeed;
      drawDoser(456 - startPosition, 90, 3, user::doserThreeSpeed, previousDoserThreeSpeed, doserThreeSpeedPosition, 1, -45);
      if (display::refreshPage || bitRead(device::doserIsPriming, 2) != bitRead(prevDoserIsPriming, 2)) {
        bitRead(device::doserIsPriming, 2) ? stopButton(492 - startPosition, 308) : primeButton(492 - startPosition, 308);
        bitWrite(prevDoserIsPriming, 2, bitRead(device::doserIsPriming, 2));
      }
    }
    if (display::calDoserPageScrollPos <= 2) { // DOSER 4
      static int16_t doserFourSpeedPosition, previousDoserFourSpeed;
      drawDoser(634 - startPosition, 90, 4, user::doserFourSpeed, previousDoserFourSpeed, doserFourSpeedPosition, 1, -45);
      if (display::refreshPage || bitRead(device::doserIsPriming, 3) != bitRead(prevDoserIsPriming, 3)) {   
        bitRead(device::doserIsPriming, 3) ? stopButton(670 - startPosition, 308) : primeButton(670 - startPosition, 308);
        bitWrite(prevDoserIsPriming, 3, bitRead(device::doserIsPriming, 3));
      }
    }
    if (display::calDoserPageScrollPos >= 1) { // DOSER 5
      static int16_t doserFiveSpeedPosition, previousDoserFiveSpeed;
      drawDoser(812 - startPosition, 90, 5, user::doserFiveSpeed, previousDoserFiveSpeed, doserFiveSpeedPosition, 1, -45);
      if (display::refreshPage || bitRead(device::doserIsPriming, 4) != bitRead(prevDoserIsPriming, 4)) {   
        bitRead(device::doserIsPriming, 4) ? stopButton(848 - startPosition, 308) : primeButton(848 - startPosition, 308);
        bitWrite(prevDoserIsPriming, 4, bitRead(device::doserIsPriming, 4));
      }
    }
    if (display::calDoserPageScrollPos == 2) { // DOSER 6
      static int16_t doserSixSpeedPosition, previousDoserSixSpeed;
      drawDoser(990 - startPosition, 90, 6, user::doserSixSpeed, previousDoserSixSpeed, doserSixSpeedPosition, 1, -45);
      if (display::refreshPage || bitRead(device::doserIsPriming, 5) != bitRead(prevDoserIsPriming, 5)) {    
        bitRead(device::doserIsPriming, 5) ? stopButton(1026 - startPosition, 443) : primeButton(1026 - startPosition, 443);
        bitWrite(prevDoserIsPriming, 5, bitRead(device::doserIsPriming, 5));
      }
    }
    // buttons
    if (display::refreshPage) {
      drawFourUpDownButtons(0);
      exitButton(384, 415);
      infoButton(770, 120);
    }
  }
  else if (display::showTdsCalibration) {
    if (display::refreshPage) {
      if (display::showCalErrorMessage)
        continueMessage(message::calibrateError, -1, 0, true, false, false);
      else if (display::calTdsPageScrollPos == 0) 
        continueMessage(message::calibrateTds, sensor::tdsCalSolutionPart1, 0, true, true, false);
      else
        continueMessage(message::calibrateTds, sensor::tdsCalSolutionPart2, 0, true, true, false);
    }
  }
  else if (display::showPhCalibration) {
    if (display::refreshPage) {
      if (display::showCalErrorMessage)
        continueMessage(message::calibrateError, -1, 0, true, false, false);
      else if (display::calPhPageScrollPos == 0) 
        continueMessage(message::calibratePh, sensor::phCalSolutionPart1, 1, true, true, false);
      else
        continueMessage(message::calibratePh, sensor::phCalSolutionPart2, 1, true, true, false);
    }
  }
  else if (display::showTouchCalibration) {
    const uint8_t maxTouches = 200;
    uint8_t touchCounts = 0;
    uint8_t prevTouchCalibrationPos = 0;
    uint16_t _calMinX = tft.getCalMinX();
    uint16_t _calMaxX = tft.getCalMaxX();
    uint16_t _calMinY = tft.getCalMinY();
    uint16_t _calMaxY = tft.getCalMaxY();
    uint16_t prevCalMinX = 0, prevCalMaxX = 0;
    uint16_t prevCalMinY = 0, prevCalMaxY = 0;

    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);

    while (display::showTouchCalibration) {
      if (display::showTouchCalibration != prevTouchCalibrationPos) { // display::refreshPage || 
        Serial.print(F("show touch int ")); Serial.println(display::showTouchCalibration); 
        if (display::showTouchCalibration == 2) {
          device::calMinX = display::touch_x;
          device::calMinY = display::touch_y;
        }
        else if (display::showTouchCalibration == 3) {
          device::calMaxX = display::touch_x;
          device::calMaxY = display::touch_y;
        }
        if (display::showTouchCalibration < 3) {
          tft.fillWindow(user::backgroundColor);
          tft.print(80, 200, F("Please stop pressing the display!"));
          delay(5000);    
          display::touch_x = 0;
          display::touch_y = 0; 
          touchCounts = 0;       
        } 
        tft.clearTouchInt();
        tft.fillWindow(user::backgroundColor);       
      }
      if (display::showTouchCalibration == 1) {
        Serial.println(F("top left"));
        tft.print(100, 200, F("Please press keep pressing"));
        tft.print(100, 250, F("the dot in the top left corner"));
        tft.fillCircle(5, 5, 5, RA8875_RED);
      }
      else if (display::showTouchCalibration == 2) {
        Serial.println(F("bottom right"));
        tft.print(100, 200, F("Please press keep pressing"));
        tft.print(100, 250, F("the dot in the bottom right corner"));
        tft.fillCircle(tft.width() - 5, tft.height() - 5, 5, RA8875_RED);
      }
      else if (display::showTouchCalibration == 3) {
        if (display::showTouchCalibration != prevTouchCalibrationPos) {
          Serial.println(F("showing touch p4"));
          tft.print(120, 180, F("Draw on the screen to fine"));
          tft.print(120, 215, F("tune the touch accuracy"));
          cancelButton(274, 366);
          continueButton(449, 366);
          tft.setTouchCalibration(device::calMinX, device::calMinY, device::calMaxX, device::calMaxY);
        } 
        if (tft.touched()) {
          tft.touchReadPixel(&display::touch_x, &display::touch_y, 10);
          tft.fillCircle(display::touch_x, display::touch_y, 2, RA8875_WHITE);
          // Cancel
          if (display::touch_x >= 270 && display::touch_x <= 420 && display::touch_y >= 360 && display::touch_y <= 400) {
            Serial.println(F("pressed touch cancel"));
            beep();
            // reset touch back to the old settings
            device::calMinX = _calMinX;
            device::calMinY = _calMinY;
            device::calMaxX = _calMaxX;
            device::calMaxY = _calMaxY;
            tft.setTouchCalibration(device::calMinX, device::calMinY, device::calMaxX, device::calMaxY);
            display::showTouchCalibration = 0;
            tft.fillWindow(user::backgroundColor);
            display::refreshPage = true;
            frame();
            OuterMenuIcons();
            drawSettingsPageFour();
            return;
          }
          // Continue
          else if (display::touch_x >= 445 && display::touch_x <= 640 && display::touch_y >= 360 && display::touch_y <= 400) {
            Serial.println(F("pressed touch continue"));
            beep();
            display::showTouchCalibration = 0;
            tft.fillWindow(user::backgroundColor);
            device::settingsAdjusted = true;
            display::refreshPage = true;
            frame();
            OuterMenuIcons();
            drawSettingsPageFour();
            return;        
          }
        }
      }     

      prevTouchCalibrationPos = display::showTouchCalibration;

      if (display::showTouchCalibration < 3) {
        while (touchCounts < maxTouches) {
          if (tft.touched()) {
            delay(10);
            uint16_t touchX = 0, touchY = 0;
            tft.readTouchADC(&touchX, &touchY);//we using 10bit adc data here
            if (display::showTouchCalibration == 1) {
              if (touchX < display::touch_x || display::touch_x == 0) 
                display::touch_x = touchX;
              if (touchY < display::touch_y || display::touch_y == 0) 
                display::touch_y = touchY;
            }
            else {
              if (touchX > display::touch_x || display::touch_x == 0) 
                display::touch_x = touchX;
              if (touchY > display::touch_y || display::touch_y == 0) 
                display::touch_y = touchY;
            }
            touchCounts++;
          }
        }
        display::showTouchCalibration++;
        Serial.print(F("cal touch x: "));  Serial.println(display::touch_x); 
        Serial.print(F("cal touch y: "));  Serial.println(display::touch_y); 
      }

      timedEvents(); 
    }
  }
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(110, 130, F("Calibration"));
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      if (user::heightSensor == user::SR04)
        tft.print(110, 170, F("Calibrate SR-04"));
      else if (user::heightSensor == user::ETAPE)
        tft.print(110, 170, F("Calibrate Etape"));
      else if (user::heightSensor == user::VL53L0X)
        tft.print(110, 170, F("Calibrate VL53L0X"));
      tft.print(110, 220, F("Calibrate TDS"));
      tft.print(110, 270, F("Calibrate Co2"));
      tft.print(110, 320, F("Calibrate PH"));
      tft.print(110, 370, F("Calibrate dosers"));
      tft.print(110, 420, F("Calibrate touch"));
      drawMiniConfirmButton(712, 170);
      drawMiniConfirmButton(712, 220);
      drawMiniConfirmButton(712, 270);
      drawMiniConfirmButton(712, 320);
      drawMiniConfirmButton(712, 370);
      drawMiniConfirmButton(712, 420);
    }
  }
}
