void adjustScreenValuesTest() {
  //
  static int testPage = 0;
  static unsigned long prevPageMillis = 0;
  if (millis() - prevPageMillis >= 30000UL) {
    
    display::refreshPage = true;
    display::showInfoDialog = true;
    unsigned long startTime = millis();
    while (millis() - startTime < 5000UL) {
      infoMessage(message::infoMessageArray, message::infoPos); 
      display::refreshPage = false;
    }
    display::showInfoDialog = false;

    setPage(testPage);

    if (testPage < 34) 
      message::infoPos++;
    else 
      message::infoPos = 0;

    if (testPage < 12) {
      testPage++;
    }
    else {
      testPage = 0;   
      user::convertToF = !user::convertToF;
      user::convertToInches = !user::convertToInches;
    }

    prevPageMillis = millis();
  }
  // 
  static unsigned long prevMillis = 0;
  if (millis() - prevMillis >= 1000UL) {
    static bool highValues = false;
    if (display::page == 0) { 
      if (!highValues && sensor::tds > 0) {
        display::homePage < 9 ? display::homePage++ : display::homePage = 0; 
        display::refreshPage = true;
        clearPage();  
      }
      sensor::tds = (highValues ? 1000 : 0);
      user::targetMinTds = (highValues ? 1000 : 0); 
      user::targetMaxTds = (highValues ? 1000 : 0); 

      sensor::ec = (highValues ? 99 : 0);
      user::targetMinEc = (highValues ? 99 : 0);
      user::targetMaxEc = (highValues ? 99 : 0);

      sensor::ph = (highValues ? 14 : 0);
      user::targetMinPh = (highValues ? 14 : 0);
      user::targetMaxPh = (highValues ? 14 : 0);

      sensor::co2 = (highValues ? 5000 : 0); 
      user::targetCo2 = (highValues ? 5000 : 0);
      user::co2Offset = (highValues ? 99 : 0);

      if (user::convertToF) {
        sensor::waterTemp = (highValues ? 210 : -99);
        user::targetMinWaterTempF = (highValues ? 210 : -99);
        user::targetMaxWaterTempF = (highValues ? 210 : -99);
      }
      else {
        sensor::waterTemp = (highValues ? 210 : -99);
        user::targetMinWaterTemp = (highValues ? 210 : -99);
        user::targetMaxWaterTemp = (highValues ? 210 : -99);
      }

      if (user::convertToInches) {
        sensor::waterLevelInches = (highValues ? 999 : 0);
        user::targetMinWaterHeightInches = (highValues ? 999 : 0);
        user::targetMaxWaterHeightInches = (highValues ? 999 : 0);
      }
      else {
        sensor::waterLevel = (highValues ? 999 : 0);
        user::targetMinWaterHeight = (highValues ? 999 : 0);
        user::targetMaxWaterHeight = (highValues ? 999 : 0);
      }

      device::fanOneSpeed = (highValues ? 100 : 0); 
      user::targetMinFanOneSpeed = (highValues ? 100 : 0); 
      user::targetMaxFanOneSpeed = (highValues ? 100 : 0); 

      device::fanTwoSpeed = (highValues ? 100 : 0); 
      user::targetMinFanTwoSpeed = (highValues ? 100 : 0); 
      user::targetMaxFanTwoSpeed = (highValues ? 100 : 0); 

      if (user::convertToF) {
        sensor::airTemp = (highValues ? 210 : -99);
        user::targetMinAirTempF = (highValues ? 210 : -99);
        user::targetMaxAirTempF = (highValues ? 210 : -99);
      }
      else {
        sensor::airTemp = (highValues ? 210 : -99);
        user::targetMinAirTemp = (highValues ? 210 : -99);
        user::targetMaxAirTemp = (highValues ? 210 : -99);
      }

      sensor::humidity = (highValues ? 100 : 0);
      user::targetMinHumidity = (highValues ? 100 : 0);
      user::targetMaxHumidity = (highValues ? 100 : 0);
    }
    // Page 1 (Graphs page)
    else if (display::page == 1) {
    
    }
    // Page 2 (Max mins page)
    else if (display::page == 2) {
      if (!highValues && device::minTds > 0) {
        display::maxMinsPage < 9 ? display::maxMinsPage++ : display::maxMinsPage = 0; 
        display::refreshPage = true;
        clearPage();  
      }

      device::minTds = (highValues ? 1000 : 0); 
      device::maxTds = (highValues ? 1000 : 0); 

      device::minEc = (highValues ? 99 : 0);
      device::maxEc = (highValues ? 99 : 0);

      device::minPh = (highValues ? 14 : 0);
      device::maxPh = (highValues ? 14 : 0);

      device::minCo2 = (highValues ? 5000 : 0); 
      device::maxCo2 = (highValues ? 5000 : 0); 

      if (user::convertToF) {
        device::minWaterTemp = (highValues ? 210 : -99);
        device::maxWaterTemp = (highValues ? 210 : -99);
      }
      else {
        device::minWaterTemp = (highValues ? 210 : -99);
        device::maxWaterTemp = (highValues ? 210 : -99);
      }

      if (user::convertToInches) {
        device::minWaterLevel = (highValues ? 999 : 0);
        device::maxWaterLevel = (highValues ? 999 : 0);
      }
      else {
        device::minWaterLevel = (highValues ? 999 : 0);
        device::maxWaterLevel = (highValues ? 999 : 0);
      }

      device::minFanOneSpeed = (highValues ? 100 : 0); 
      device::maxFanOneSpeed = (highValues ? 100 : 0); 

      device::minFanTwoSpeed = (highValues ? 100 : 0);
      device::maxFanTwoSpeed = (highValues ? 100 : 0);

      if (user::convertToF) {
        device::minAirTemp = (highValues ? 210 : -99);
        device::maxAirTemp = (highValues ? 210 : -99);
      }
      else {
        device::minAirTemp = (highValues ? 210 : -99);
        device::maxAirTemp = (highValues ? 210 : -99);
      }

      device::minHumidity = (highValues ? 100 : 0);
      device::maxHumidity = (highValues ? 100 : 0);
    }
    // Page 4 (Settings page)
    else if (display::page == 4) {
      // TO DO !
    }
    // Page 5 (EC\TDS page)
    else if (display::page == 5) {
      if (user::convertToTds) {
        user::targetMinTds = (highValues ? 1000 : 0); 
        user::targetMaxTds = (highValues ? 1000 : 0); 
        if (!highValues && user::targetMinTds > 0) {
          user::convertToTds = false;
          display::refreshPage = true;
          clearPage(); 
        }
      }
      else {
        user::targetMinEc = (highValues ? 99 : 0);
        user::targetMaxEc = (highValues ? 99 : 0);
        if (!highValues && user::targetMinEc > 0) {
          user::convertToTds = true;
          display::refreshPage = true;
          clearPage(); 
        }
      }
    }
    // Page 6 (PH page)
    else if (display::page == 6) {
      user::targetMinPh = (highValues ? 14 : 0);
      user::targetMaxPh = (highValues ? 14 : 0);
    }
    // Page 7 (Co2 page) 
    else if (display::page == 7) { // small amount of data so no need to segment based on slider
      if (!highValues && user::targetCo2 > 0) {
        display::co2PageScrollPos < 3 ? display::co2PageScrollPos++ : display::co2PageScrollPos = 0; 
        display::refreshPage = true;
        clearPage(); 
      }
      user::targetCo2 = (highValues ? 9999 : 0);
      user::co2Offset = (highValues ? 99 : 0);
      if (user::convertToInches) {
        user::roomLengthInches = (highValues ? 999 : 0);
        user::roomWidthInches = (highValues ? 999 : 0);
        user::roomHeightInches = (highValues ? 999 : 0);
        user::co2FlowrateFeet3 = (highValues ? 999 : 0);
      }
      else {
        user::roomLengthCm = (highValues ? 999 : 0);
        user::roomWidthCm = (highValues ? 999 : 0);
        user::roomHeightCm = (highValues ? 999 : 0);
        user::co2FlowrateLtrs = (highValues ? 999 : 0);
      }
      user::co2CheckTimeMinute = (highValues ? 59 : 0);
      user::co2CheckTimeHour = (highValues ? 23 : 0);
      user::disableFansTimer = (highValues ? 1 : 0);
      user::enableManualCo2Duration = (highValues ? 1 : 0);
      user::disableCo2Control = (highValues ? 1 : 0);
      user::manualCo2GasDuration = (highValues ? 999 : 0); 
    }
    // Page 8 (Water page)
    else if (display::page == 8) { // small amount of data so no need to segment based on slider
      if (!highValues && user::targetCo2 > 0) {
        display::waterPageScrollPos < 4 ? display::waterPageScrollPos++ : display::waterPageScrollPos = 0; 
        display::refreshPage = true;
        clearPage(); 
      }

      if (user::convertToInches) {
        user::targetMinWaterHeightInches = (highValues ? 999 : 0);
        user::targetMaxWaterHeightInches = (highValues ? 999 : 0);
      }
      else {
        user::targetMinWaterHeight = (highValues ? 999 : 0);
        user::targetMaxWaterHeight = (highValues ? 999 : 0);
      }
      if (user::convertToF) {
        user::targetMinWaterTempF = (highValues ? 210 : -99);
        user::targetMaxWaterTempF = (highValues ? 210 : -99);
      }
      else {
        user::targetMinWaterTemp = (highValues ? 210 : -99);
        user::targetMaxWaterTemp = (highValues ? 210 : -99);
      }
      if (user::convertToInches) {
        user::waterTankLengthInches = (highValues ? 999 : 0);
        user::waterTankWidthInches = (highValues ? 999 : 0);
      }
      else {
        user::waterTankLength = (highValues ? 999 : 0);
        user::waterTankWidth = (highValues ? 999 : 0);
      }
      user::autoFillMinute = (highValues ? 59 : 0);
      user::autoFillHour = (highValues ? 23 : 0);
      user::disableDrainAndRefill = (highValues ? 1 : 0);
      user::refillDoserOneMills = (highValues ? 999 : 0);
      user::refillDoserTwoMills = (highValues ? 999 : 0);
      user::refillDoserThreeMills = (highValues ? 999 : 0);
      user::refillDoserFourMills = (highValues ? 999 : 0);
      user::refillDoserFiveMills = (highValues ? 999 : 0);
      user::refillDoserSixMills = (highValues ? 999 : 0);
    }
    // Page 9 (Doser page) 
    else if (display::page == 9) {
      if (!highValues && user::doserOneMills > 0) {
        display::doserPageScrollPos < 2 ? display::doserPageScrollPos++ : display::doserPageScrollPos = 0; 
        display::refreshPage = true;
        clearPage(); 
      }

      user::doserOneMills = (highValues ? 999 : 0);
      user::doserTwoMills = (highValues ? 999 : 0);
      user::doserThreeMills = (highValues ? 999 : 0);
      user::doserFourMills = (highValues ? 999 : 0);
      user::doserFiveMills = (highValues ? 999 : 0);
      user::doserSixMills = (highValues ? 999 : 0);
    }
    // Page 10 (Lighting page) 
    else if (display::page == 10) {
      user::lightOnTimeMin = (highValues ? 59 : 0);
      user::lightOnTimeHour = (highValues ? 23 : 0);
      user::lightOffTimeMin = (highValues ? 59 : 0);
      user::lightOffTimeHour = (highValues ? 23 : 0);
      user::lightMode = (highValues ? 1 : 0);
    }
    // Page 11 (Fans page)
    else if (display::page == 11) { // small amount of data so no need to segment based on slider
      if (!highValues && user::targetMinFanOneSpeed > 0) {
        display::fansPage < 4 ? display::fansPage++ : display::fansPage = 0; 
        display::refreshPage = true;
        clearPage(); 
      }

      user::targetMinFanOneSpeed = (highValues ? 100 : 0);
      user::targetMaxFanOneSpeed = (highValues ? 100 : 0);
      user::targetMinFanTwoSpeed = (highValues ? 100 : 0);
      user::targetMaxFanTwoSpeed = (highValues ? 100 : 0);
      if (user::convertToF) {
        user::targetMinAirTempF = (highValues ? 210 : -99);
        user::targetMaxAirTempF = (highValues ? 210 : -99);
      }
      else {
        user::targetMinAirTemp = (highValues ? 210 : -99);
        user::targetMaxAirTemp = (highValues ? 210 : -99);
      }
      user::targetMinHumidity = (highValues ? 100 : 0);
      user::targetMaxHumidity = (highValues ? 100 : 0);
      user::fansControlTemperature = (highValues ? 1 : 0);
      user::fansControlHumidity = (highValues ? 1 : 0);
      user::fanOneFixedSpeed = (highValues ? 1 : 0);
      user::fanTwoFixedSpeed = (highValues ? 1 : 0);
    }
    // Page 12 (Warnings page)
    else if (display::page == 12) {  // small amount of data so no need to segment based on slider
      if (!highValues && sensor::tds > 0) {
        display::warningsPage < 7 ? display::warningsPage++ : display::warningsPage = 0; 
        display::refreshPage = true;
        clearPage();
      }
      sensor::tds = (highValues ? 99 : 0);
      user::targetMinTds = (highValues ? 99 : 0);
      user::targetMaxTds = (highValues ? 99 : 0);
      user::tdsErrorMargin = (highValues ? 99 : 0);
      sensor::ec = (highValues ? 99 : 0);
      user::targetMinEc = (highValues ? 99 : 0);
      user::targetMaxEc = (highValues ? 99 : 0);
      user::ecErrorMargin = (highValues ? 99 : 0);
      sensor::ph = (highValues ? 14 : 0);
      user::targetMinPh = (highValues ? 14 : 0);
      user::targetMaxPh = (highValues ? 14 : 0);
      user::phErrorMargin = (highValues ? 9.9 : 0);
      sensor::co2 = (highValues ? 5000 : 0);
      user::targetCo2 = (highValues ? 5000 : 0);
      user::co2ErrorMargin = (highValues ? 99 : 0);
      if (user::convertToF) {
        sensor::waterTemp = (highValues ? 210 : -99);
        user::targetMinWaterTempF = (highValues ? 210 : -99);
        user::targetMaxWaterTempF = (highValues ? 210 : -99);
        user::waterTempErrorMarginF = (highValues ? 210 : 0);
      }
      else {
        sensor::waterTemp = (highValues ? 210 : -99);
        user::targetMinWaterTemp = (highValues ? 210 : -99);
        user::targetMaxWaterTemp = (highValues ? 210 : -99);
        user::waterTempErrorMargin = (highValues ? 210 : -99);
      }
      if (user::convertToInches) {
        sensor::waterLevelInches = (highValues ? 999 : 0);
        user::targetMinWaterHeightInches = (highValues ? 999 : 0);
        user::targetMaxWaterHeightInches = (highValues ? 999 : 0);
        user::waterHeightErrorMarginInches = (highValues ? 999 : 0);
      }
      else {
        sensor::waterLevel = (highValues ? 999 : 0);
        user::targetMinWaterHeight = (highValues ? 999 : 0);
        user::targetMaxWaterHeight = (highValues ? 999 : 0);
        user::waterHeightErrorMargin = (highValues ? 999 : 0);
      }
      if (user::convertToF) {
        sensor::airTemp = (highValues ? 210 : -99);
        user::targetMinAirTempF = (highValues ? 210 : -99);
        user::targetMaxAirTempF = (highValues ? 210 : -99);
        user::airTempErrorMarginF = (highValues ? 210 : -99);
      }
      else {
        sensor::airTemp = (highValues ? 210 : -99);
        user::targetMinAirTemp = (highValues ? 210 : -99);
        user::targetMaxAirTemp = (highValues ? 210 : -99);
        user::airTempErrorMargin = (highValues ? 210 : -99);
      }
      sensor::humidity = (highValues ? 100 : 0);
      user::targetMinHumidity = (highValues ? 100 : 0);
      user::targetMaxHumidity = (highValues ? 100 : 0);
      user::humidityErrorMargin = (highValues ? 100 : 0);
    }

    for (int i = 0; i < sensor::maxSensorArrayVals; i++) {
      sensor::phArray[i] = (float)random(1, 140) / 10;
      sensor::co2Array[i] = random(300, 5000);
      sensor::ecArray[i] = (float)random(1, 10000) / 10;
      sensor::tdsArray[i] = random(1, 99);
      sensor::waterTemperatureArray[i] = (float)random(10, 1000) / 10;
      sensor::waterTemperatureArrayF[i] = (float)random(10, 2100) / 10;
      sensor::waterLevelArray[i] = random(1, 99);
      sensor::waterLevelArrayInInches[i] = random(1, 99);
      sensor::airTemperatureArray[i] = (float)random(50, 1000) / 10;
      sensor::airTemperatureArrayF[i] = (float)random(320, 2100) / 10;
      sensor::humidityArray[i] = (float)random(0, 1000) / 10;
      sensor::fanOneSpeedArray[i] = random(0, 100);
      sensor::fanTwoSpeedArray[i] = random(0, 100);
    }
    sensor::sensorArrayPos = sensor::maxSensorArrayVals;

    highValues = !highValues;
    prevMillis = millis();
  }
}