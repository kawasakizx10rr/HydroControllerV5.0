void handleRoot() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Length", (String)sizeof(htmlText));
  server.send(200, "text/html", "");
  // send webpage in sizeof(buffer) byte packets
  unsigned int byteCount = 0;
  memset(wifi::buffer, 0, sizeof(wifi::buffer));
  if (device::globalDebug) {
    Serial.print(F("Sending "));
    Serial.print(sizeof(htmlText));
    Serial.println(F(" bytes..."));
  }
  for (unsigned int i = 0; i < sizeof(htmlText);) {
    if (byteCount < sizeof(wifi::buffer)) {
      wifi::buffer[byteCount] = pgm_read_byte(htmlText + i++);
      byteCount++;
    } 
    else {
      server.sendContent_P(wifi::buffer, byteCount);
      //if (device::globalDebug)
      //Serial.print(wifi::buffer);
      byteCount = 0;
      memset(wifi::buffer, 0, sizeof(wifi::buffer));
      yield();
    }
  }
  if (byteCount > 0) {
    //if (device::globalDebug)
    //Serial.print(wifi::buffer);
    server.sendContent_P(wifi::buffer, byteCount);
  }
  yield();
  if (device::globalDebug)
    Serial.println(F("Packets sent!"));
}

void setValue() {
 yield();
 char cmd[32]{0};
 char val[64]{0};
 server.arg("cmd").toCharArray(cmd, 32);
 server.arg("val").toCharArray(val, 64);
 if (cmd == "" || val[0] == 0) {  //Parameters not found
   if (device::globalDebug)
     Serial.println(F("Web argument not found!"));
 } 
 else {
   if (device::globalDebug) {
     Serial.print(F("Received cmd: "));
     Serial.print(cmd);
     Serial.print(F(" and val: "));
     Serial.println(val);
   }
   // ============== PROFILES PAGE ==============
   // profileOneName
   if (strcmp(cmd, "profileOneName") == 0) {
     buffToCharArray(val, user::profileOneName, 16);
   }
   // profileTwoName
   else if (strcmp(cmd, "profileTwoName") == 0) {
     buffToCharArray(val, user::profileTwoName, 16);
   }
   // profileThreeName
   else if (strcmp(cmd, "profileThreeName") == 0) {
     buffToCharArray(val, user::profileThreeName, 16);
   }
   // profileFourName
   else if (strcmp(cmd, "profileFourName") == 0) {
     buffToCharArray(val, user::profileFourName, 16);
   }
   // profileFiveName
   else if (strcmp(cmd, "profileFiveName") == 0) {
     buffToCharArray(val, user::profileFiveName, 16);
   }
   // loadData
   else if (strcmp(cmd, "loadData") == 0) {
     wifi::remotlyLoadUserProfile = atoi(val);
   }
   // saveData
   else if (strcmp(cmd, "saveData") == 0) {
     wifi::remotlySaveUserProfile = atoi(val);
   }
   // ============== SETTINGS PAGE  ==============
   // numberOfDosers
   else if (strcmp(cmd, "numDosers") == 0) {
     user::numberOfDosers = atoi(val);
   }
   // dosingInterval
   else if (strcmp(cmd, "dosingInt") == 0) {
     user::dosingInterval = atoi(val);
   }
   // convertToFarenheit
   else if (strcmp(cmd, "convertToF") == 0) {
     user::convertToF = strBool(val);
   }
   // convertToInchs
   else if (strcmp(cmd, "convertToInchs") == 0) {
     user::convertToInches = strBool(val);
   }
   // convertToTds
   else if (strcmp(cmd, "convertToTds") == 0) {
     user::convertToTds = strBool(val);
   }
   // disableCo2Warnings
   else if (strcmp(cmd, "disableCo2Warn") == 0) {
     user::disableCo2Warnings = strBool(val);
   }
   // disableEcWarnings
   else if (strcmp(cmd, "disableEcWarn") == 0) {
     user::disableEcWarnings = strBool(val);
   }
   // disablePpmWarnings
   else if (strcmp(cmd, "disablePpmWarn") == 0) {
     user::disablePpmWarnings = strBool(val);
   }
   // disablePhWarnings
   else if (strcmp(cmd, "disablePhWarn") == 0) {

     user::disablePhWarnings = strBool(val);
   }
   // disableWaterTempWarnings
   else if (strcmp(cmd, "disableWaterTempWarn") == 0) {
     user::disableWaterTempWarnings = strBool(val);
   }
   // disableWaterHeightWarnings
   else if (strcmp(cmd, "disableWaterHeightWarn") == 0) {
     user::disableWaterHeightWarnings = strBool(val);
   }
   // disableAirTempWarnings
   else if (strcmp(cmd, "disableAirTempWarn") == 0) {
     user::disableAirTempWarnings = strBool(val);
   }
   // disableHumidityWarnings
   else if (strcmp(cmd, "disableHumidityWarn") == 0) {
     user::disableHumidityWarnings = strBool(val);
   }
   // ============== EC PAGE ==============
   // targetMinEc
   else if (strcmp(cmd, "targetMinEc") == 0) {
     user::targetMinEc = atof(val);
   }
   // targetMaxEc
   else if (strcmp(cmd, "targetMaxEc") == 0) {
     user::targetMaxEc = atof(val);
   }
   // ============== TDS/PPM PAGE ==============
   // targetMinTds
   else if (strcmp(cmd, "targetMinTds") == 0) {
     user::targetMinTds = atof(val);
   }
   // targetMaxTds
   else if (strcmp(cmd, "targetMaxTds") == 0) {
     user::targetMaxTds = atof(val);
   }
   // ============== PH PAGE ==============
   // targetMinPh
   else if (strcmp(cmd, "targetMinPh") == 0) {
     user::targetMinPh = atof(val);
   }
   // targetMaxPh
   else if (strcmp(cmd, "targetMaxPh") == 0) {
     user::targetMaxPh = atof(val);
   }
   // ============== Co2 PAGE ==============
   // targetCo2
   else if (strcmp(cmd, "targetCo2") == 0) {
     user::targetCo2 = atoi(val);
   }
   // co2Offset
   else if (strcmp(cmd, "co2Offset") == 0) {
     user::co2Offset = atoi(val);
   }
   // roomLength
   else if (strcmp(cmd, "roomLength") == 0) {
     (user::convertToInches ? user::roomLengthInches : user::roomLengthCm) = atoi(val);
   }
   // roomWidth
   else if (strcmp(cmd, "roomWidth") == 0) {
     (user::convertToInches ? user::roomWidthInches : user::roomWidthCm) = atoi(val);
   }
   // roomHeight
   else if (strcmp(cmd, "roomHeight") == 0) {
     (user::convertToInches ? user::roomHeightInches : user::roomHeightCm) = atoi(val);
   }
   // co2Flowrate
   else if (strcmp(cmd, "co2Flowrate") == 0) {
     (user::convertToInches ? user::co2FlowrateFeet3 : user::co2FlowrateLtrs) = atof(val);
   }
   // co2CheckTimeMinute
   else if (strcmp(cmd, "co2CheckTimeMin") == 0) {
     user::co2CheckTimeMinute = atoi(val);
   }
   // co2CheckTimeHour
   else if (strcmp(cmd, "co2CheckTimeHour") == 0) {
     user::co2CheckTimeHour = atoi(val);
   }
   // disableFansTimer
   else if (strcmp(cmd, "disableFansTimer") == 0) {
     user::disableFansTimer = atoi(val);
   }
   // manualCo2Duration
   else if (strcmp(cmd, "gasTimeEnabled") == 0) {
     user::enableManualCo2Duration = strBool(val);
   }
   // manualCo2GasDuration
   else if (strcmp(cmd, "gasTime") == 0) {
     user::manualCo2GasDuration = atoi(val);
   }
   // disableCo2Control
   else if (strcmp(cmd, "disableCo2Control") == 0) {
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
   else if (strcmp(cmd, "autoFillDays") == 0) {
     buffToBitArray(val, user::autoFillDays, 31);
   }
   // autoFillMinute
   else if (strcmp(cmd, "autoFillMinute") == 0) {
     user::autoFillMinute = atoi(val);
   }
   // autoFillHour
   else if (strcmp(cmd, "autoFillHour") == 0) {
     user::autoFillHour = atoi(val);
   }
   // disableDrainAndRefill
   else if (strcmp(cmd, "disableRefillDrain") == 0) {
     user::disableDrainAndRefill = strBool(val);
   }
   // refillDoserOneMills
   else if (strcmp(cmd, "d1mls") == 0) {
     user::refillDoserOneMills = atoi(val);
   }
   // refillDoserTwoMills
   else if (strcmp(cmd, "d2mls") == 0) {
     user::refillDoserTwoMills = atoi(val);
   }
   // refillDoserThreeMills
   else if (strcmp(cmd, "d3mls") == 0) {
     user::refillDoserThreeMills = atoi(val);
   }
   // refillDoserFourMills
   else if (strcmp(cmd, "d4mls") == 0) {
     user::refillDoserFourMills = atoi(val);
   }
   // refillDoserFiveMills
   else if (strcmp(cmd, "d5mls") == 0) {
     user::refillDoserFiveMills = atoi(val);
   }
   // refillDoserSixMills
   else if (strcmp(cmd, "d6mls") == 0) {
     user::refillDoserSixMills = atoi(val);
   }
   // ============== DOSER PAGE ==============
   // doserOneMills
   else if (strcmp(cmd, "doserMls_1") == 0) {
     user::doserOneMills = atoi(val);
   }
   // doserTwoMills
   else if (strcmp(cmd, "doserMls_2") == 0) {
     user::doserTwoMills = atoi(val);
   }
   // doserThreeMills
   else if (strcmp(cmd, "doserMls_3") == 0) {
     user::doserThreeMills = atoi(val);
   }
   // doserFourMills
   else if (strcmp(cmd, "doserMls_4") == 0) {
     user::doserFourMills = atoi(val);
   }
   // doserFiveMills
   else if (strcmp(cmd, "doserMls_5") == 0) {
     user::doserFiveMills = atoi(val);
   }
   // doserSixMills
   else if (strcmp(cmd, "doserMls_6") == 0) {
     user::doserSixMills = atoi(val);
   }
   // doserOneMode
   else if (strcmp(cmd, "doserOneMode") == 0) {
     user::doserOneMode = atoi(val);
   }
   // doserTwoMode
   else if (strcmp(cmd, "doserTwoMode") == 0) {
     user::doserTwoMode = atoi(val);
   }
   // doserThreeMode
   else if (strcmp(cmd, "doserThreeMode") == 0) {
     user::doserThreeMode = atoi(val);
   }
   // doserFourMode
   else if (strcmp(cmd, "doserFourMode") == 0) {
     user::doserFourMode = atoi(val);
   }
   // doserFiveMode
   else if (strcmp(cmd, "doserFiveMode") == 0) {
     user::doserFiveMode = atoi(val);
   }
   // doserSixMode
   else if (strcmp(cmd, "doserSixMode") == 0) {
     user::doserSixMode = atoi(val);
   }
   // ============== TIMER PAGE ==============
   // lightOnTimeMin
   else if (strcmp(cmd, "lightOnTimeMin") == 0) {
     user::lightOnTimeMin = atoi(val);
   }
   // lightOnTimeHour
   else if (strcmp(cmd, "lightOnTimeHour") == 0) {
     user::lightOnTimeHour = atoi(val);
   }
   // lightOffTimeMin
   else if (strcmp(cmd, "lightOffTimeMin") == 0) {
     user::lightOffTimeMin = atoi(val);
   }
   // lightOffTimeHour
   else if (strcmp(cmd, "lightOffTimeHour") == 0) {
     user::lightOffTimeHour = atoi(val);
   }
   // lightOnDuration
   else if (strcmp(cmd, "lightOnDuration") == 0) {
     user::lightOnDuration = atoi(val);
   }
   // lightOffDuration
   else if (strcmp(cmd, "lightOffDuration") == 0) {
     user::lightOffDuration = atoi(val);
   }
   // lightState
   else if (strcmp(cmd, "lightState") == 0) {
     user::lightState = atoi(val);
   }
   // lightMode
   else if (strcmp(cmd, "lightMode") == 0) {
     user::lightMode = atoi(val);
   }
   // auxRelayOneOnTimeHour
   else if (strcmp(cmd, "auxRelayOneOnTimeHour") == 0) {
     user::auxRelayOneOnTimeHour = atoi(val);
   }
   // auxRelayOneOnTimeMin
   else if (strcmp(cmd, "auxRelayOneOnTimeMin") == 0) {
     user::auxRelayOneOnTimeMin = atoi(val);
   }
   // auxRelayOneOffTimeHour
   else if (strcmp(cmd, "auxRelayOneOffTimeHour") == 0) {
     user::auxRelayOneOffTimeHour = atoi(val);
   }
   // auxRelayOneOffTimeMin
   else if (strcmp(cmd, "auxRelayOneOffTimeMin") == 0) {
     user::auxRelayOneOffTimeMin = atoi(val);
   }
   // auxRelayOneOnDuration
   else if (strcmp(cmd, "auxRelayOneOnDuration") == 0) {
     user::auxRelayOneOnDuration = atoi(val);
   }
   // auxRelayOneOffDuration
   else if (strcmp(cmd, "auxRelayOneOffDuration") == 0) {
     user::auxRelayOneOffDuration = atoi(val);
   }
   // auxRelayOneState
   else if (strcmp(cmd, "auxRelayOneState") == 0) {
     user::auxRelayOneState = atoi(val);
   }
   // auxRelayOneMode
   else if (strcmp(cmd, "auxRelayOneMode") == 0) {
     user::auxRelayOneMode = atoi(val);
   }
   // auxRelayTwoOnTimeHour
   else if (strcmp(cmd, "auxRelayTwoOnTimeHour") == 0) {
     user::auxRelayTwoOnTimeHour = atoi(val);
   }
   // auxRelayTwoOnTimeMin
   else if (strcmp(cmd, "auxRelayTwoOnTimeMin") == 0) {
     user::auxRelayTwoOnTimeMin = atoi(val);
   }
   // auxRelayTwoOffTimeHour
   else if (strcmp(cmd, "auxRelayTwoOffTimeHour") == 0) {
     user::auxRelayTwoOffTimeHour = atoi(val);
   }
   // auxRelayTwoOffTimeMin
   else if (strcmp(cmd, "auxRelayTwoOffTimeMin") == 0) {
     user::auxRelayTwoOffTimeMin = atoi(val);
   }
   // auxRelayTwoOnDuration
   else if (strcmp(cmd, "auxRelayTwoOnDuration") == 0) {
     user::auxRelayTwoOnDuration = atoi(val);
   }
   // auxRelayTwoOffDuration
   else if (strcmp(cmd, "auxRelayTwoOffDuration") == 0) {
     user::auxRelayTwoOffDuration = atoi(val);
   }
   // auxRelayTwoState
   else if (strcmp(cmd, "auxRelayTwoState") == 0) {
     user::auxRelayTwoState = atoi(val);
   }
   // auxRelayTwoMode
   else if (strcmp(cmd, "auxRelayTwoMode") == 0) {
     user::auxRelayTwoMode = atoi(val);
   }
   // ============== FANS PAGE ==============
   // targetMinFanOneSpeed
   else if (strcmp(cmd, "fanOneMinSpeed") == 0) {
     user::targetMinFanOneSpeed = atoi(val);
   }
   // targetMaxFanOneSpeed
   else if (strcmp(cmd, "fanOneMaxSpeed") == 0) {
     user::targetMaxFanOneSpeed = atoi(val);
   }
   // targetMinFanTwoSpeed
   else if (strcmp(cmd, "fanTwoMinSpeed") == 0) {
     user::targetMinFanTwoSpeed = atoi(val);
   }
   // targetMaxFanTwoSpeed
   else if (strcmp(cmd, "fanTwoMaxSpeed") == 0) {
     user::targetMaxFanTwoSpeed = atoi(val);
   }
   // targetDayMinAirTemp
   else if (strcmp(cmd, "targetDayMinAirTemp") == 0) {
     (user::convertToF ? user::targetDayMinAirTempF : user::targetDayMinAirTemp) = atof(val);
   }
   // targetDayMaxAirTemp
   else if (strcmp(cmd, "targetDayMaxAirTemp") == 0) {
     (user::convertToF ? user::targetDayMaxAirTempF : user::targetDayMaxAirTemp) = atof(val);
   }
   // targetDayMinHumidity
   else if (strcmp(cmd, "targetDayMinHumidity") == 0) {
     user::targetDayMinHumidity = atof(val);
   }
   // targetDayMaxHumidity
   else if (strcmp(cmd, "targetDayMaxHumidity") == 0) {
     user::targetDayMaxHumidity = atof(val);
   }
   // targetNightMinAirTemp
   else if (strcmp(cmd, "targetNightMinAirTemp") == 0) {
     (user::convertToF ? user::targetNightMinAirTempF : user::targetNightMinAirTemp) = atof(val);
   }
   // targetNightMaxAirTemp
   else if (strcmp(cmd, "targetNightMaxAirTemp") == 0) {
     (user::convertToF ? user::targetNightMaxAirTempF : user::targetNightMaxAirTemp) = atof(val);
   }
   // targetNightMinHumidity
   else if (strcmp(cmd, "targetNightMinHumidity") == 0) {
     user::targetNightMinHumidity = atof(val);
   }
   // targetNightMaxHumidity
   else if (strcmp(cmd, "targetNightMaxHumidity") == 0) {
     user::targetNightMaxHumidity = atof(val);
   }
   // fansControlTemp
   else if (strcmp(cmd, "fansControlTemp") == 0) {
     user::fansControlTemperature = strBool(val);
   }
   // fansControlHum
   else if (strcmp(cmd, "fansControlHum") == 0) {
     user::fansControlHumidity = strBool(val);
   }
   // fanOneFixedSpeed
   else if (strcmp(cmd, "fanOneFixedSpeed") == 0) {
     user::fanOneFixedSpeed = strBool(val);
   }
   // fanTwoFixedSpeed
   else if (strcmp(cmd, "fanTwoFixedSpeed") == 0) {
     user::fanTwoFixedSpeed = strBool(val);
   }
   // ============== WARNINGS PAGE ==============
   // ecErrorMargin
   else if (strcmp(cmd, "ecErrorMargin") == 0) {
     user::ecErrorMargin = atof(val);
   }
   // tdsErrorMargin
   else if (strcmp(cmd, "tdsErrorMargin") == 0) {
     user::tdsErrorMargin = atoi(val);
   }
   // phErrorMargin
   else if (strcmp(cmd, "phErrorMargin") == 0) {
     user::phErrorMargin = atof(val);
   }
   // co2ErrorMargin
   else if (strcmp(cmd, "co2ErrorMargin") == 0) {
     user::co2ErrorMargin = atoi(val);
   }
   // waterHeightErrorMargin
   else if (strcmp(cmd, "waterHeightErrorMargin") == 0) {
     (user::convertToInches ? user::waterHeightErrorMarginInches :user::waterHeightErrorMargin) = atof(val);
   }
   // waterTempErrorMargin
   else if (strcmp(cmd, "waterTempErrorMargin") == 0) {
     (user::convertToF ? user::waterTempErrorMarginF :user::waterTempErrorMargin) = atof(val);
   }
   // airTempErrorMargin
   else if (strcmp(cmd, "airTempErrorMargin") == 0) {
     (user::convertToF ? user::airTempErrorMarginF :user::airTempErrorMargin) = atof(val);
   }
   // humidityErrorMargin
   else if (strcmp(cmd, "humidityErrorMargin") == 0) {
     user::humidityErrorMargin = atof(val);
   } 
   else if (device::globalDebug)
     Serial.println(F("server arg not recognised!"));
   yield();
 }
 server.send(200, "text/plane", "OK");
}

void getPageData() {
 yield();
 uint8_t page = atoi(server.arg("page").c_str());
 uint8_t slide = atoi(server.arg("slide").c_str());
 // Copy data to a buffer and send to the server
 if (device::globalDebug) {
   Serial.print(F("Received web request for page "));
   Serial.print(page);
   Serial.print(F(" and slide "));
   Serial.println(slide);
 }
 memset(wifi::buffer, 0, sizeof(wifi::buffer));
 // Page 0 (Home page)
 if (page == 0) {
   copyIntToArray(device::lightOn);
   copyIntToArray(user::convertToF);
   copyIntToArray(user::convertToInches);
   copyIntToArray(sensor::sensorArrayPos); 
   if (slide == 0) {  // home page slide 0
     copyFloatToArray(sensor::ec, 2);
     copyFloatToArray(user::targetMinEc, 2);
     copyFloatToArray(user::targetMaxEc, 2);
     copyFloatArrayToArray(sensor::ecArray, sensor::maxSensorArrayVals, 2);
   } 
   else if (slide == 1) {  // home page slide 1
     copyIntToArray(sensor::tds);
     copyIntToArray(user::targetMinTds);
     copyIntToArray(user::targetMaxTds);
     copyFloatArrayToArray(sensor::tdsArray, sensor::maxSensorArrayVals, 0);
   } 
   else if (slide == 2) {  // home page slide 2
     copyFloatToArray(sensor::ph, 2);
     copyFloatToArray(user::targetMinPh, 2);
     copyFloatToArray(user::targetMaxPh, 2);
     copyFloatArrayToArray(sensor::phArray, sensor::maxSensorArrayVals, 2);
   } 
   else if (slide == 3) {  // home page slide 3
     copyIntToArray(sensor::co2);
     copyIntToArray(user::targetCo2);
     copyIntToArray(user::targetCo2);
     copyFloatArrayToArray(sensor::co2Array, sensor::maxSensorArrayVals, 0);
   } 
   else if (slide == 4) {  // home page slide 4
     if (user::convertToF) {
       copyFloatToArray(convertToF(sensor::waterTemp), 1); 
       copyFloatToArray(user::targetMinWaterTempF, 1); 
       copyFloatToArray(user::targetMaxWaterTempF, 1); 
       copyFloatArrayToArray(sensor::waterTemperatureArrayF, sensor::maxSensorArrayVals, 1);
     }
     else {
       copyFloatToArray(sensor::waterTemp, 1); 
       copyFloatToArray(user::targetMinWaterTemp, 1); 
       copyFloatToArray(user::targetMaxWaterTemp, 1); 
       copyFloatArrayToArray(sensor::waterTemperatureArray, sensor::maxSensorArrayVals, 1);
     }
   } 
   else if (slide == 5) {  // home page slide 5
     if (user::convertToInches) {
       copyFloatToArray(convertToInches(sensor::waterLevel), 1); 
       copyFloatToArray(user::targetMinWaterHeightInches, 1); 
       copyFloatToArray(user::targetMaxWaterHeightInches, 1); 
       copyFloatArrayToArray(sensor::waterLevelArrayInInches, sensor::maxSensorArrayVals, 1);
     }
     else {
       copyFloatToArray(sensor::waterLevel, 1); 
       copyFloatToArray(user::targetMinWaterHeight, 1); 
       copyFloatToArray(user::targetMaxWaterHeight, 1); 
       copyFloatArrayToArray(sensor::waterLevelArray, sensor::maxSensorArrayVals, 1);
     }
   } 
   else if (slide == 6) {  // home page slide 6
     if (user::convertToF) {    
       copyFloatToArray(convertToF(sensor::airTemp), 1); 
       device::lightOn ? copyFloatToArray(user::targetDayMinAirTempF, 1) : copyFloatToArray(user::targetNightMinAirTempF, 1); 
       device::lightOn ? copyFloatToArray(user::targetDayMaxAirTempF, 1) : copyFloatToArray(user::targetNightMaxAirTempF, 1); 
       copyFloatArrayToArray(sensor::airTemperatureArrayF, sensor::maxSensorArrayVals, 1);
     }
     else {
       copyFloatToArray(sensor::airTemp, 1); 
       device::lightOn ? copyFloatToArray(user::targetDayMinAirTemp, 1) : copyFloatToArray(user::targetNightMinAirTemp, 1); 
       device::lightOn ? copyFloatToArray(user::targetDayMaxAirTemp, 1) : copyFloatToArray(user::targetNightMaxAirTemp, 1);        
       copyFloatArrayToArray(sensor::airTemperatureArray, sensor::maxSensorArrayVals, 1);
     }
   } 
   else if (slide == 7) {  // home page slide 7
     copyFloatToArray(sensor::humidity, 1);
     device::lightOn ? copyFloatToArray(user::targetDayMinHumidity, 1) : copyFloatToArray(user::targetNightMinHumidity, 1);
     device::lightOn ? copyFloatToArray(user::targetDayMaxHumidity, 1) : copyFloatToArray(user::targetNightMaxHumidity, 1);
     copyFloatArrayToArray(sensor::humidityArray, sensor::maxSensorArrayVals, 1);
   } 
   else if (slide == 8) {  // home page slide 8
     copyIntToArray(device::fanOneSpeed);
     copyIntToArray(user::targetMinFanOneSpeed);
     copyIntToArray(user::targetMaxFanOneSpeed);
     copyFloatArrayToArray(sensor::fanOneSpeedArray, sensor::maxSensorArrayVals, 0);
   } 
   else if (slide == 9) {  // home page slide 9
     copyIntToArray(device::fanTwoSpeed);
     copyIntToArray(user::targetMinFanTwoSpeed);
     copyIntToArray(user::targetMaxFanTwoSpeed);
     copyFloatArrayToArray(sensor::fanTwoSpeedArray, sensor::maxSensorArrayVals, 0);
   }
 }
 // Page 1 (Graphs page)
 else if (page == 1) {
   copyIntToArray(user::convertToF);
   copyIntToArray(user::convertToInches);
   copyIntToArray(sensor::sensorArrayPos); 
   if (slide == 0) {
     copyFloatArrayToArray(sensor::ecArray, sensor::maxSensorArrayVals, 2);
   } 
   else if (slide == 1) {
     copyFloatArrayToArray(sensor::tdsArray, sensor::maxSensorArrayVals, 0);
   } 
   else if (slide == 2) {
     copyFloatArrayToArray(sensor::phArray, sensor::maxSensorArrayVals, 2);
   } 
   else if (slide == 3) {
     copyFloatArrayToArray(sensor::co2Array, sensor::maxSensorArrayVals, 0);
   } 
   else if (slide == 4) {
     user::convertToF ? copyFloatArrayToArray(sensor::waterTemperatureArrayF, sensor::maxSensorArrayVals, 1) : copyFloatArrayToArray(sensor::waterTemperatureArray, sensor::maxSensorArrayVals, 1);
   } 
   else if (slide == 5) {
     user::convertToInches ? copyFloatArrayToArray(sensor::waterLevelArrayInInches, sensor::maxSensorArrayVals, 1): copyFloatArrayToArray(sensor::waterLevelArray, sensor::maxSensorArrayVals, 1);
   } 
   else if (slide == 6) {
     user::convertToF ? copyFloatArrayToArray(sensor::airTemperatureArrayF, sensor::maxSensorArrayVals, 1) : copyFloatArrayToArray(sensor::airTemperatureArray, sensor::maxSensorArrayVals, 1);
   } 
   else if (slide == 7) {
     copyFloatArrayToArray(sensor::humidityArray, sensor::maxSensorArrayVals, 1);
   } 
   else if (slide == 8) {
     copyFloatArrayToArray(sensor::fanOneSpeedArray, sensor::maxSensorArrayVals, 0);
   } 
   else if (slide == 9) {
     copyFloatArrayToArray(sensor::fanTwoSpeedArray, sensor::maxSensorArrayVals, 0);
   }
 }
 // Page 2 (Max mins page)
 else if (page == 2) {
   copyIntToArray(user::convertToF);
   copyIntToArray(user::convertToInches);
   if (slide == 0) {
     copyFloatToArray(device::minEc, 2);
     copyFloatToArray(device::maxEc, 2);
   } 
   else if (slide == 1) {
     copyIntToArray(device::minTds);
     copyIntToArray(device::maxTds);
   } 
   else if (slide == 2) {
     copyFloatToArray(device::minPh, 2);
     copyFloatToArray(device::maxPh, 2);
   } 
   else if (slide == 3) {
     copyIntToArray(device::minCo2);
     copyIntToArray(device::maxCo2);
   } 
   else if (slide == 4) {
     if (user::convertToF) {
       copyFloatToArray(convertToF(device::minWaterTemp), 1); 
       copyFloatToArray(convertToF(device::maxWaterTemp), 1); 
     }
     else {
       copyFloatToArray(device::minWaterTemp, 1); 
       copyFloatToArray(device::maxWaterTemp, 1); 
     }
   } 
   else if (slide == 5) {
     if (user::convertToInches) {
       copyFloatToArray(convertToInches(device::minWaterLevel), 1); 
       copyFloatToArray(convertToInches(device::maxWaterLevel), 1); 
     }
     else {
       copyFloatToArray(device::minWaterLevel, 1); 
       copyFloatToArray(device::maxWaterLevel, 1); 
     }
   } 
   else if (slide == 6) {
     if (user::convertToF) {
       copyFloatToArray(convertToF(device::minAirTemp), 1); 
       copyFloatToArray(convertToF(device::maxAirTemp), 1); 
     }
     else {
       copyFloatToArray(device::minAirTemp, 1); 
       copyFloatToArray(device::maxAirTemp, 1); 
     }
   } 
   else if (slide == 7) {
     copyFloatToArray(device::minHumidity, 1);
     copyFloatToArray(device::maxHumidity, 1);
   } 
   else if (slide == 8) {
     copyIntToArray(device::minFanOneSpeed);
     copyIntToArray(device::maxFanOneSpeed);
   } 
   else if (slide == 9) {
     copyFloatToArray(device::minFanTwoSpeed, 1);
     copyFloatToArray(device::maxFanTwoSpeed, 1);
   }
 }
 // Page 3 (Profiles page)
 else if (page == 3) {
   strcpy(wifi::buffer, user::profileOneName); strcat(wifi::buffer, ",");
   strcat(wifi::buffer, user::profileTwoName); strcat(wifi::buffer, ",");
   strcat(wifi::buffer, user::profileThreeName); strcat(wifi::buffer, ",");
   strcat(wifi::buffer, user::profileFourName); strcat(wifi::buffer, ",");
   strcat(wifi::buffer, user::profileOneName); strcat(wifi::buffer, ",");
 }
 // Page 4 (Settings page)
 else if (page == 4) {
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
 else if (page == 5) {
   copyIntToArray(user::convertToTds);
   copyIntToArray(user::targetMinTds);
   copyIntToArray(user::targetMaxTds);
   copyFloatToArray(user::targetMinEc, 2);
   copyFloatToArray(user::targetMaxEc, 2);
 }
 // Page 6 (PH page)
 else if (page == 6) {
   copyFloatToArray(user::targetMinPh, 2);
   copyFloatToArray(user::targetMaxPh, 2);
 }
 // Page 7 (Co2 page)
 else if (page == 7) {  // small amount of data so no need to segment based on slider
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
 else if (page == 8) {  // small amount of data so no need to segment based on slider
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
   copyIntToArray(user::autoFillMinute);
   copyIntToArray(user::autoFillHour);
   copyIntToArray(user::disableDrainAndRefill);
   copyIntToArray(user::refillDoserOneMills);
   copyIntToArray(user::refillDoserTwoMills);
   copyIntToArray(user::refillDoserThreeMills);
   copyIntToArray(user::refillDoserFourMills);
   copyIntToArray(user::refillDoserFiveMills);
   copyIntToArray(user::refillDoserSixMills);
   copyBitArrayToArray(user::autoFillDays, 31);
 }
 // Page 9 (Doser page)
 else if (page == 9) {
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
 // Page 10 (Timer page)
 else if (page == 10) {
   copyIntToArray(user::lightOnTimeHour);
   copyIntToArray(user::lightOnTimeMin);
   copyIntToArray(user::lightOffTimeHour);
   copyIntToArray(user::lightOffTimeMin);
   copyIntToArray(user::lightOnDuration);
   copyIntToArray(user::lightOffDuration);
   copyIntToArray(user::lightState);
   copyIntToArray(user::lightMode);
   copyIntToArray(user::auxRelayOneOnTimeHour);
   copyIntToArray(user::auxRelayOneOnTimeMin);
   copyIntToArray(user::auxRelayOneOffTimeHour);
   copyIntToArray(user::auxRelayOneOffTimeMin);
   copyIntToArray(user::auxRelayOneOnDuration);
   copyIntToArray(user::auxRelayOneOffDuration);
   copyIntToArray(user::auxRelayOneState);
   copyIntToArray(user::auxRelayOneMode);
   copyIntToArray(user::auxRelayTwoOnTimeHour);
   copyIntToArray(user::auxRelayTwoOnTimeMin);
   copyIntToArray(user::auxRelayTwoOffTimeHour);
   copyIntToArray(user::auxRelayTwoOffTimeMin);
   copyIntToArray(user::auxRelayTwoOnDuration);
   copyIntToArray(user::auxRelayTwoOffDuration);
   copyIntToArray(user::auxRelayTwoState);
   copyIntToArray(user::auxRelayTwoMode);
 }
 // Page 11 (Fans page)
 else if (page == 11) {  // small amount of data so no need to segment based on slider
   copyIntToArray(user::convertToF);
   copyIntToArray(user::targetMinFanOneSpeed);
   copyIntToArray(user::targetMaxFanOneSpeed);
   copyIntToArray(user::targetMinFanTwoSpeed);
   copyIntToArray(user::targetMaxFanTwoSpeed);
   if (user::convertToF) {
     copyFloatToArray(user::targetDayMinAirTempF, 1);
     copyFloatToArray(user::targetDayMaxAirTempF, 1);
     copyFloatToArray(user::targetNightMinAirTempF, 1);
     copyFloatToArray(user::targetNightMaxAirTempF, 1);
   }
   else {
     copyFloatToArray(user::targetDayMinAirTemp, 1);
     copyFloatToArray(user::targetDayMaxAirTemp, 1);
     copyFloatToArray(user::targetNightMinAirTemp, 1);
     copyFloatToArray(user::targetNightMaxAirTemp, 1);
   }
   copyFloatToArray(user::targetDayMinHumidity, 1);
   copyFloatToArray(user::targetDayMaxHumidity, 1);
   copyFloatToArray(user::targetNightMinAirTemp, 1);
   copyFloatToArray(user::targetNightMaxAirTemp, 1);
   copyFloatToArray(user::targetNightMinHumidity, 1);
   copyFloatToArray(user::targetNightMaxHumidity, 1);
   copyIntToArray(user::fansControlTemperature);
   copyIntToArray(user::fansControlHumidity);
   copyIntToArray(user::fanOneFixedSpeed);
   copyIntToArray(user::fanTwoFixedSpeed);
 }
 // Page 12 (Warnings page)
 else if (page == 12) {  // small amount of data so no need to segment based on slider
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
     copyFloatToArray(convertToInches(sensor::waterLevel), 1);
     copyFloatToArray(user::targetMinWaterHeightInches, 1);
     copyFloatToArray(user::targetMaxWaterHeightInches, 1);
     copyFloatToArray(user::waterHeightErrorMarginInches, 1);
   }
   else {
     copyFloatToArray(sensor::waterLevel, 1);
     copyFloatToArray(user::targetMinWaterHeight, 1);
     copyFloatToArray(user::targetMaxWaterHeight, 1);
     copyFloatToArray(user::waterHeightErrorMargin, 1);
   };
   if (user::convertToF) {
     copyFloatToArray(convertToF(sensor::airTemp), 1);
     device::lightOn ? copyFloatToArray(user::targetDayMinAirTempF, 1) : copyFloatToArray(user::targetNightMinAirTempF, 1);
     device::lightOn ? copyFloatToArray(user::targetDayMaxAirTempF, 1) : copyFloatToArray(user::targetNightMaxAirTempF, 1);
     copyFloatToArray(user::airTempErrorMarginF, 1);
   }
   else {
     copyFloatToArray(sensor::airTemp, 1);
     device::lightOn ? copyFloatToArray(user::targetDayMinAirTemp, 1) : copyFloatToArray(user::targetNightMinAirTemp, 1);
     device::lightOn ? copyFloatToArray(user::targetDayMaxAirTemp, 1) : copyFloatToArray(user::targetNightMaxAirTemp, 1);
     copyFloatToArray(user::airTempErrorMargin, 1);
   }
   copyFloatToArray(sensor::humidity, 1);
   device::lightOn ? copyFloatToArray(user::targetDayMinHumidity, 1) : copyFloatToArray(user::targetNightMinHumidity, 1);
   device::lightOn ? copyFloatToArray(user::targetDayMaxHumidity, 1) : copyFloatToArray(user::targetNightMaxHumidity, 1);
   copyFloatToArray(user::humidityErrorMargin, 1);
 }
 else if (device::globalDebug) {
   Serial.print(F("Page not found!"));
 }
 // Send responce to the server
 if (strlen(wifi::buffer) > 0)
   wifi::buffer[strlen(wifi::buffer) - 1] = 0;
  server.send(200, "text/plane", wifi::buffer);
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
  char buffer[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
  itoa(a_value, buffer, 10);
  strcat(wifi::buffer, buffer);
  strcat(wifi::buffer, ",");
  yield();
}

void copyBitArrayToArray(uint32_t& a_value, const int8_t a_len) {
  for (int i = a_len - 1; i >= 0; i--) {
   copyIntToArray(bitRead(a_value, i)); 
  }
}

void copyFloatArrayToArray(const float* a_array, const int& a_len, const uint8_t& a_precison) {
  for (int i = 0; i < a_len; i++) {
    copyFloatToArray(a_array[i], a_precison);
  }
}

void copyFloatToArray(const float a_value, const uint8_t& a_precison) {
  char buffer[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
  dtostrf(a_value, 0, a_precison, buffer);
  strcat(wifi::buffer, buffer);
  strcat(wifi::buffer, ",");
  yield();
}

void buffToCharArray(const char* a_src, char* a_dest, const int& a_size) {
  for (uint8_t i = 0; i < a_size; i++) {
    a_dest[i] = a_src[i];
  }
  yield();
}

void buffToBitArray(const char* a_src, uint32_t& a_value, const int8_t a_len) {
  uint8_t pos = 0;
   for (int i = a_len; i >= 0; i--) {
    bool bitVal = a_src[pos++] == '1' ? true : false;
    bitWrite(a_value, i, bitVal);
  }
}
