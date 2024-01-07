void handleRoot() {
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

void getData() {
  buildArgs();
  server.send(200, "text/plane", wifi::webData);
  yield();
}

void setValue() {
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

void buildArgs() {
  printf("Building web args...\n");
  memset(wifi::webData, 0, sizeof(wifi::webData));
  delay(1);
  // ec array val 0
  char buffer[8] {0, 0, 0, 0, 0, 0, 0, 0};
  dtostrf(device::ecArray[0], 0, 2, buffer);
  strcpy(wifi::webData, buffer); // val 0
  strcat(wifi::webData, ",");
  // ec array 120 floats (vals 1 - 120)
  for (uint8_t i = 1; i < device::maxGraphArrayValues; i++) {
    copyFloatToArray(device::ecArray[i], 2);
  }
  // tds array 120 ints (vals 121 - 241)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyIntToArrray(device::tdsArray[i]);
  }
  // ph array 120 floats (vals 222 - 342)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyFloatToArray(device::phArray[i], 2);
  }
  // co2 array 120 floats (vals 343 - 463)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyIntToArrray(device::co2Array[i]);
  }
  // water Level array 120 ints (vals 464 - 584)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyFloatToArray(user::convertToF ? convertToF(device::waterTemperatureArray[i]) : device::waterTemperatureArray[i], 1);
  }
    // air temperature array 120 floats (vals 585 - 705)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyFloatToArray(user::convertToInches ? convertToInch(device::waterLevelArray[i]) : device::waterLevelArray[i], 1);
  }
 // humidity array 120 ints (vals 706 - 826)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyFloatToArray(user::convertToF ? convertToF(device::airTemperatureArray[i]) : device::airTemperatureArray[i], 1);
  }
   // fan one speed array 120 ints (vals 827 - 947)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyFloatToArray(device::humidityArray[i], 1);
  }
 // fan two speed array 120 ints (vals 948 - 1068)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyIntToArrray(device::fanOneSpeedArray[i]);
  }
 // fan two speed array 120 ints (vals 1069 - 1189)
  for (uint8_t i = 0; i < device::maxGraphArrayValues; i++) {
    copyIntToArrray(device::fanTwoSpeedArray[i]);
  }
  // auto fill days array 31 ints (vals 1190 - 1290)
  for (uint8_t i = 0; i < 31; i++) {
    copyIntToArrray(user::autoFillDays[i]);
  }
  // graph array position (val 1291)
  copyIntToArrray(device::graphArrayPos);
  // profile 1 name (val 1292)
  strcat(wifi::webData, user::profileOneName);
  strcat(wifi::webData, ",");
  // profile 2 name (val 1293)
  strcat(wifi::webData, user::profileTwoName);
  strcat(wifi::webData, ",");
  // profile 3 name (vals 1294)
  strcat(wifi::webData, user::profileThreeName);
  strcat(wifi::webData, ",");
  // profile 4 name (vals 1295)
  strcat(wifi::webData, user::profileFourName);
  strcat(wifi::webData, ",");
  // profile 5 name (vals 1296)
  strcat(wifi::webData, user::profileFiveName);
  strcat(wifi::webData, ",");
  // Light on time hour (val 1297)
  copyIntToArrray(user::lightOnTimeHour);
  // Light on time min (val 1298)
  copyIntToArrray(user::lightOnTimeMin);
  // Light off time hour (val 1299)
  copyIntToArrray(user::lightOffTimeHour);
  // Light off time min (val 1300)
  copyIntToArrray(user::lightOffTimeMin);
  // Light mode (val 1301)
  copyIntToArrray(user::lightMode);
  // ec sensor (val 1302)
  copyFloatToArray(sensor::ec, 2);
  // tds sensor (val 1303)
  copyIntToArrray(sensor::tds);
  // ph sensor (val 1304)
  copyFloatToArray(sensor::ph, 2);
  // co2 sensor (val 1305)
  copyIntToArrray(sensor::co2);
  // water temp sensor (val 1306)
  copyFloatToArray(user::convertToF ? convertToF(sensor::waterTemp) : sensor::waterTemp, 1);
  // water level sensor (val 1307)
  copyFloatToArray(user::convertToInches ? convertToInch(sensor::waterLevel) : sensor::waterLevel, 1);
  // air temp sensor (val 1308)
  copyFloatToArray(user::convertToF ? convertToF(sensor::airTemp) : sensor::airTemp, 1);
  // humidity sensor (val 1309)
  copyFloatToArray(sensor::humidity, 1);
  // fan One Speed (val 1310)
  copyIntToArrray(device::fanOneSpeed);
  // fan two Speed (val 1311)
  copyIntToArrray(device::fanTwoSpeed);
  // fan One Min Speed (val 1312)
  copyIntToArrray(user::targetMinFanOneSpeed);
  // fan One Max Speed (val 1313)
  copyIntToArrray(user::targetMaxFanOneSpeed);
  // fan Two Min Speed (val 1314)
  copyIntToArrray(user::targetMinFanTwoSpeed);
  // fan Two Max Speed (val 1315)
  copyIntToArrray(user::targetMaxFanTwoSpeed);
  // fans Control Temp (val 1316)
  copyIntToArrray(user::fansControlTemperature);
  // fans Control Hum (val 1317)
  copyIntToArrray(user::fansControlHumidity);
  // fan One Fixed Speed (val 1318)
  copyIntToArrray(user::fanOneFixedSpeed);
  // fan Two Fixed Speed (val 1319)
  copyIntToArrray(user::fanTwoFixedSpeed);
  // target Min Air Temp (val 1320)
  copyFloatToArray(user::convertToF ? user::targetMinAirTempF : user::targetMinAirTemp, 1); 
  // target Max Air Temp (val 1321)
  copyFloatToArray(user::convertToF ? user::targetMaxAirTempF : user::targetMaxAirTemp, 1); 
  // target Min Humidity (val 1322)
  copyFloatToArray(user::targetMinHumidity, 1);
  // target Max Humidity (val 1323)
  copyFloatToArray(user::targetMaxHumidity, 1);
  // target Min Tds (val 1324)
  copyIntToArrray(user::targetMinTds);
  // target Max Tds (val 1325)
  copyIntToArrray(user::targetMaxTds);
  // target Co2 (val 1326)
  copyIntToArrray(user::targetCo2);
  // co2 Offset (val 1327)
  copyIntToArrray(user::co2Offset);
  // room Width (val 1328)
  copyIntToArrray(user::convertToInches ? user::roomWidthInches : user::roomWidthCm); 
  // room Length (val 1329)
  copyIntToArrray(user::convertToInches ? user::roomLengthInches : user::roomLengthCm); 
  // room Height (val 1330)
  copyIntToArrray(user::convertToInches ? user::roomHeightInches : user::roomHeightCm); 
  // co2 Flow rate (val 1331)
  copyFloatToArray(user::convertToInches ? user::co2FlowrateFeet3 :user::co2FlowrateLtrs, 1); 
  // co2 Check Time Minute (val 1332)
  copyIntToArrray(user::co2CheckTimeMinute);
  // co2 Check Time Hour (val 1333)
  copyIntToArrray(user::co2CheckTimeHour);
  // disable Fans Timer (val 1334)
  copyIntToArrray(user::disableFansTimer);
  // manual Co2 Gas Time Enabled (val 1335)
  copyIntToArrray(user::enableManualCo2Duration);
  // manual Gas Duration Time (val 1336)
  copyIntToArrray(user::manualCo2GasDuration);
  // co2 Control Disabled (val 1337)
  copyIntToArrray(user::disableCo2Control);
  // target Min Ph (val 1338)
  copyFloatToArray(user::targetMinPh, 2);
  // target Max Ph (val 1339)
  copyFloatToArray(user::targetMaxPh, 2);
  // target Min Ec (val 1340)
  copyFloatToArray(user::targetMinEc, 2);
  // target Max Ec (val 1341)
  copyFloatToArray(user::targetMaxEc, 2);
  // target Min Water Temp (val 1342)
  copyFloatToArray(user::convertToF ? user::targetMinWaterTempF : user::targetMinWaterTemp, 1);
  // target Max Water Temp (val 1343)
  copyFloatToArray(user::convertToF ? user::targetMaxWaterTempF : user::targetMaxWaterTemp, 1);
  // target Min Water Height (val 1344)
  copyFloatToArray(user::convertToInches ? user::targetMinWaterHeightInches : user::targetMinWaterHeight, 1);
  // target Max Water Height (val 1346)
  copyFloatToArray(user::convertToInches ? user::targetMaxWaterHeightInches : user::targetMaxWaterHeight, 1);
  // min Ph (val 1347)
  copyFloatToArray(device::minPh, 2);
  // max Ph (val 1348)
  copyFloatToArray(device::maxPh, 2);
  // min Co2 (val 1349)
  copyIntToArrray(device::minCo2);
  // max Co2 (val 1350)
  copyIntToArrray(device::maxCo2);
  // min Ec (val 1351)
  copyFloatToArray(device::minEc, 2);
  // max Ec (val 1352)
  copyFloatToArray(device::maxEc, 2);
  // min Water Temp (val 1353)
  copyFloatToArray(user::convertToF ? convertToF(device::minWaterTemp) : device::minWaterTemp, 1);
  // max Water Temp (val 1354)
  copyFloatToArray(user::convertToF ? convertToF(device::maxWaterTemp) : device::maxWaterTemp, 1); 
  // min Water Level (val 1355)
  copyFloatToArray(user::convertToInches ? convertToInch(device::minWaterLevel) : device::minWaterLevel, 1); 
  // max Water Level (val 1356)
  copyFloatToArray(user::convertToInches ? convertToInch(device::maxWaterLevel) : device::maxWaterLevel, 1); 
  // min Tds (val 1357)
  copyIntToArrray(device::minTds);
  // max Tds (val 1358)
  copyIntToArrray(device::maxTds);
  // min Air Temp (val 1359)
  copyFloatToArray(user::convertToF ? convertToF(device::minAirTemp) : device::minAirTemp, 1); 
  // max Air Temp (val 1360)
  copyFloatToArray(user::convertToF ? convertToF(device::maxAirTemp) : device::maxAirTemp, 1); 
  // min Humidity (val 1361)
  copyFloatToArray(device::minHumidity, 1);
  // max Humidity (val 1362)
  copyFloatToArray(device::maxHumidity, 1);
  // min Fan One Speed (val 1363)
  copyIntToArrray(device::minFanOneSpeed);
  // max Fan One Speed (val 1364)
  copyIntToArrray(device::maxFanOneSpeed);
  // min Fan Two Speed (val 1365)
  copyIntToArrray(device::minFanTwoSpeed);
  // max Fan Two Speed (val 1366)
  copyIntToArrray(device::maxFanTwoSpeed);
  // num Dosers (val 1367)
  copyIntToArrray(user::numberOfDosers);
  // doser One Mode (val 1368)
  copyIntToArrray(user::doserOneMode);
  // doser Two Mode (val 1369)
  copyIntToArrray(user::doserTwoMode);
  // doser Three Mode (val 1370)
  copyIntToArrray(user::doserThreeMode);
  // doser Four Mode (val 1371)
  copyIntToArrray(user::doserFourMode);
  // doser Five Mode (val 1372)
  copyIntToArrray(user::doserFiveMode);
  // doser Six Mode (val 1373)
  copyIntToArrray(user::doserSixMode);
  // doser One Mills (val 1374)
  copyIntToArrray(user::doserOneMills);
  // doser Two Mills (val 1374)
  copyIntToArrray(user::doserTwoMills);
  // doser Three Mills (val 1376)
  copyIntToArrray(user::doserThreeMills);
  // doser Four Mills (val 1377)
  copyIntToArrray(user::doserFourMills);
  // doser Five Mills (val 1378)
  copyIntToArrray(user::doserFiveMills);
  // doser Six Mills (val 1379)
  copyIntToArrray(user::doserSixMills);
  // doser One Speed
  //copyIntToArrray(user::doserOneSpeed);
  // doser TwoSpeed
  //copyIntToArrray(user::doserTwoSpeed);
  // doser ThreeSpeed
  //copyIntToArrray(user::doserThreeSpeed);
  // doser FourSpeed
  //copyIntToArrray(user::doserFourSpeed);
  // doser Five Speed
  //copyIntToArrray(user::doserFiveSpeed);
  // doser Six Speed
  //copyIntToArrray(user::doserSixSpeed);
  // dosing Interval (val 1380)
  copyIntToArrray(user::dosingInterval);
  // swap Interval (val 1381)
  copyIntToArrray(0);
  // refill Doser One Mills (val 1382)
  copyIntToArrray(user::refillDoserOneMills);
  // refill Doser Two Mills (val 1383)
  copyIntToArrray(user::refillDoserTwoMills);
  // refill Doser Three Mills (val 1384)
  copyIntToArrray(user::refillDoserThreeMills);
  // refill Doser Four Mills (val 1385)
  copyIntToArrray(user::refillDoserFourMills);
  // refill Doser Five Mills (val 1386)
  copyIntToArrray(user::refillDoserFiveMills);
  // refill Doser Six Mills (val 1387)
  copyIntToArrray(user::refillDoserSixMills);
  // auto Fill Hour (val 1388)
  copyIntToArrray(user::autoFillHour);
  // auto Fill Minute (val 1389)
  copyIntToArrray(user::autoFillMinute);
  // tds Error Margin (val 1390)
  copyIntToArrray(user::tdsErrorMargin);
  // ec Error Margin (val 1391)
  copyFloatToArray(user::ecErrorMargin, 2);
  // ph Error Margin (val 1392)
  copyFloatToArray(user::phErrorMargin, 2);
  // co2 Error Margin (val 1393)
  copyIntToArrray(user::co2ErrorMargin);
  // water Temp Error Margin (val 1394)
  copyFloatToArray(user::convertToF ? user::waterTempErrorMarginF : user::waterTempErrorMargin, 1);
  // water Height Error Margin (val 1395)
  copyFloatToArray(user::convertToInches ? user::waterHeightErrorMarginInches : user::waterHeightErrorMargin, 1); 
  // air Temp Error Margin (val 1396)
  copyFloatToArray(user::airTempErrorMargin, 1);
  // humidity Error Margin (val 1397)
  copyFloatToArray(user::humidityErrorMargin, 1);
  // disable Ppm Warning (val 1398)
  copyIntToArrray(user::disablePpmWarnings);
  // disable Ec Warnings (val 1399)
  copyIntToArrray(user::disableEcWarnings);
  // disable Co2 Warnings (val 1400)
  copyIntToArrray(user::disableCo2Warnings);
  // disable Ph Warnings (val 1401)
  copyIntToArrray(user::disablePhWarnings);
  // disable Water Temp Warnings (val 1402)
  copyIntToArrray(user::disableWaterTempWarnings);
  // disable Water Height Warnings (val 1403)
  copyIntToArrray(user::disableWaterHeightWarnings);
  // disable Air Temp Warnings (val 1404)
  copyIntToArrray(user::disableAirTempWarnings);
  // disable Humidity Warnings (val 1405)
  copyIntToArrray(user::disableHumidityWarnings);
  // disable Drain And Refill (val 1406)
  copyIntToArrray(user::disableDrainAndRefill);
  // convert Celcius To Farenheit (val 1407)
  copyIntToArrray(user::convertToF);
  // convert cm To Inchs (val 1408)
  copyIntToArrray(user::convertToInches);
  // convert To Tds (val 1409)
  copyIntToArrray(user::convertToTds);
  // ppm Error State (val 1410)
  copyIntToArrray(device::ppmErrorState);
  // ec Error State (val 1411)
  copyIntToArrray(device::ecErrorState);
  // ph Error State (val 1412)
  copyIntToArrray(device::phErrorState);
  // co2 Error State (val 1413)
  copyIntToArrray(device::co2ErrorState);
  // water Level Error State (val 1414)
  copyIntToArrray(device::waterTempErrorState);
  // water Level Error State (val 1415)
  copyIntToArrray(device::waterLevelErrorState);
  // air Temp Error State (val 1416)
  copyIntToArrray(device::airTempErrorState);
  // humidity Error State (val 1417)
  copyIntToArrray(device::humidityErrorState);
  // user Profile (val 1418)
  copyIntToArrray(device::userProfile);
   // user Profile (val 1419)
  copyIntToArrray(device::systemStatus);
  delay(1);
  yield();
  printf("webData Length = %d\n", strlen(wifi::webData));
  //printf("built wifi::webData array: %s\n", wifi::webData);
}

bool strBool(const char* buffer) {
  if (strstr(buffer, "true") != NULL)
    return true;
  else
    return false;
}

void copyIntToArrray(const int& a_value) {
  char buffer[8] {0, 0, 0, 0, 0, 0, 0, 0};
  itoa(a_value, buffer, 10);
  strcat(wifi::webData, buffer);
  strcat(wifi::webData, ",");
  delay(1);
  yield();
}

void copyFloatToArray(const float& a_value, const uint8_t& a_precison) {
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
