// TO DO: remotlyLoadUserProfile, remotlySaveUserProfile, SETTINGS send vars, replace getData in javascript !
void handleRoot() {
  printf("handleRoot: Executing on core %d\n", xPortGetCoreID());
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Length", (String)sizeof(htmlText));
  server.send(200, "text/html", "");
  // send webpage in 2048-bit packets
  unsigned int byteCount = 0;
  char packetBuffer[2048];
  for (int n = 0; n < 2048; n++) {
    packetBuffer[n] = 0;
  }
  printf("Sending html file @ %d bytes...\n", sizeof(htmlText));
  for (unsigned int i = 0; i < sizeof(htmlText);) {
    if (byteCount < 2048) {
      packetBuffer[byteCount] = htmlText[i++];
      byteCount++;
    }
    else {
      server.sendContent_P(packetBuffer, byteCount);
      //printf("%s\n", packetBuffer);
      byteCount = 0;
      for (int n = 0; n < 2048; n++) {
        packetBuffer[n] = 0;
      }
      yield();
    }
  }
  if (byteCount > 0) {
    //printf("%s\n", packetBuffer);
    server.sendContent_P(packetBuffer, byteCount);
  }
  yield();
  printf("Packets sent!\n");
}

void setValue() {
  printf("setValue: Executing on core %d\n", xPortGetCoreID());
  yield();
  String cmd = server.arg("cmd");
  char val[64] {0};
  server.arg("val").toCharArray(val, 64);
  if (cmd == "" || val[0] == 0) { //Parameters not found
    printf("Web argument not found!\n");
  }
  else {
    printf("Received web cmd: %s and val: %s\n", cmd, val);
    // ============== PROFILES PAGE  ==============
    // profileOneName
    if (cmd == "profileOneName") {
      buffToCharArray(val, user::profileOneName, 11);
    }
    // profileTwoName
    else if (cmd == "profileTwoName") {
      buffToCharArray(val, user::profileTwoName, 11);
    }
    // profileThreeName
    else if (cmd == "profileThreeName") {
      buffToCharArray(val, user::profileThreeName, 11);
    }
    // profileFourName
    else if (cmd == "profileFourName") {
      buffToCharArray(val, user::profileFourName, 11);
    }
    // profileFiveName
    else if (cmd == "profileFiveName") {
      buffToCharArray(val, user::profileFiveName, 11);
    }
    // loadData
    else if (cmd == "loadData") {
      device::remotlyLoadUserProfile = atoi(val);
    }
    // saveData
    else if (cmd == "saveData") {
      device::remotlySaveUserProfile = atoi(val);
    }
    // ============== SETTINGS PAGE  ==============
    // numberOfDosers
    else if (cmd == "numDosers") {
      user::numberOfDosers = atoi(val);
    }
    // dosingInterval
    else if (cmd == "dosingInt") {
      user::dosingInterval = atoi(val);
    }
    // convertToFarenheit
    else if (cmd == "convertToF") {
      user::convertToF = strBool(val);
    }
    // convertToInchs
    else if (cmd == "convertToInchs") {
      user::convertToInches = strBool(val);
    }
    // convertToTds
    else if (cmd == "convertToTds") {
      user::convertToTds = strBool(val);
    }
    // disableCo2Warnings
    else if (cmd == "disableCo2Warn") {
      user::disableCo2Warnings = strBool(val);
    }
    // disableEcWarnings
    else if (cmd == "disableEcWarn") {
      user::disableEcWarnings = strBool(val);
    }
    // disablePpmWarnings
    else if (cmd == "disablePpmWarn") {
      user::disablePpmWarnings = strBool(val);
    }
    // disablePhWarnings
    else if (cmd == "disablePhWarn") {
      user::disablePhWarnings = strBool(val);
    }
    // disableWaterTempWarnings
    else if (cmd == "disableWaterTempWarn") {
      user::disableWaterTempWarnings = strBool(val);
    }
    // disableWaterHeightWarnings
    else if (cmd == "disableWaterHeightWarn") {
      user::disableWaterHeightWarnings = strBool(val);
    }
    // disableAirTempWarnings
    else if (cmd == "disableAirTempWarn") {
      user::disableAirTempWarnings = strBool(val);
    }
    // disableHumidityWarnings
    else if (cmd == "disableHumidityWarn") {
      user::disableHumidityWarnings = strBool(val);
    }
    // ============== EC PAGE ==============
    // targetMinEc
    else if (cmd == "targetMinEc") {
      user::targetMinEc = atof(val);
    }
    // targetMaxEc
    else if (cmd == "targetMaxEc") {
      user::targetMaxEc = atof(val);
    }
    // ============== TDS/PPM PAGE ==============
    // targetMinTds
    else if (cmd == "targetMinTds") {
      user::targetMinTds = atof(val);
    }
    // targetMaxTds
    else if (cmd == "targetMaxTds") {
      user::targetMaxTds = atof(val);
    }
    // ============== PH PAGE ==============
    // targetMinPh
    else if (cmd == "targetMinPh") {
      user::targetMinPh = atof(val);
    }
    // targetMaxPh
    else if (cmd == "targetMaxPh") {
      user::targetMaxPh = atof(val);
    }
    // ============== Co2 PAGE ==============
    // targetCo2
    else if (cmd == "targetCo2") {
      user::targetCo2 = atoi(val);
    }
    // co2Offset
    else if (cmd == "co2Offset") {
      user::co2Offset = atoi(val);
    }
    // roomLength
    else if (cmd == "roomLength") {
      (user::convertToInches ? user::roomLengthInches : user::roomLengthCm) = atoi(val);
    }
    // roomWidth
    else if (cmd == "roomWidth") {
       (user::convertToInches ? user::roomWidthInches : user::roomWidthCm) = atoi(val);
    }
    // roomHeight
    else if (cmd == "roomHeight") {
      (user::convertToInches ? user::roomHeightInches : user::roomHeightCm) = atoi(val);
    }
    // co2Flowrate
    else if (cmd == "co2Flowrate") {
      (user::convertToInches ? user::co2FlowrateFeet3 : user::co2FlowrateLtrs) = atof(val);
    }
    // co2CheckTimeMinute
    else if (cmd == "co2CheckTimeMin") {
      user::co2CheckTimeMinute = atoi(val);
    }
    // co2CheckTimeHour
    else if (cmd == "co2CheckTimeHour") {
      user::co2CheckTimeHour = atoi(val);
    }
    // disableFansTimer
    else if (cmd == "disableFansTimer") {
      user::disableFansTimer = atoi(val);
    }
    // manualCo2Duration
    else if (cmd == "gasTimeEnabled") {
      user::enableManualCo2Duration = strBool(val);
    }
    // manualCo2GasDuration
    else if (cmd == "gasTime") {
      user::manualCo2GasDuration = atoi(val);
    }
    // disableCo2Control
    else if (cmd == "disableCo2Control") {
      user::disableCo2Control = strBool(val);
    }
    // ============== WATER PAGE ==============
    // targetMinWaterHeight
    else if (cmd == "targetMinWaterLvl") {
      (user::convertToInches ? user::targetMinWaterHeightInches : user::targetMinWaterHeight) = atof(val);
    }
    // targetMaxWaterHeight
    else if (cmd == "targetMaxWaterLvl") {
      (user::convertToInches ? user::targetMaxWaterHeightInches : user::targetMaxWaterHeight) = atof(val);
    }
    // targetMinWaterTemp
    else if (cmd == "targetMinWaterTemp") {
      (user::convertToF ? user::targetMinWaterTempF : user::targetMinWaterTemp) = atof(val);
    }
    // targetMaxWaterTemp
    else if (cmd == "targetMaxWaterTemp") {
      (user::convertToF ? user::targetMaxWaterTempF : user::targetMaxWaterTemp) = atof(val);
    }
    // targetWaterTankLength
    else if (cmd == "targetWaterTankLength") {
      (user::convertToInches ? user::waterTankLengthInches : user::waterTankLength) = atof(val);
    }
    // targetWaterTankWidth
    else if (cmd == "targetWaterTankWidth") {
      (user::convertToInches ? user::waterTankWidthInches : user::waterTankWidth) = atof(val);
    }
    // autoFillDays
    else if (cmd == "autoFillDays") {
      buffToBoolArray(val, user::autoFillDays, 31);
    }
    // autoFillMinute
    else if (cmd == "autoFillMinute") {
      user::autoFillMinute = atoi(val);
    }
    // autoFillHour
    else if (cmd == "autoFillHour") {
      user::autoFillHour = atoi(val);
    }
    // disableDrainAndRefill
    else if (cmd == "disableRefillDrain") {
      user::disableDrainAndRefill = strBool(val);
    }
    // refillDoserOneMills
    else if (cmd == "d1mls") {
      user::refillDoserOneMills = atoi(val);
    }
    // refillDoserTwoMills
    else if (cmd == "d2mls") {
      user::refillDoserTwoMills = atoi(val);
    }
    // refillDoserThreeMills
    else if (cmd == "d3mls") {
      user::refillDoserThreeMills = atoi(val);
    }
    // refillDoserFourMills
    else if (cmd == "d4mls") {
      user::refillDoserFourMills = atoi(val);
    }
    // refillDoserFiveMills
    else if (cmd == "d5mls") {
      user::refillDoserFiveMills = atoi(val);
    }
    // refillDoserSixMills
    else if (cmd == "d6mls") {
      user::refillDoserSixMills = atoi(val);
    }
    // ============== DOSER PAGE ==============
    // doserOneMills
    else if (cmd == "doserMls_1") {
      user::doserOneMills = atoi(val);
    }
    // doserTwoMills
    else if (cmd == "doserMls_2") {
      user::doserTwoMills = atoi(val);
    }
    // doserThreeMills
    else if (cmd == "doserMls_3") {
      user::doserThreeMills = atoi(val);
    }
    // doserFourMills
    else if (cmd == "doserMls_4") {
      user::doserFourMills = atoi(val);
    }
    // doserFiveMills
    else if (cmd == "doserMls_5") {
      user::doserFiveMills = atoi(val);
    }
    // doserSixMills
    else if (cmd == "doserMls_6") {
      user::doserSixMills = atoi(val);
    }
    // doserOneMode
    else if (cmd == "doserOneMode") {
      user::doserOneMode = atoi(val);
    }
    // doserTwoMode
    else if (cmd == "doserTwoMode") {
      user::doserTwoMode = atoi(val);
    }
    // doserThreeMode
    else if (cmd == "doserThreeMode") {
      user::doserThreeMode = atoi(val);
    }
    // doserFourMode
    else if (cmd == "doserFourMode") {
      user::doserFourMode = atoi(val);
    }
    // doserFiveMode
    else if (cmd == "doserFiveMode") {
      user::doserFiveMode = atoi(val);
    }
    // doserSixMode
    else if (cmd == "doserSixMode") {
      user::doserSixMode = atoi(val);
    }
    // ============== LIGHT PAGE ==============
    // lightOnTimeMin
    else if (cmd == "onTimeMin") {
      user::lightOnTimeMin = atoi(val);
    }
    // lightOnTimeHour
    else if (cmd == "onTimeHour") {
      user::lightOnTimeHour = atoi(val);
    }
    // lightOffTimeMin
    else if (cmd == "offTimeMin") {
      user::lightOffTimeMin = atoi(val);
    }
    // lightOffTimeHour
    else if (cmd == "offTimeHour") {
      user::lightOffTimeHour = atoi(val);
    }
    // lightMode
    else if (cmd == "lightMode") {
      user::lightMode = atoi(val);
    }
    // ============== FANS PAGE ==============
    // targetMinFanOneSpeed
    else if (cmd == "fanOneMinSpeed") {
      user::targetMinFanOneSpeed = atoi(val);
    }
    // targetMaxFanOneSpeed
    else if (cmd == "fanOneMaxSpeed") {
      user::targetMaxFanOneSpeed = atoi(val);
    }
    // targetMinFanTwoSpeed
    else if (cmd == "fanTwoMinSpeed") {
      user::targetMinFanTwoSpeed = atoi(val);
    }
    // targetMaxFanTwoSpeed
    else if (cmd == "fanTwoMaxSpeed") {
      user::targetMaxFanTwoSpeed = atoi(val);
    }
    // targetMinAirTemp
    else if (cmd == "targetMinAirTemp") {
      (user::convertToF ? user::targetMinAirTempF : user::targetMinAirTemp) = atof(val);
    }
    // targetMaxAirTemp
    else if (cmd == "targetMaxAirTemp") {
      (user::convertToF ? user::targetMaxAirTempF : user::targetMaxAirTemp) = atof(val);
    }
    // targetMinHumidity
    else if (cmd == "targetMinHumidity") {
      user::targetMinHumidity = atof(val);
    }
    // targetMaxHumidity
    else if (cmd == "targetMaxHumidity") {
      user::targetMaxHumidity = atof(val);
    }
    // fansControlTemp
    else if (cmd == "fansControlTemp") {
      user::fansControlTemperature = strBool(val);
    }
    // fansControlHum
    else if (cmd == "fansControlHum") {
      user::fansControlHumidity = strBool(val);
    }
    // fanOneFixedSpeed
    else if (cmd == "fanOneFixedSpeed") {
      user::fanOneFixedSpeed = strBool(val);
    }
    // fanTwoFixedSpeed
    else if (cmd == "fanTwoFixedSpeed") {
      user::fanTwoFixedSpeed = strBool(val);
    }
    // ============== WARNINGS PAGE ==============
    // ecErrorMargin
    else if (cmd == "ecErrorMargin") {
      user::ecErrorMargin = atof(val);
    }
    // tdsErrorMargin
    else if (cmd == "tdsErrorMargin") {
      user::tdsErrorMargin = atoi(val);
    }
    // phErrorMargin
    else if (cmd == "phErrorMargin") {
      user::phErrorMargin = atof(val);
    }
    // co2ErrorMargin
    else if (cmd == "co2ErrorMargin") {
      user::co2ErrorMargin = atoi(val);
    }
    // waterHeightErrorMargin
    else if (cmd == "waterHeightErrorMargin") {
      (user::convertToInches ? user::waterHeightErrorMarginInches :user::waterHeightErrorMargin) = atof(val);
    }
    // waterTempErrorMargin
    else if (cmd == "waterTempErrorMargin") {
      (user::convertToF ? user::waterTempErrorMarginF :user::waterTempErrorMargin) = atof(val);
    }
    // airTempErrorMargin
    else if (cmd == "airTempErrorMargin") {
      (user::convertToF ? user::airTempErrorMarginF :user::airTempErrorMargin) = atof(val);
    }
    // humidityErrorMargin
    else if (cmd == "humidityErrorMargin") {
      user::humidityErrorMargin = atof(val);
    }
    else
      printf("server arg not recognised!\n");
  }
  server.send(200, "text/plane", "OK");
}

void getPageData() {
  printf("getPagedata: Executing on core %d\n", xPortGetCoreID());
  yield();
  String page = server.arg("page");
  String slide = server.arg("slide");
  printf("Received web request for page %s and slide %s\n", page, slide);
  memset(wifi::webData, 0, sizeof(wifi::webData));
    // Page 0 (Home page)
  if (page == "0") {
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::convertToInches); 
    if (slide == "0") { // home page slide 0
      copyFloatToArray(sensor::ec, 2);
      copyFloatToArray(user::targetMinEc, 2); 
      copyFloatToArray(user::targetMaxEc, 2); 
      copyFloatArrayToArray(device::ecArray, 120, 2);   
    }
    else if (slide == "1") { // home page slide 1
      copyIntToArray(sensor::tds);
      copyIntToArray(user::targetMinTds); 
      copyIntToArray(user::targetMaxTds); 
      copyFloatArrayToArray(device::tdsArray, 120, 0);   
    }
    else if (slide == "2") { // home page slide 2
      copyFloatToArray(sensor::ph, 2); 
      copyFloatToArray(user::targetMinPh, 2); 
      copyFloatToArray(user::targetMaxPh, 2); 
      copyFloatArrayToArray(device::phArray, 120, 2);
    }
    else if (slide == "3") { // home page slide 3
      copyIntToArray(sensor::co2); 
      copyIntToArray(user::targetCo2); 
      copyIntToArray(user::targetCo2); 
      copyFloatArrayToArray(device::co2Array, 120, 0);
    }
    else if (slide == "4") { // home page slide 4
      if (user::convertToF) {
        copyFloatToArray(convertToF(sensor::waterTemp), 1); 
        copyFloatToArray(user::targetMinWaterTempF, 1); 
        copyFloatToArray(user::targetMaxWaterTempF, 1); 
        copyFloatArrayToArray(device::waterTemperatureArrayF, 120, 1);
      }
      else {
        copyFloatToArray(sensor::waterTemp, 1); 
        copyFloatToArray(user::targetMinWaterTemp, 1); 
        copyFloatToArray(user::targetMaxWaterTemp, 1); 
        copyFloatArrayToArray(device::waterTemperatureArray, 120, 1);
      }
    }
    else if (slide == "5") { // home page slide 5
      if (user::convertToInches) {
        copyFloatToArray(sensor::waterLevelInches, 1); 
        copyFloatToArray(user::targetMinWaterHeightInches, 1); 
        copyFloatToArray(user::targetMaxWaterHeightInches, 1); 
        copyFloatArrayToArray(device::waterLevelArrayInInches, 120, 1);
      }
      else {
        copyFloatToArray(sensor::waterLevel, 1); 
        copyFloatToArray(user::targetMinWaterHeight, 1); 
        copyFloatToArray(user::targetMaxWaterHeight, 1); 
        copyFloatArrayToArray(device::waterLevelArray, 120, 1);
      }
    }
    else if (slide == "6") { // home page slide 6
      if (user::convertToF) {
        copyFloatToArray(convertToF(sensor::airTemp), 1); 
        copyFloatToArray(user::targetMinAirTempF, 1); 
        copyFloatToArray(user::targetMaxAirTempF, 1); 
        copyFloatArrayToArray(device::airTemperatureArrayF, 120, 1);
      }
      else {
        copyFloatToArray(sensor::airTemp, 1); 
        copyFloatToArray(user::targetMinAirTemp, 1); 
        copyFloatToArray(user::targetMaxAirTemp, 1); 
        copyFloatArrayToArray(device::airTemperatureArray, 120, 1);
      }
    }
    else if (slide == "7") { // home page slide 7
      copyFloatToArray(sensor::humidity, 1); 
      copyFloatToArray(user::targetMinHumidity, 1); 
      copyFloatToArray(user::targetMaxHumidity, 1); 
      copyFloatArrayToArray(device::humidityArray, 120, 1);
    }
    else if (slide == "8") { // home page slide 8
      copyIntToArray(device::fanOneSpeed); 
      copyIntToArray(user::targetMinFanOneSpeed); 
      copyIntToArray(user::targetMaxFanOneSpeed); 
      copyFloatArrayToArray(device::fanOneSpeedArray, 120, 0);
    }
    else if (slide == "9") { // home page slide 9
      copyIntToArray(device::fanTwoSpeed); 
      copyIntToArray(user::targetMinFanTwoSpeed); 
      copyIntToArray(user::targetMaxFanTwoSpeed); 
      copyFloatArrayToArray(device::fanTwoSpeedArray, 120, 0);
    }
  }
  // Page 1 (Graphs page)
  else if (page == "1") {
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::convertToInches); 
    if (slide == "0") {
      copyFloatArrayToArray(device::ecArray, 120, 2);     
    }
    else if (slide == "1") {
       copyFloatArrayToArray(device::tdsArray, 120, 0); 
    }
    else if (slide == "2") {
      copyFloatArrayToArray(device::phArray, 120, 2);
    }
    else if (slide == "3") {
      copyFloatArrayToArray(device::co2Array, 120, 0);
    }
    else if (slide == "4") {
      if (user::convertToF)
        copyFloatArrayToArray(device::waterTemperatureArrayF, 120, 1);
      else
        copyFloatArrayToArray(device::waterTemperatureArray, 120, 1);
    }
    else if (slide == "5") {
      if (user::convertToInches) 
        copyFloatArrayToArray(device::waterLevelArrayInInches, 120, 1);
      else
        copyFloatArrayToArray(device::waterLevelArray, 120, 1);
    }
    else if (slide == "6") {
      if (user::convertToF)
        copyFloatArrayToArray(device::airTemperatureArrayF, 120, 1);
      else
        copyFloatArrayToArray(device::airTemperatureArray, 120, 1);
    }
    else if (slide == "7") {
      copyFloatArrayToArray(device::humidityArray, 120, 1);
    }
    else if (slide == "8") {
      copyFloatArrayToArray(device::fanOneSpeedArray, 120, 0);
    }
    else if (slide == "9") {
      copyFloatArrayToArray(device::fanTwoSpeedArray, 120, 0);
    }
  }
   // Page 2 (Max mins page)
  else if (page == "2") {
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::convertToInches); 
    if (slide == "0") {
      copyFloatToArray(device::minEc, 2); 
      copyFloatToArray(device::maxEc, 2); 
    }
    else if (slide == "1") {
      copyIntToArray(device::minTds); 
      copyIntToArray(device::maxTds); 
    }
    else if (slide == "2") {
      copyFloatToArray(device::minPh, 2); 
      copyFloatToArray(device::maxPh, 2); 
    }
    else if (slide == "3") {
      copyIntToArray(device::minCo2); 
      copyIntToArray(device::maxCo2); 
    }
    else if (slide == "4") {
      if (user::convertToF) {
        copyFloatToArray(convertToF(device::minWaterTemp), 1); 
        copyFloatToArray(convertToF(device::maxWaterTemp), 1); 
      }
      else {
        copyFloatToArray(device::minWaterTemp, 1); 
        copyFloatToArray(device::maxWaterTemp, 1); 
      }
    }
    else if (slide == "5") {
      if (user::convertToInches) {
        copyFloatToArray(convertToInch(device::minWaterLevel), 1); 
        copyFloatToArray(convertToInch(device::maxWaterLevel), 1); 
      }
      else {
        copyFloatToArray(device::minWaterLevel, 1); 
        copyFloatToArray(device::maxWaterLevel, 1); 
      }
    }
    else if (slide == "6") {
      if (user::convertToF) {
        copyFloatToArray(convertToF(device::minAirTemp), 1); 
        copyFloatToArray(convertToF(device::maxAirTemp), 1); 
      }
      else {
        copyFloatToArray(device::minAirTemp, 1); 
        copyFloatToArray(device::maxAirTemp, 1); 
      }
    }
    else if (slide == "7") {
      copyFloatToArray(device::minHumidity, 1); 
      copyFloatToArray(device::maxHumidity, 1); 
    }
    else if (slide == "8") {
      copyIntToArray(device::minFanOneSpeed); 
      copyIntToArray(device::maxFanOneSpeed); 
    }
    else if (slide == "9") {
      copyFloatToArray(device::minFanTwoSpeed, 1); 
      copyFloatToArray(device::maxFanTwoSpeed, 1); 
    }
  }
  // Page 3 (Profiles page)
  else if (page == "3") {
    strcpy(wifi::webData, user::profileOneName);
    strcat(wifi::webData, user::profileTwoName);
    strcat(wifi::webData, user::profileThreeName);
    strcat(wifi::webData, user::profileFourName);
    strcat(wifi::webData, user::profileOneName);
  }
  // Page 4 (Settings page)
  else if (page == "4") {
    copyIntToArray(user::numberOfDosers); 
    copyIntToArray(user::dosingInterval); 
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::convertToInches); 
    copyIntToArray(user::convertToTds); 
    copyIntToArray(user::disablePpmWarnings);
    copyIntToArray(user::disableEcWarnings);
    copyIntToArray(user::disableCo2Warnings);
    copyIntToArray(user::disablePhWarnings);
    copyIntToArray(user::disableWaterTempWarnings);
    copyIntToArray(user::disableWaterHeightWarnings);
    copyIntToArray(user::disableAirTempWarnings);
    copyIntToArray(user::disableHumidityWarnings);
  }
  // Page 5 (EC\TDS page)
  else if (page == "5") {
    copyIntToArray(user::targetMinTds); 
    copyIntToArray(user::targetMaxTds); 
    copyFloatToArray(user::targetMinEc, 2); 
    copyFloatToArray(user::targetMaxEc, 2); 
  }
  // Page 6 (PH page)
  else if (page == "6") {
    copyFloatToArray(user::targetMinPh, 2); 
    copyFloatToArray(user::targetMaxPh, 2); 
  }
  // Page 7 (Co2 page) 
  else if (page == "7") { // small amount of data so no need to segment based on slider 
    copyIntToArray(user::convertToInches); 
    if (user::convertToInches) {
      copyFloatToArray(user::roomLengthInches, 1);
      copyFloatToArray(user::roomWidthInches, 1);
      copyFloatToArray(user::roomHeightInches, 1);
      copyFloatToArray(user::co2FlowrateFeet3, 1);
    }
    else {
      copyFloatToArray(user::roomLengthCm, 1);
      copyFloatToArray(user::roomWidthCm, 1);
      copyFloatToArray(user::roomHeightCm, 1);
      copyFloatToArray(user::co2FlowrateLtrs, 1);
    }
    copyIntToArray(user::co2CheckTimeMinute);
    copyIntToArray(user::co2CheckTimeHour);
    copyIntToArray(user::disableFansTimer);
    copyIntToArray(user::enableManualCo2Duration);
    copyIntToArray(user::disableCo2Control);
    copyIntToArray(user::manualCo2GasDuration);
  }
  // Page 8 (Water page)
  else if (page == "8") { // small amount of data so no need to segment based on slider
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::convertToInches); 
    if (user::convertToInches) {
      copyFloatToArray(user::targetMinWaterHeightInches, 1);
      copyFloatToArray(user::targetMaxWaterHeightInches, 1);
    }
    else {
      copyFloatToArray(user::targetMinWaterHeight, 1);
      copyFloatToArray(user::targetMaxWaterHeight, 1);
    }
    if (user::convertToF) {
      copyFloatToArray(user::targetMinWaterTempF, 1);
      copyFloatToArray(user::targetMaxWaterTempF, 1);
    }
    else {
      copyFloatToArray(user::targetMinWaterTemp, 1);
      copyFloatToArray(user::targetMaxWaterTemp, 1);
    }
    if (user::convertToInches) {
      copyFloatToArray(user::waterTankLengthInches, 1);
      copyFloatToArray(user::waterTankWidthInches, 1);
    }
    else {
      copyFloatToArray(user::waterTankLength, 1);
      copyFloatToArray(user::waterTankWidth, 1);
    }
    copyBoolArrayToArray(user::autoFillDays, 31);
    copyIntToArray(user::autoFillMinute);
    copyIntToArray(user::autoFillHour);
    copyIntToArray(user::disableDrainAndRefill);
    copyIntToArray(user::refillDoserOneMills);
    copyIntToArray(user::refillDoserTwoMills);
    copyIntToArray(user::refillDoserThreeMills);
    copyIntToArray(user::refillDoserFourMills);
    copyIntToArray(user::refillDoserFiveMills);
    copyIntToArray(user::refillDoserSixMills);
  }
  // Page 9 (Doser page) 
  else if (page == "9") {
    copyIntToArray(user::numberOfDosers); 
    copyIntToArray(user::doserOneMode);
    copyIntToArray(user::doserTwoMode);
    copyIntToArray(user::doserThreeMode);
    copyIntToArray(user::doserFourMode);
    copyIntToArray(user::doserFiveMode);
    copyIntToArray(user::doserSixMode);
    copyIntToArray(user::doserOneMills);
    copyIntToArray(user::doserTwoMills);
    copyIntToArray(user::doserThreeMills);
    copyIntToArray(user::doserFourMills);
    copyIntToArray(user::doserFiveMills);
    copyIntToArray(user::doserSixMills);
    //doserOneSpeed = values[cnt++];
    //doserTwoSpeed = values[cnt++];
    //doserThreeSpeed = values[cnt++];
    //doserFourSpeed = values[cnt++];
    //doserFiveSpeed = values[cnt++];
    //doserSixSpeed = values[cnt++];
  }
  // Page 10 (Lighting page) 
  else if (page == "10") {
    copyIntToArray(user::lightOnTimeMin);
    copyIntToArray(user::lightOnTimeHour);
    copyIntToArray(user::lightOffTimeMin);
    copyIntToArray(user::lightOffTimeHour);
    copyIntToArray(user::lightMode);
  }
  // Page 11 (Fans page)
  else if (page == "11") { // small amount of data so no need to segment based on slider
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::targetMinFanOneSpeed);
    copyIntToArray(user::targetMaxFanOneSpeed);
    copyIntToArray(user::targetMinFanTwoSpeed);
    copyIntToArray(user::targetMaxFanTwoSpeed);
    if (user::convertToF) {
      copyFloatToArray(user::targetMinAirTempF, 1);
      copyFloatToArray(user::targetMaxAirTempF, 1);
    }
    else {
      copyFloatToArray(user::targetMinAirTemp, 1);
      copyFloatToArray(user::targetMaxAirTemp, 1);
    }
    copyFloatToArray(user::targetMinHumidity, 1);
    copyFloatToArray(user::targetMaxHumidity, 1);
    copyIntToArray(user::fansControlTemperature);
    copyIntToArray(user::fansControlHumidity);
    copyIntToArray(user::fanOneFixedSpeed);
    copyIntToArray(user::fanTwoFixedSpeed);
  }
  // Page 12 (Warnings page)
  else if (page == "12") {  // small amount of data so no need to segment based on slider
    copyIntToArray(user::convertToF); 
    copyIntToArray(user::convertToInches); 
    copyIntToArray(sensor::tds);
    copyIntToArray(user::targetMinTds);
    copyIntToArray(user::targetMaxTds);
    copyIntToArray(user::tdsErrorMargin);
    copyFloatToArray(sensor::ec, 2);
    copyFloatToArray(user::targetMinEc, 2);
    copyFloatToArray(user::targetMaxEc, 2);
    copyFloatToArray(user::ecErrorMargin, 2);
    copyFloatToArray(sensor::ph, 2);
    copyFloatToArray(user::targetMinPh, 2);
    copyFloatToArray(user::targetMaxPh, 2);
    copyFloatToArray(user::phErrorMargin, 2);
    copyIntToArray(sensor::co2);
    copyIntToArray(user::targetCo2);
    copyIntToArray(user::targetCo2);
    copyIntToArray(user::co2ErrorMargin);
    if (user::convertToF) {
      copyFloatToArray(convertToF(sensor::waterTemp), 1);
      copyFloatToArray(user::targetMinWaterTempF, 1);
      copyFloatToArray(user::targetMaxWaterTempF, 1);
      copyFloatToArray(user::waterTempErrorMarginF, 1);
    }
    else {
      copyFloatToArray(sensor::waterTemp, 1);
      copyFloatToArray(user::targetMinWaterTemp, 1);
      copyFloatToArray(user::targetMaxWaterTemp, 1);
      copyFloatToArray(user::waterTempErrorMargin, 1);
    }
    if (user::convertToInches) {
      copyFloatToArray(sensor::waterLevelInches, 1);
      copyFloatToArray(user::targetMinWaterHeightInches, 1);
      copyFloatToArray(user::targetMaxWaterHeightInches, 1);
      copyFloatToArray(user::waterHeightErrorMarginInches, 1);
    }
    else {
      copyFloatToArray(sensor::waterLevel, 1);
      copyFloatToArray(user::targetMinWaterHeight, 1);
      copyFloatToArray(user::targetMaxWaterHeight, 1);
      copyFloatToArray(user::waterHeightErrorMargin, 1);
    }
    if (user::convertToF) {
      copyFloatToArray(convertToF(sensor::airTemp), 1);
      copyFloatToArray(user::targetMinAirTempF, 1);
      copyFloatToArray(user::targetMaxAirTempF, 1);
      copyFloatToArray(user::airTempErrorMarginF, 1);
    }
    else {
      copyFloatToArray(sensor::airTemp, 1);
      copyFloatToArray(user::targetMinAirTemp, 1);
      copyFloatToArray(user::targetMaxAirTemp, 1);
      copyFloatToArray(user::airTempErrorMargin, 1);
    }
    copyFloatToArray(sensor::humidity, 1);
    copyFloatToArray(user::targetMinHumidity, 1);
    copyFloatToArray(user::targetMaxHumidity, 1);
    copyFloatToArray(user::humidityErrorMargin, 1);
  }
  else {
    printf("Page not found!\n");
  }
  server.send(200, "text/plane", wifi::webData);
  yield();
}

bool strBool(const char* buffer) {
  if (strstr(buffer, "true") != NULL)
    return true;
  else
    return false;
}

void copyBoolArrayToArray(const bool* a_array, const int& a_len) {
  for (int i = 0; i < a_len; i++) {
    copyIntToArray(a_array[i]);
  }
}

void copyIntToArray(const int a_value) {
  char buffer[8] {0, 0, 0, 0, 0, 0, 0, 0};
  itoa(a_value, buffer, 10);
  strcat(wifi::webData, buffer);
  strcat(wifi::webData, ",");
  delay(1);
  yield();
}

void copyFloatArrayToArray(const float* a_array, const int& a_len, const uint8_t& a_precison) {
  for (int i = 0; i < a_len; i++) {
    copyFloatToArray(a_array[i], a_precison);
  }
}

void copyFloatToArray(const float a_value, const uint8_t& a_precison) {
  char buffer[8] {0, 0, 0, 0, 0, 0, 0, 0};
  dtostrf(a_value, 0, a_precison, buffer);
  strcat(wifi::webData, buffer);
  strcat(wifi::webData, ",");
  delay(1);
  yield();
}

void buffToCharArray(const char* a_src, char* a_dest, const int& a_size) {
  for (uint8_t i = 0; i < a_size; i++) {
    a_dest[i] = a_src[i];
  }
  delay(1);
  yield();
}

void buffToBoolArray(char* a_src, bool* a_dest, const int& a_size) {
  uint8_t i = 0;
  char* data = strtok(a_src, ",");
  while (data != NULL) {
    if (i < a_size)
      a_dest[i++] = atoi(data);
    data = strtok(NULL, ",");
  }
  delay(1);
  yield();
}
