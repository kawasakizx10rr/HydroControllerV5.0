// Draw the given page
void drawPages() {
  if (display::showInfoDialog)
    infoMessage();
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
    lightingPage();
  else if (display::page == 11)
    fansPage();
  else if (display::page == 12)
    warningsPage();
  display::refreshPage = false;
  device::newGraphData = false;
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
      tft.print(140, 140, "TDS sensor");
    }
    else if (display::homePage == 1) {
      tft.print(160, 140, "EC sensor");
    }
    else if (display::homePage == 2) {
      tft.print(160, 140, "PH sensor");
    }
    else if (display::homePage == 3) {
      tft.print(140, 140, "Co2 sensor");
    }
    else if (display::homePage == 4) {
      tft.print(130, 140, "Water temp");
      tft.setCursor(tft.getFontX() + 6, 130);
      !user::convertToF ? tft.print("c") : tft.print("f");
    }
    else if (display::homePage == 5) {
      !user::convertToInches ? tft.setCursor(100, 140) : tft.setCursor(120, 140);
      tft.print("Water height");
      tft.setCursor(tft.getFontX() + 6, 140);
      !user::convertToInches ? tft.print("cm") : tft.print("\"");
    }
    else if (display::homePage == 6) {
      tft.print(110, 140, "Fan one speed");
    }
    else if (display::homePage == 7) {
      tft.print(110, 140, "Fan two speed");
    }
    else if (display::homePage == 8) {
      tft.print(170, 140, "Air temp");
      tft.setCursor(tft.getFontX() + 6, 130);
      !user::convertToF ? tft.print("c") : tft.print("f");
    }
    else if (display::homePage == 9) {
      tft.print(180, 140, "Humidity");
    }
  }
  // RTC clock
  static uint8_t previousRTCMinute;
  rtc.refresh();
  if (display::refreshPage || rtc.minute() != previousRTCMinute) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    static int statusBarStartX = 0, statusBarWidth = 0;
    tft.fillRect(statusBarStartX, 92, statusBarWidth, 34, user::backgroundColor);
    char timeStr[16]{0};
    strcpy(timeStr, rtc.getTimeStr());
    strcat(timeStr, (rtc.hour() <= 12 ? " AM" : " PM"));
    int timeStrOffset = tft.getStringWidth(timeStr);
    tft.print(795-timeStrOffset, 92, timeStr);
    // Wifi symbol
    tft.drawXBMP(795 - (timeStrOffset + 45), 99, 30, 26, miniWifiIcon, 97, wifi::wifiEnabled ? RA8875_BLUE : display::RA8875_DARKGREY, user::backgroundColor, 1);
    // Warning symbol
    //if (device::globalErrorState != device::NO_WARNING)
      tft.drawXBMP(795 - (timeStrOffset + 90), 95, 31, 30, miniWarningIcon, 116, device::globalErrorState == device::MAJOR_WARNING ? RA8875_RED : RA8875_YELLOW, user::backgroundColor, 1);
    statusBarStartX = 795 - (timeStrOffset + 90);
    statusBarWidth = tft.width() - (timeStrOffset + 90);
    previousRTCMinute = rtc.minute();
  }
  // draw values and graph
  //if (display::refreshPage || device::newGraphData) {
    if (display::homePage == 0) {
      drawSensorSlide(sensor::tds, user::targetMinTds, user::targetMaxTds, device::tdsArray, 0);
    }
    else if (display::homePage == 1) {
      drawSensorSlide(sensor::ec, user::targetMinEc, user::targetMaxEc, device::ecArray, 2);
    }
    else if (display::homePage == 2) {
      drawSensorSlide(sensor::ph, user::targetMinPh, user::targetMaxPh, device::phArray, 2);
    }
    else if (display::homePage == 3) {
      drawSensorSlide(sensor::co2, user::targetCo2, user::targetCo2, device::co2Array, 0);
    }
    else if (display::homePage == 4) {
      if (user::convertToF)
        drawSensorSlide(convertToF(sensor::waterTemp), user::targetMinWaterTempF, user::targetMaxWaterTempF, device::waterTemperatureArrayF, 1);
      else
        drawSensorSlide(sensor::waterTemp, user::targetMinWaterTemp, user::targetMaxWaterTemp, device::waterTemperatureArray, 1);
    }
    else if (display::homePage == 5) {
      if (user::convertToInches)
        drawSensorSlide(sensor::waterLevelInches, user::targetMinWaterHeightInches, user::targetMaxWaterHeightInches, device::waterLevelArrayInInches, 1);
      else
        drawSensorSlide(sensor::waterLevel, user::targetMinWaterHeight, user::targetMaxWaterHeight, device::waterLevelArray, 1);
    }
    else if (display::homePage == 6) {
      drawSensorSlide(device::fanOneSpeed, user::targetMinFanOneSpeed, user::targetMaxFanOneSpeed, device::fanOneSpeedArray, 0);
    }
    else if (display::homePage == 7) {
      drawSensorSlide(device::fanTwoSpeed, user::targetMinFanTwoSpeed, user::targetMaxFanTwoSpeed, device::fanTwoSpeedArray, 0);
    }
    else if (display::homePage == 8) {
      if (user::convertToF)
        drawSensorSlide(convertToF(sensor::airTemp), user::targetMinAirTempF, user::targetMaxAirTempF, device::airTemperatureArrayF, 1);
      else
        drawSensorSlide(sensor::airTemp, user::targetMinAirTemp, user::targetMaxAirTemp, device::airTemperatureArray, 1);
    }
    else if (display::homePage == 9) {
      drawSensorSlide(sensor::humidity, user::targetMinHumidity, user::targetMaxHumidity, device::humidityArray, 1);
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
    
    int numOfGraphs = 0;
    float* graphArrays[10] {};
    unsigned int graphColors[10] {};

    if (display::showTdsGraph) {
      graphArrays[numOfGraphs] = device::tdsArray;
      graphColors[numOfGraphs++] = display::RA8875_PURPLE;
    }
    if (display::showPhGraph) {
      graphArrays[numOfGraphs] = device::phArray;
      graphColors[numOfGraphs++] = display::RA8875_LIGHT_GREEN;
    }
    if (display::showEcGraph) {
      graphArrays[numOfGraphs] = device::ecArray;
      graphColors[numOfGraphs++] = RA8875_YELLOW;
    }
    if (display::showCo2Graph) {
      graphArrays[numOfGraphs] = device::co2Array;
      graphColors[numOfGraphs++] = RA8875_MAGENTA;
    }
    if (display::showWaterTempGraph) {
      graphArrays[numOfGraphs] = (user::convertToF ? device::waterTemperatureArrayF : device::waterTemperatureArray);
      graphColors[numOfGraphs++] = display::RA8875_ORANGE;
    }
    if (display::showWaterLvlGraph) {
      graphArrays[numOfGraphs] = (user::convertToInches ? device::waterLevelArrayInInches : device::waterLevelArray);
      graphColors[numOfGraphs++] = RA8875_BLUE;
    }
    if (display::showFanOneGraph) {
      graphArrays[numOfGraphs] = device::fanOneSpeedArray;
      graphColors[numOfGraphs++] = RA8875_WHITE;
    }
    if (display::showFanTwoGraph) {
      graphArrays[numOfGraphs] = device::fanTwoSpeedArray;
      graphColors[numOfGraphs++] = RA8875_BLACK;
    }
    if (display::showAirTempGraph) {
      graphArrays[numOfGraphs] = (user::convertToF ? device::airTemperatureArrayF : device::airTemperatureArray);
      graphColors[numOfGraphs++] = RA8875_RED;
    }
    if (display::showHumidityGraph) {
      graphArrays[numOfGraphs] = device::humidityArray;
      graphColors[numOfGraphs++] = display::RA8875_SEABLUE;
    }
    if (numOfGraphs > 0)
      drawGraph(95, 100, 350, tft.width() - 155, 10, graphArrays, numOfGraphs, device::graphArrayPos, 0, graphColors);
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void maxminsPage() {
  unsigned int color = 0;
  unsigned int color2 = 0;
  if (display::refreshPage) {
    drawSlideIcons(110, 112, display::maxMinsPage, 10);
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(176, 170, "Minimum");
    tft.print(530, 170, "Maximum");
    infoButton(770, 120);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
  }
  if (display::maxMinsPage == 0) {
    if (display::refreshPage)
      tft.print(330, 126, "PPM history");
    drawTwoValues(285, device::minTds, RA8875_BLACK, 0, 650, device::maxTds, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::maxMinsPage == 1) {
    if (display::refreshPage)
      tft.print(340, 126, "EC history");
    drawTwoValues(285, device::minEc, RA8875_BLACK, 2, 650, device::maxEc, RA8875_BLACK, 2, NULL, 00);
  }
  else if (display::maxMinsPage == 2) {
    if (display::refreshPage)
      tft.print(340, 126, "PH history");
    drawTwoValues(285, device::minPh, RA8875_BLACK, 2, 650, device::maxPh, RA8875_BLACK, 2, NULL, 00);
  }
  else if (display::maxMinsPage == 3) {
    if (display::refreshPage)
      tft.print(330, 126, "Co2 history");
    drawTwoValues(285, device::minCo2, RA8875_BLACK, 0, 650, device::maxCo2, RA8875_BLACK, 0, NULL, 00);
  }
  else if (display::maxMinsPage == 4) {
    if (display::refreshPage)
      tft.print(250, 126, "Water temp history");
    if (user::convertToF)
      drawTwoValues(285, convertToF(device::minWaterTemp), RA8875_BLACK, 1, 650, convertToF(device::maxWaterTemp), RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, device::minWaterTemp, RA8875_BLACK, 1, 650, device::maxWaterTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::maxMinsPage == 5) {
    if (display::refreshPage)
      tft.print(234, 126, "Water height history");
    if (user::convertToInches) {
      drawTwoValues(285, convertToInch(device::minWaterLevel), RA8875_BLACK, 0, 650, convertToInch(device::maxWaterLevel), RA8875_BLACK, 0, "\"", 0);
    }
    else {
      drawTwoValues(285, device::minWaterLevel, RA8875_BLACK, 0, 650, device::maxWaterLevel, RA8875_BLACK, 0, "CM", 50);
    }
  }
  else if (display::maxMinsPage == 6) {
    if (display::refreshPage)
      tft.print(214, 126, "Fan one speed history");
    drawTwoValues(285, device::minFanOneSpeed, RA8875_BLACK, 0, 650, device::maxFanOneSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::maxMinsPage == 7) {
    if (display::refreshPage)
      tft.print(214, 126, "Fan two speed history");
    drawTwoValues(285, device::minFanTwoSpeed, RA8875_BLACK, 0, 650, device::maxFanTwoSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::maxMinsPage == 8) {
    if (display::refreshPage)
      tft.print(286, 126, "Air temp history");
    if (user::convertToF)
      drawTwoValues(285, convertToF(device::minAirTemp), RA8875_BLACK, 1, 650, convertToF(device::maxAirTemp), RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, device::minAirTemp, RA8875_BLACK, 1, 650, device::maxAirTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::maxMinsPage == 9) {
    if (display::refreshPage)
      tft.print(282, 126, "Humidity history");
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
    tft.print(200, 100, "Save and load profiles");
    // Input box and buttons for profile 1
    inputBox(130, 160, user::profileOneName);
    saveButton(493, 160);
    loadButton(631, 160);
    // Input box and buttons for profile 2
    inputBox(130, 222, user::profileTwoName);
    saveButton(493, 222);
    loadButton(631, 222);
    // Input box and buttons for profile 3
    inputBox(130, 284, user::profileThreeName);
    saveButton(493, 284);
    loadButton(631, 284);
    // Input box and buttons for profile 4
    inputBox(130, 346, user::profileFourName);
    saveButton(493, 346);
    loadButton(631, 346);
    // Input box and buttons for profile 5
    inputBox(130, 408, user::profileFiveName);
    saveButton(493, 408);
    loadButton(631, 408);
    const int position = 160 + (device::userProfile * 62);
    tft.drawRect(125, position - 5, 350, 51, RA8875_BLUE);
    tft.drawRect(126, position - 4, 348, 49, RA8875_BLUE);
    tft.drawRect(127, position - 3, 346, 47, RA8875_BLUE);
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
    tft.print(300, 116, "Target PPM");
    tft.print(176, 170, "Minimum");
    tft.print(530, 170, "Maximum");
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
    tft.print(326, 116, "Target EC");
    tft.print(176, 170, "Minimum");
    tft.print(530, 170, "Maximum");
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
    tft.print(326, 116, "Target PH");
    tft.print(176, 170, "Minimum");
    tft.print(530, 170, "Maximum");
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
      tft.print(330, 116, "Target Co2");
      tft.print(156, 166, "Target PPM");
      tft.print(530, 166, "Tolerance");
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
      tft.print(330, 116, "Dimensions");
      tft.print(160, 166, "Length");
      tft.print(380, 166, "Width");
      tft.print(590, 166, "Height");
      drawThreeUpDownButtons();
      infoButton(770, 120);
    }
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    // ROOM LENGTH
    static int roomLengthStartPos = 0, roomLengthEndPos = 0;
    static float previousRoomLength;
    float roomLength = user::convertToInches ? user::roomLengthInches : user::roomLengthCm;
    if (display::refreshPage || hasChanged(roomLength, previousRoomLength, 0.01)) {
      tft.fillRect(roomLengthStartPos, 236, roomLengthEndPos - roomLengthStartPos, 50, user::backgroundColor);
      if (user::convertToInches) {
        roomLengthStartPos = 225 - ((tft.getStringWidth(roomLength, 1) / 2) + 8);
        tft.print(roomLengthStartPos, 240, roomLength, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 230, "\"");
      }
      else {
        roomLengthStartPos = 225 - ((tft.getStringWidth(roomLength, 0) / 2) + 23);
        tft.print(roomLengthStartPos, 240, roomLength, 0);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 250, "cm");
      }
      roomLengthEndPos = tft.getFontX();
      previousRoomLength = roomLength;
    }
    // ROOM WIDTH
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int roomWidthStartPos = 0, roomWidthEndPos = 0;
    static float previousRoomWidth;
    float roomWidth = user::convertToInches ? user::roomWidthInches : user::roomWidthCm;
    if (display::refreshPage || hasChanged(roomWidth, previousRoomWidth, 0.01)) {
      tft.fillRect(roomWidthStartPos, 236, roomWidthEndPos - roomWidthStartPos, 50, user::backgroundColor);
      if (user::convertToInches) {
        roomWidthStartPos = 430 - ((tft.getStringWidth(roomWidth, 1) / 2) + 8);
        tft.print(roomWidthStartPos, 240, roomWidth, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 230, "\"");
      }
      else {
        roomWidthStartPos = 430 - ((tft.getStringWidth(roomWidth, 0) / 2) + 23);
        tft.print(roomWidthStartPos, 240, roomWidth, 0);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 250, "cm");
      }
      roomWidthEndPos = tft.getFontX();
      previousRoomWidth = roomWidth;
    }
    // ROOM HEIGHT
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int roomHeightStartPos = 0, roomHeightEndPos = 0;
    static float previousRoomHeight;
    float roomHeight = user::convertToInches ? user::roomHeightInches : user::roomHeightCm;
    if (display::refreshPage || hasChanged(roomHeight, previousRoomHeight, 0.01)) {
      tft.fillRect(roomHeightStartPos, 236, roomHeightEndPos - roomHeightStartPos, 50, user::backgroundColor);
      if (user::convertToInches) {
        roomHeightStartPos = 640 - ((tft.getStringWidth(roomHeight, 1) / 2) + 8);
        tft.print(roomHeightStartPos, 240, roomHeight, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 230, "\"");
      }
      else {
        roomHeightStartPos = 640 - ((tft.getStringWidth(roomHeight, 0) / 2) + 23);
        tft.print(roomHeightStartPos, 240, roomHeight, 0);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.print(tft.getFontX() + 6, 250, "cm");
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
      tft.print(330, 116, "Co2 settings");
      tft.print(130, 166, "Flow rate");
      tft.print(380, 166, "Time");
      tft.print(580, 166, "Fans off");
      drawThreeUpDownButtons();
      infoButton(770, 120);
    }
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    // CO2 FLOW RATE LTR/MIN
    static int co2FlowrateStartPos = 0, co2FlowrateEndPos = 0;
    static float previousCo2Flowrate;
    float co2Flowrate = user::convertToInches ? user::co2FlowrateFeet3 : user::co2FlowrateLtrs;
    if (display::refreshPage || hasChanged(co2Flowrate, previousCo2Flowrate, 0.01)) {
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
        tft.print("Ft3");
      }
      else {
        tft.print(co2Flowrate, 1);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.setCursor(tft.getFontX() + 4, 250);
        co2Flowrate == 1 ? tft.print("ltr") : tft.print("ltrs");
      }
      co2FlowrateEndPos = tft.getFontX();
      previousCo2Flowrate = co2Flowrate;
    }
    // CO2 CHECK TIME
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int co2CheckTimePosition;
    static int previousCo2CheckTimeMinute;
    if (display::refreshPage || user::co2CheckTimeMinute != previousCo2CheckTimeMinute) {
      tft.fillRect(338, 238, co2CheckTimePosition - 336, 50, user::backgroundColor);
      tft.setCursor(350, 240);
      if (user::co2CheckTimeHour < 10)
        tft.print("0");
      tft.print(user::co2CheckTimeHour);
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.print(":");
      tft.setFont(&HallfeticaLargenum_42px_Regular);
      tft.setFontScale(1);
      if (user::co2CheckTimeMinute < 10)
        tft.print("0");
      tft.print(user::co2CheckTimeMinute);
      co2CheckTimePosition = tft.getFontX();
      previousCo2CheckTimeMinute = user::co2CheckTimeMinute;
    }
    // DISABLE FANS TIMER
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    static int disableFansTimerStartPos = 0, disableFansTimerEndPos = 0;
    static int previousDisableFansTimer;

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
        tft.print("Disabled");
      }
      else {
        tft.print(user::disableFansTimer);
        tft.setFont(&akashi_36px_Regular);
        tft.setFontScale(1);
        tft.setCursor(tft.getFontX() + 6, 250);
        tft.print("mins");
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
      tft.print(330, 116, "Co2 settings");
      tft.print(142, 166, "Manual");
      tft.print(130, 196, "duration");
      tft.print(380, 166, "Gas");
      tft.print(358, 196, "duration");
      tft.print(592, 166, "Disable");
      tft.print(624, 196, "Co2");
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
    static int manualCo2GasDurationStartPos = 0, manualCo2GasDurationEndPos = 0;
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
      user::manualCo2GasDuration == 1 ? tft.print("min") : tft.print("mins");
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
      tft.print(260, 116, "Target water level");
      tft.print(176, 170, "Minimum");
      tft.print(530, 170, "Maximum");
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
      tft.print(260, 116, "Target water temp");
      tft.print(176, 170, "Minimum");
      tft.print(530, 170, "Maximum");
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
      tft.print(300, 116, "Tank dimensions");
      tft.print(180, 170, "Length");
      tft.print(540, 170, "Width");
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
      tft.print(140, 140, "Auto drain dates");
      tft.print(510, 200, "Disable drain");
      tft.print(510, 250, "and refill");
      tft.print(100, 420, "Time");
      drawMiniButtonIncrements(385, 440);
      infoButton(770, 120);
    }
    if (display::refreshPage || display::refreshCalander) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, RA8875_WHITE);
      int calanderX = 110, calanderY = 205;
      for (uint8_t i = 1; i < 32; i++) {
        if (i < 10)
          tft.setCursor(calanderX + 14, calanderY);
        else if (i < 19)
          tft.setCursor(calanderX + 6, calanderY);
        else if (i == 21 || i == 31)
          tft.setCursor(calanderX + 6, calanderY);
        else
          tft.setCursor(calanderX + 2, calanderY);
        if (user::autoFillDays[i - 1])
          tft.fillRect(calanderX - 1, calanderY + 1, 52, 38, RA8875_BLUE);
        else
          tft.fillRect(calanderX - 1, calanderY + 1, 52, 38, RA8875_WHITE);
        tft.drawRect(calanderX - 2, calanderY, 54, 40, RA8875_BLACK);
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
    static int autoFillTimePosition;
    if (display::refreshPage || user::autoFillMinute != previousAutoFillMinute) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.fillRect(204, 418, autoFillTimePosition - 204, 42, user::backgroundColor);
      tft.setCursor(206, 420);
      if (user::autoFillHour < 10)
        tft.print("0");
      tft.print(user::autoFillHour);
      tft.print(":");
      if (user::autoFillMinute < 10)
        tft.print("0");
      tft.print(user::autoFillMinute);
      previousAutoFillMinute = user::autoFillMinute;
      autoFillTimePosition = tft.getFontX();
    }
    static bool previousdisableDrainAndRefill;
    if (display::refreshPage || user::disableDrainAndRefill != previousdisableDrainAndRefill) {
      drawRadioButton(565, 320, user::disableDrainAndRefill);
      previousdisableDrainAndRefill = user::disableDrainAndRefill;
    }
  }
  else if (display::waterPageScrollPos == 4) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(276, 110, "Refill doser mls");
      drawFourUpDownButtons(60);
      infoButton(770, 120);
    }
    if (display::refillDoserPageScrollPos > 0) {
      tft.fillTriangle(95, 240, 115, 200, 115, 280, RA8875_BLUE);
    }
    if (display::refillDoserPageScrollPos < user::numberOfDosers - 4) {
      tft.fillTriangle(779, 200, 799, 240, 779, 280, RA8875_BLUE);
    }
    int startPosition = display::refillDoserPageScrollPos * 178;
    // The below method may look odd but it is much faster than a for loop "after testing"
    if (display::refillDoserPageScrollPos == 0) { // DOSER 1
      static uint8_t previousDoserOneMode;
      static int doserOneMlsPosition, previousDoserOneMls;
      drawDoser(100 - startPosition, 150, 1, user::refillDoserOneMills, previousDoserOneMls, doserOneMlsPosition, 0, 0);
      drawEcPhButton(149 - startPosition, 300, user::doserOneMode, previousDoserOneMode, false);
    }
    if (display::refillDoserPageScrollPos <= 1) { // DOSER 2
      static uint8_t previousDoserTwoMode;
      static int doserTwoMlsPosition, previousDoserTwoMls;
      drawDoser(278 - startPosition, 150, 2, user::refillDoserTwoMills, previousDoserTwoMls, doserTwoMlsPosition, 0, 0);
      drawEcPhButton(327 - startPosition, 300, user::doserTwoMode, previousDoserTwoMode, false);
    }
    if (display::refillDoserPageScrollPos <= 2) { // DOSER 3
      static uint8_t previousDoserThreeMode;
      static int doserThreeMlsPosition, previousDoserThreeMls;
      drawDoser(456 - startPosition, 150, 3, user::refillDoserThreeMills, previousDoserThreeMls, doserThreeMlsPosition, 0, 0);
      drawEcPhButton(505 - startPosition, 300, user::doserThreeMode, previousDoserThreeMode, false);
    }
    if (display::refillDoserPageScrollPos <= 2) { // DOSER 4
      static uint8_t previousDoserFourMode;
      static int doserFourMlsPosition, previousDoserFourMls;
      drawDoser(634 - startPosition, 150, 4, user::refillDoserFourMills, previousDoserFourMls, doserFourMlsPosition, 0, 0);
      drawEcPhButton(683 - startPosition, 300, user::doserFourMode, previousDoserFourMode, false);
    }
    if (display::refillDoserPageScrollPos >= 1) { // DOSER 5
      static uint8_t previousDoserFiveMode;
      static int doserFiveMlsPosition, previousDoserFiveMls;
      drawDoser(812 - startPosition, 150, 5, user::refillDoserFiveMills, previousDoserFiveMls, doserFiveMlsPosition, 0, 0);
      drawEcPhButton(861 - startPosition, 300, user::doserFiveMode, previousDoserFiveMode, false);
    }
    if (display::refillDoserPageScrollPos == 2) { // DOSER 6
      static uint8_t previousDoserSixMode;
      static int doserSixMlsPosition, previousDoserSixMls;
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
  static bool prevDoserIsPriming[6]{};
  int startPosition = display::doserPageScrollPos * 178;
  if (display::doserPageScrollPos == 0) { // DOSER 1
    static uint8_t previousDoserOneMode;
    static int doserOneMlsPosition, previousDoserOneMls;
    drawDoser(100 - startPosition, 90, 1, user::doserOneMills, previousDoserOneMls, doserOneMlsPosition, 0, 0);
    drawEcPhButton(149 - startPosition, 255, user::doserOneMode, previousDoserOneMode, true);
    if (display::refreshPage || device::doserIsPriming[0] != prevDoserIsPriming[0]) {
      device::doserIsPriming[0] ? stopButton(136 - startPosition, 443) : primeButton(136 - startPosition, 443);
      prevDoserIsPriming[0] = device::doserIsPriming[0];
    }
  }
  if (display::doserPageScrollPos <= 1) { // DOSER 2
    static uint8_t previousDoserTwoMode;
    static int doserTwoMlsPosition, previousDoserTwoMls;
    drawDoser(278 - startPosition, 90, 2, user::doserTwoMills, previousDoserTwoMls, doserTwoMlsPosition, 0, 0);
    drawEcPhButton(327 - startPosition, 255, user::doserTwoMode, previousDoserTwoMode, true);
    if (display::refreshPage || device::doserIsPriming[1] != prevDoserIsPriming[1]) {
      device::doserIsPriming[1] ? stopButton(314 - startPosition, 443) : primeButton(314 - startPosition, 443);
      prevDoserIsPriming[1] = device::doserIsPriming[1];
    }
  }
  if (display::doserPageScrollPos <= 2) { // DOSER 3
    static uint8_t previousDoserThreeMode;
    static int doserThreeMlsPosition, previousDoserThreeMls;
    drawDoser(456 - startPosition, 90, 3, user::doserThreeMills, previousDoserThreeMls, doserThreeMlsPosition, 0, 0);
    drawEcPhButton(505 - startPosition, 255, user::doserThreeMode, previousDoserThreeMode, true);
    if (display::refreshPage || device::doserIsPriming[2] != prevDoserIsPriming[2]) {
      device::doserIsPriming[2] ? stopButton(492 - startPosition, 443) : primeButton(492 - startPosition, 443);
      prevDoserIsPriming[2] = device::doserIsPriming[2];
    }
  }
  if (display::doserPageScrollPos <= 2) { // DOSER 4
    static uint8_t previousDoserFourMode;
    static int doserFourMlsPosition, previousDoserFourMls;
    drawDoser(634 - startPosition, 90, 4, user::doserFourMills, previousDoserFourMls, doserFourMlsPosition, 0, 0);
    drawEcPhButton(683 - startPosition, 255, user::doserFourMode, previousDoserFourMode, true);
    if (display::refreshPage || device::doserIsPriming[3] != prevDoserIsPriming[3]) {
      device::doserIsPriming[3] ? stopButton(670 - startPosition, 443) : primeButton(670 - startPosition, 443);
      prevDoserIsPriming[3] = device::doserIsPriming[3];
    }
  }
  if (display::doserPageScrollPos >= 1) { // DOSER 5
    static uint8_t previousDoserFiveMode;
    static int doserFiveMlsPosition, previousDoserFiveMls;
    drawDoser(812 - startPosition, 90, 5, user::doserFiveMills, previousDoserFiveMls, doserFiveMlsPosition, 0, 0);
    drawEcPhButton(861 - startPosition, 255, user::doserFiveMode, previousDoserFiveMode, true);
    if (display::refreshPage || device::doserIsPriming[4] != prevDoserIsPriming[4]) {
      device::doserIsPriming[4] ? stopButton(848 - startPosition, 443) : primeButton(848 - startPosition, 443);
      prevDoserIsPriming[4] = device::doserIsPriming[4];
    }
  }
  if (display::doserPageScrollPos >= 2) { // DOSER 6
    static uint8_t previousDoserSixMode;
    static int doserSixMlsPosition, previousDoserSixMls;
    drawDoser(990 - startPosition, 90, 6, user::doserSixMills, previousDoserSixMls, doserSixMlsPosition, 0, 0);
    drawEcPhButton(1039 - startPosition, 255, user::doserSixMode, previousDoserSixMode, true);
    if (display::refreshPage || device::doserIsPriming[5] != prevDoserIsPriming[5]) {
      device::doserIsPriming[5] ? stopButton(1026 - startPosition, 443) : primeButton(1026 - startPosition, 443);
      prevDoserIsPriming[5] = device::doserIsPriming[5];
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void lightingPage() {
  if (display::refreshPage) {
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(300, 116, "Light settings");
    tft.print(110, 166, "Light on");
    tft.print(335, 166, "Light off");
    tft.print(550, 166, "On/Auto/Off");
    drawUpDownButtons(164, 366, 243, 366, RA8875_BLUE);
    drawUpDownButtons(374, 366, 457, 366, RA8875_BLUE);
    infoButton(770, 120);
  }
  // LIGHT ON TIME
  static uint8_t previousOnTime;
  if (display::refreshPage || previousOnTime != user::lightOnTimeHour + user::lightOnTimeMin) {
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.setFontScale(1);
    tft.fillRect(105, 238, 200, 50, user::backgroundColor);
    tft.setCursor(110, 240);
    if (user::lightOnTimeHour < 10)
      tft.print(0);
    tft.print(user::lightOnTimeHour);
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.print(190, 240, ":");
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    tft.setCursor(210, 240);
    if (user::lightOnTimeMin < 10)
      tft.print(0);
    tft.print(user::lightOnTimeMin);
    previousOnTime = user::lightOnTimeHour + user::lightOnTimeMin;
  }
  // LIGHT OFF TIME
  static uint8_t previousOffTime;
  if (display::refreshPage || previousOffTime != user::lightOffTimeHour + user::lightOffTimeMin) {
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    tft.fillRect(325, 238, 200, 50, user::backgroundColor);
    tft.setCursor(330, 240);
    if (user::lightOffTimeHour < 10)
      tft.print(0);
    tft.print(user::lightOffTimeHour);
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.print(410, 240, ":");
    tft.setFont(&HallfeticaLargenum_42px_Regular);
    tft.setFontScale(1);
    tft.setCursor(430, 240);
    if (user::lightOffTimeMin < 10)
      tft.print(0);
    tft.print(user::lightOffTimeMin);
    previousOffTime = user::lightOffTimeHour + user::lightOffTimeMin;
  }
  // Light mode 3-state radio button
  static uint8_t previousLightMode;
  if (display::refreshPage || user::lightMode != previousLightMode) {
    tft.fillRoundRect(568, 238, 184, 44, 5, RA8875_BLACK);
    if (user::lightMode == 0) { // auto
      tft.fillRoundRect(570, 240, 60, 40, 5, display::RA8875_DARKGREY);
      tft.fillRoundRect(630, 240, 60, 40, 5, RA8875_GREEN);
      tft.fillRoundRect(690, 240, 60, 40, 5, display::RA8875_DARKGREY);
    }
    else if (user::lightMode == 1) { // on
      tft.fillRoundRect(570, 240, 60, 40, 5, RA8875_GREEN);
      tft.fillRoundRect(630, 240, 60, 40, 5, display::RA8875_DARKGREY);
      tft.fillRoundRect(690, 240, 60, 40, 5, display::RA8875_DARKGREY);
      tft.drawLine(690, 240, 690, 280, RA8875_BLACK);
    }
    else { // off
      tft.fillRoundRect(570, 240, 60, 40, 5, display::RA8875_DARKGREY);
      tft.fillRoundRect(630, 240, 60, 40, 5, display::RA8875_DARKGREY);
      tft.fillRoundRect(690, 240, 60, 40, 5, RA8875_RED);
      tft.drawLine(630, 240, 630, 280, RA8875_BLACK);
    }
    previousLightMode = user::lightMode;
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void fansPage() {
  if (display::refreshPage)
    drawSlideIcons(110, 112, display::fansPage, 5);
  if (display::fansPage == 0) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(300, 116, "Fan one target");
      tft.print(180, 166, "Min speed");
      tft.print(506, 166, "Max speed");
      drawUpDownButtons(230, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(570, 400, 650, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetMinFanOneSpeed, RA8875_BLACK, 0, 620, user::targetMaxFanOneSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::fansPage == 1) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(300, 116, "Fan two target");
      tft.print(180, 166, "Min speed");
      tft.print(506, 166, "Max speed");
      drawUpDownButtons(230, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(570, 400, 650, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetMinFanTwoSpeed, RA8875_BLACK, 0, 620, user::targetMaxFanTwoSpeed, RA8875_BLACK, 0, "%", 50);
  }
  else if (display::fansPage == 2) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(296, 116, "Target air temp");
      tft.print(176, 170, "Minimum");
      tft.print(530, 170, "Maximum");
      drawUpDownButtons(230, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(585, 400, 665, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    if (user::convertToF)
      drawTwoValues(285, user::targetMinAirTempF, RA8875_BLACK, 1, 650, user::targetMaxAirTempF, RA8875_BLACK, 1, "F", 0);
    else
      drawTwoValues(285, user::targetMinAirTemp, RA8875_BLACK, 1, 650, user::targetMaxAirTemp, RA8875_BLACK, 1, "C", 0);
  }
  else if (display::fansPage == 3) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(290, 116, "Target humidity");
      tft.print(176, 170, "Minimum");
      tft.print(530, 170, "Maximum");
      drawUpDownButtons(230, 400, 310, 400, RA8875_BLUE);
      drawUpDownButtons(585, 400, 665, 400, RA8875_BLUE);
      infoButton(770, 120);
    }
    drawTwoValues(285, user::targetMinHumidity, RA8875_BLACK, 1, 650, user::targetMaxHumidity, RA8875_BLACK, 1, "%", 50);
  }
  else if (display::fansPage == 4) {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(324, 126, "Fan settings");
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(110, 180, "Control temperature");
      tft.print(110, 230, "Control humidity");
      tft.print(110, 280, "Fan one fixed speed");
      tft.print(110, 330, "Fan two fixed speed");
      infoButton(770, 120);
    }
    // FAN MODES
    static bool previousFansControlTemperature;
    if (display::refreshPage || user::fansControlTemperature != previousFansControlTemperature) {
      drawRadioButton(640, 180, user::fansControlTemperature);
      previousFansControlTemperature = user::fansControlTemperature;
    }
    static bool previousFansControlHumidity;
    if (display::refreshPage || user::fansControlHumidity != previousFansControlHumidity) {
      drawRadioButton(640, 230, user::fansControlHumidity);
      previousFansControlHumidity = user::fansControlHumidity;
    }
    static bool previousFanOneFixedSpeed;
    if (display::refreshPage || user::fanOneFixedSpeed != previousFanOneFixedSpeed) {
      drawRadioButton(640, 280, user::fanOneFixedSpeed);
      previousFanOneFixedSpeed = user::fanOneFixedSpeed;
    }
    static bool previousFanTwoFixedSpeed;
    if (display::refreshPage || user::fanTwoFixedSpeed != previousFanTwoFixedSpeed) {
      drawRadioButton(640, 330, user::fanTwoFixedSpeed);
      previousFanTwoFixedSpeed = user::fanTwoFixedSpeed;
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void warningsPage() {
  unsigned int color;
  if (display::refreshPage) {
    drawSlideIcons(110, 112, display::warningsPage, 8);
    tft.setFont(&akashi_36px_Regular);
    tft.setFontScale(1);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
    tft.print(116, 170, "Current reading");
    tft.print(470, 170, "Error margin");
    drawUpDownButtons(554, 400, 664, 400, RA8875_BLUE);
    infoButton(770, 120);
    tft.setTextColor(RA8875_BLACK, user::backgroundColor);
  }
  if (display::warningsPage == 0) {
    if (display::refreshPage)
      tft.print(310, 126, "PPM warning");
    color = setWarningColor(sensor::tds, user::targetMinTds, user::targetMaxTds, user::tdsErrorMargin);
    drawTwoValues(285, sensor::tds, color, 0, 620, user::tdsErrorMargin, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::warningsPage == 1) {
    if (display::refreshPage)
      tft.print(324, 126, "EC warning");
    color = setWarningColor(sensor::ec, user::targetMinEc, user::targetMaxEc, user::ecErrorMargin);
    drawTwoValues(285, sensor::ec, color, 2, 620, user::ecErrorMargin, RA8875_BLACK, 2, NULL, 0);
  }
  else if (display::warningsPage == 2) {
    if (display::refreshPage)
      tft.print(324, 126, "PH warning");
    color = setWarningColor(sensor::ph, user::targetMinPh, user::targetMaxPh, user::phErrorMargin);
    drawTwoValues(285, sensor::ph, color, 2, 620, user::phErrorMargin, RA8875_BLACK, 2, NULL, 0);
  }
  else if (display::warningsPage == 3) {
    if (display::refreshPage)
      tft.print(310, 126, "Co2 warning");
    color = setWarningColor(sensor::co2, user::targetCo2, user::targetCo2, user::co2ErrorMargin);
    drawTwoValues(285, sensor::co2, color, 0, 620, user::co2ErrorMargin, RA8875_BLACK, 0, NULL, 0);
  }
  else if (display::warningsPage == 4) {
    if (display::refreshPage)
      tft.print(246, 126, "Water temp warning");
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
      tft.print(224, 126, "Water height warning");
    if (user::convertToInches) {
      color = setWarningColor(sensor::waterLevelInches, user::targetMinWaterHeightInches, user::targetMaxWaterHeightInches, user::waterHeightErrorMarginInches);
      drawTwoValues(285, sensor::waterLevelInches, color, 1, 620, user::waterHeightErrorMarginInches, RA8875_BLACK, 1, "\"", 0);
    }
    else {
      color = setWarningColor(sensor::waterLevel, user::targetMinWaterHeight, user::targetMaxWaterHeight, user::waterHeightErrorMargin);
      drawTwoValues(285, sensor::waterLevel, color, 1, 620, user::waterHeightErrorMargin, RA8875_BLACK, 1, "CM", 0);
    }
  }
  else if (display::warningsPage == 6) {
    if (display::refreshPage)
      tft.print(284, 126, "Air temp warning");
    if (user::convertToF) {
      color = setWarningColor(convertToF(sensor::airTemp), user::targetMinAirTempF, user::targetMaxAirTempF, user::airTempErrorMarginF);
      drawTwoValues(285, convertToF(sensor::airTemp), color, 1, 620, user::airTempErrorMarginF, RA8875_BLACK, 1, "F", 0);
    }
    else {
      color = setWarningColor(sensor::airTemp, user::targetMinAirTemp, user::targetMaxAirTemp, user::airTempErrorMargin);
      drawTwoValues(285, sensor::airTemp, color, 1, 620, user::airTempErrorMargin, RA8875_BLACK, 1, "C", 0);
    }
  }
  else if (display::warningsPage == 7) {
    if (display::refreshPage)
      tft.print(280, 126, "Humidity warning");
    color = setWarningColor(sensor::humidity, user::targetMinHumidity, user::targetMaxHumidity, user::humidityErrorMargin);
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
  else if (display::showGraphInterval)
    displaySetGraphInterval();
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(110, 130, "Display");
      int scrollPercentage = map(display::settingsPageZeroScrollPos, 0, 2, 0, 100);
      drawVerticalSlider(760, 100, 370, scrollPercentage);
    }
    int scrollMargin = display::settingsPageZeroScrollPos * 50;
    if (display::settingsPageZeroScrollPos == 0) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 170 - scrollMargin, "Background color");
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
        tft.print(110, 220 - scrollMargin, "Set display timeout");
        drawMiniConfirmButton(672, 220 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 2) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 270 - scrollMargin, "Set graph interval");
        drawMiniConfirmButton(672, 270 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 3) {
      if (display::refreshPage) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 320 - scrollMargin, "Set datetime");
        drawMiniConfirmButton(672, 320 - scrollMargin);
      }
    }
    if (display::settingsPageZeroScrollPos <= 4) {
      static bool previousTempInF;
      if (display::refreshPage || user::convertToF != previousTempInF) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 370 - scrollMargin, "Set temperature In F");
        drawMiniRadioButton(652, 370 - scrollMargin, user::convertToF);
        previousTempInF = user::convertToF ;
      }
    }
    if (display::settingsPageZeroScrollPos <= 5) {
      static bool previousHeightInInches;
      if (display::refreshPage || user::convertToInches != previousHeightInInches) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 420 - scrollMargin, "Set height In Inches");
        drawMiniRadioButton(652, 420 - scrollMargin, user::convertToInches);
        previousHeightInInches = user::convertToInches;
      }
    }
    if (display::settingsPageZeroScrollPos >= 1) {
      static bool previousConvertToTds;
      if (display::refreshPage || user::convertToTds != previousConvertToTds) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 470 - scrollMargin, "Set EC to TDS");
        drawMiniRadioButton(652, 470 - scrollMargin, user::convertToTds);
        previousConvertToTds = user::convertToTds;
      }
    }
    if (display::settingsPageZeroScrollPos >= 2) {
      static uint8_t previousConversionType;
      if (display::refreshPage || previousConversionType != device::conversionType) {
        tft.setFont(&akashi_36px_Regular);
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 520 - scrollMargin, "Set EC to TDS conversion");
        drawEcConversionButton(672, 520 - scrollMargin);
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
      tft.print(110, 130, "System");
      int scrollPercentage = map(display::settingsPageOneScrollPos, 0, 5, 0, 100);
      drawVerticalSlider(760, 100, 370, scrollPercentage);
    }
    int scrollMargin = display::settingsPageOneScrollPos * 50;
    if (display::settingsPageOneScrollPos == 0) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 170 - scrollMargin, "Show system logs");
        drawMiniConfirmButton(672, 170 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos <= 1) {
      static bool previousEnableWifi;
      if (display::refreshPage || wifi::wifiEnabled != previousEnableWifi) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 220 - scrollMargin, "Enable Wifi");
        drawMiniRadioButton(652, 220 - scrollMargin, wifi::wifiEnabled);
        previousEnableWifi = wifi::wifiEnabled;
      }
    }
    if (display::settingsPageOneScrollPos <= 2) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 270 - scrollMargin, "Set WiFi SSID");
        drawMiniConfirmButton(672, 270 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos <= 3) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 320 - scrollMargin, "Set WiFi password");
        drawMiniConfirmButton(672, 320 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos <= 4) {
      static bool previousHiddenNetwork;
      if (display::refreshPage || wifi::hiddenNetwork != previousHiddenNetwork) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 370 - scrollMargin, "Hide WiFi network");
        drawMiniRadioButton(652, 370 - scrollMargin, wifi::hiddenNetwork);
        previousHiddenNetwork = wifi::hiddenNetwork;
      }
    }
    if (display::settingsPageOneScrollPos <= 5) {
      static bool previousDisableLED;
      if (display::refreshPage || user::disableLED != previousDisableLED) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 420 - scrollMargin, "Disable LED");
        drawMiniRadioButton(652, 420 - scrollMargin, user::disableLED);
        previousDisableLED = user::disableLED;
      }
    }
    if (display::settingsPageOneScrollPos >= 1 && display::settingsPageOneScrollPos <= 6) {
      static bool previousDisableBeeper;
      if (display::refreshPage || user::disableBeeper != previousDisableBeeper) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 470 - scrollMargin, "Disable speaker");
        drawMiniRadioButton(652, 470 - scrollMargin, user::disableBeeper);
        previousDisableBeeper = user::disableBeeper;
      }
    }
    if (display::settingsPageOneScrollPos >= 2 && display::settingsPageOneScrollPos <= 6) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 520 - scrollMargin, "Clear graph history");
        drawMiniConfirmButton(672, 520 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos >= 3 && display::settingsPageOneScrollPos <= 7) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 570 - scrollMargin, "Clear max mins");
        drawMiniConfirmButton(672,570 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos >= 4 && display::settingsPageOneScrollPos <= 8) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 620 - scrollMargin, "Clear system logs");
        drawMiniConfirmButton(672, 620 - scrollMargin);
      }
    }
    if (display::settingsPageOneScrollPos >= 5 && display::settingsPageOneScrollPos <= 10) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 670 - scrollMargin, "Factory reset");
        drawMiniConfirmButton(672, 670 - scrollMargin);
      }
    }
  }
}
// =============================================================================================================================================================================================
// =============================================================================================================================================================================================
void drawSettingsPageTwo() {
  int scrollMargin = display::settingsPageTwoScrollPos * 50;
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
      tft.print(110, 130, "Enviroment");
      int scrollPercentage = map(display::settingsPageTwoScrollPos, 0, 1, 0, 100);
      drawVerticalSlider(760, 100, 370, scrollPercentage);
    }
    if (display::settingsPageTwoScrollPos <= 0) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 170 - scrollMargin, "Set number of dosers");
        drawMiniConfirmButton(672, 170 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 1) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 220 - scrollMargin, "Set dosing interval");
        drawMiniConfirmButton(672, 220 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 2) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 270 - scrollMargin, "Set EC dosing mode");
        drawMiniConfirmButton(672, 270 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 3) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 320 - scrollMargin, "Set PH dosing mode");
        drawMiniConfirmButton(672, 320 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 4) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 370 - scrollMargin, "Set EC/TDS value");
        drawMiniConfirmButton(672, 370 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos <= 5) {
      if (display::refreshPage) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 420 - scrollMargin, "Set PH down/up value");
        drawMiniConfirmButton(672, 420 - scrollMargin);
      }
    }
    if (display::settingsPageTwoScrollPos >= 1 && display::settingsPageTwoScrollPos <= 2) {
      static bool previousUseEtapeSensor;
      if (display::refreshPage || user::useEtapeSensor != previousUseEtapeSensor) {
        tft.setTextColor(RA8875_BLACK, user::backgroundColor);
        tft.print(110, 470 - scrollMargin, "Water height sensor");
        drawMiniEtapeButton(612, 470 - scrollMargin, user::useEtapeSensor);
        previousUseEtapeSensor = user::useEtapeSensor;
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
    tft.print(110, 130, "Warnings");
    int scrollPercentage = map(display::settingsPageThreeScrollPos, 0, 2, 0, 100);
    drawVerticalSlider(760, 100, 370, scrollPercentage);
  }
  int scrollMargin = display::settingsPageThreeScrollPos * 50;
  if (display::settingsPageThreeScrollPos == 0) {
    static bool previousDisablePpmWarnings;
    if (display::refreshPage || user::disablePpmWarnings != previousDisablePpmWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::ppmErrorState);
      tft.print(110, 170 - scrollMargin, "Disable PPM");
      drawMiniRadioButton(652, 170 - scrollMargin, user::disablePpmWarnings);
      previousDisablePpmWarnings = user::disablePpmWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 1) {
    static bool previousDisableEcWarnings;
    if (display::refreshPage || user::disableEcWarnings != previousDisableEcWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::ecErrorState);
      tft.print(110, 220 - scrollMargin, "Disable EC");
      drawMiniRadioButton(652, 220 - scrollMargin, user::disableEcWarnings);
      previousDisableEcWarnings = user::disableEcWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 2) {
    static bool previousDisablePhWarnings;
    if (display::refreshPage || user::disablePhWarnings != previousDisablePhWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::phErrorState);
      tft.print(110, 270 - scrollMargin, "Disable PH");
      drawMiniRadioButton(652, 270 - scrollMargin, user::disablePhWarnings);
      previousDisablePhWarnings = user::disablePhWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 3) {
    static bool previousDisableCo2Warnings;
    if (display::refreshPage || user::disableCo2Warnings != previousDisableCo2Warnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::co2ErrorState);
      tft.print(110, 320 - scrollMargin, "Disable Co2");
      drawMiniRadioButton(652, 320 - scrollMargin, user::disableCo2Warnings);
      previousDisableCo2Warnings = user::disableCo2Warnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 4) {
    static bool previousDisableWaterTempWarnings;
    if (display::refreshPage || user::disableWaterTempWarnings != previousDisableWaterTempWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::waterTempErrorState);
      tft.print(110, 370 - scrollMargin, "Disable water temp");
      drawMiniRadioButton(652, 370 - scrollMargin, user::disableWaterTempWarnings);
      previousDisableWaterTempWarnings = user::disableWaterTempWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos <= 5) {
    static bool previousDisableWaterHeightWarnings;
    if (display::refreshPage || user::disableWaterHeightWarnings != previousDisableWaterHeightWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::waterLevelErrorState);
      tft.print(110, 420 - scrollMargin, "Disable water height");
      drawMiniRadioButton(652, 420 - scrollMargin, user::disableWaterHeightWarnings);
      previousDisableWaterHeightWarnings = user::disableWaterHeightWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos >= 1 && display::settingsPageThreeScrollPos <= 6) {
    static bool previousDisableAirTempWarnings;
    if (display::refreshPage || user::disableAirTempWarnings != previousDisableAirTempWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::airTempErrorState);
      tft.print(110, 470 - scrollMargin, "Disable air temp");
      drawMiniRadioButton(652, 470 - scrollMargin, user::disableAirTempWarnings);
      previousDisableAirTempWarnings = user::disableAirTempWarnings;
    }
  }
  if (display::settingsPageThreeScrollPos >= 2 && display::settingsPageThreeScrollPos <= 7) {
    static bool previousDisableHumidityWarnings;
    if (display::refreshPage || user::disableHumidityWarnings != previousDisableHumidityWarnings) {
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      setTextWarningColor(device::humidityErrorState);
      tft.print(110, 520 - scrollMargin, "Disable humidity");
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
    static bool prevDoserIsPriming[6]{};
    int startPosition = display::calDoserPageScrollPos * 178;
    if (display::calDoserPageScrollPos == 0) { // DOSER 1
      static int doserOnSpeedPosition, previousDoserOneSpeed;
      drawDoser(100 - startPosition, 90, 1, user::doserOneSpeed, previousDoserOneSpeed, doserOnSpeedPosition, 1, -45);
      if (display::refreshPage || device::doserIsPriming[0] != prevDoserIsPriming[0]) {
        device::doserIsPriming[0] ? stopButton(136 - startPosition, 443) : primeButton(136 - startPosition, 443);
        prevDoserIsPriming[0] = device::doserIsPriming[0];
      }
    }
    if (display::calDoserPageScrollPos <= 1) { // DOSER 2
      static int doserTwoSpeedPosition, previousDoserTwoSpeed;
      drawDoser(278 - startPosition, 90, 2, user::doserTwoSpeed, previousDoserTwoSpeed, doserTwoSpeedPosition, 1, -45);
      if (display::refreshPage || device::doserIsPriming[1] != prevDoserIsPriming[1]) {
        device::doserIsPriming[1] ? stopButton(314 - startPosition, 443) : primeButton(314 - startPosition, 443);
        prevDoserIsPriming[0] = device::doserIsPriming[0];
      }
    }
    if (display::calDoserPageScrollPos <= 2) { // DOSER 3
      static int doserThreeSpeedPosition, previousDoserThreeSpeed;
      drawDoser(456 - startPosition, 90, 3, user::doserThreeSpeed, previousDoserThreeSpeed, doserThreeSpeedPosition, 1, -45);
      if (display::refreshPage || device::doserIsPriming[2] != prevDoserIsPriming[2]) {
        device::doserIsPriming[2] ? stopButton(492 - startPosition, 443) : primeButton(492 - startPosition, 443);
        prevDoserIsPriming[0] = device::doserIsPriming[0];
      }
    }
    if (display::calDoserPageScrollPos <= 2) { // DOSER 4
      static int doserFourSpeedPosition, previousDoserFourSpeed;
      drawDoser(634 - startPosition, 90, 4, user::doserFourSpeed, previousDoserFourSpeed, doserFourSpeedPosition, 1, -45);
      if (display::refreshPage || device::doserIsPriming[3] != prevDoserIsPriming[3]) {      
        device::doserIsPriming[3] ? stopButton(670 - startPosition, 443) : primeButton(670 - startPosition, 443);
        prevDoserIsPriming[0] = device::doserIsPriming[0];
      }
    }
    if (display::calDoserPageScrollPos >= 1) { // DOSER 5
      static int doserFiveSpeedPosition, previousDoserFiveSpeed;
      drawDoser(812 - startPosition, 90, 5, user::doserFiveSpeed, previousDoserFiveSpeed, doserFiveSpeedPosition, 1, -45);
      if (display::refreshPage || device::doserIsPriming[4] != prevDoserIsPriming[4]) {      
        device::doserIsPriming[4] ? stopButton(848 - startPosition, 443) : primeButton(848 - startPosition, 443);
        prevDoserIsPriming[0] = device::doserIsPriming[0];
      }
    }
    if (display::calDoserPageScrollPos == 2) { // DOSER 6
      static int doserSixSpeedPosition, previousDoserSixSpeed;
      drawDoser(990 - startPosition, 90, 6, user::doserSixSpeed, previousDoserSixSpeed, doserSixSpeedPosition, 1, -45);
      if (display::refreshPage || device::doserIsPriming[5] != prevDoserIsPriming[5]) {      
        device::doserIsPriming[5] ? stopButton(1026 - startPosition, 443) : primeButton(1026 - startPosition, 443);
        prevDoserIsPriming[0] = device::doserIsPriming[0];
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
        continueMessage(message::calibrateTds, sensor::ecCalSolutionPart1, 3, true, true, false);
      else
        continueMessage(message::calibrateTds, sensor::ecCalSolutionPart2, 2, true, true, false);
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
  else {
    if (display::refreshPage) {
      tft.setFont(&akashi_36px_Regular);
      tft.setFontScale(1);
      tft.setTextColor(RA8875_BLUE, user::backgroundColor);
      tft.print(110, 130, "Calibration");
      tft.setTextColor(RA8875_BLACK, user::backgroundColor);
      tft.print(110, 170, "Calibrate SR-04");
      tft.print(110, 220, "Calibrate TDS");
      tft.print(110, 270, "Calibrate Co2");
      tft.print(110, 320, "Calibrate PH");
      tft.print(110, 370, "Calibrate dosers");
      tft.print(110, 420, "Calibrate Etape");
      drawMiniConfirmButton(712, 170);
      drawMiniConfirmButton(712, 220);
      drawMiniConfirmButton(712, 270);
      drawMiniConfirmButton(712, 320);
      drawMiniConfirmButton(712, 370);
      drawMiniConfirmButton(712, 420);
    }
  }
}
