void initializeEEPROM() {
  char versionNum[6] {'\0'};
  for (uint8_t i = 0; i < 6; i++)
    versionNum[i] = externalEEPROM.read(i);
  device::systemEEPROMSize = getSetSystemEEPROM(device::EEPROM_VERIFY);
  device::profileEEPROMSize = getSetProfile(0, device::EEPROM_VERIFY);
  if (device::globalDebug) {
    Serial.print("Current version number: "); Serial.println(versionNum);
    Serial.print("Software version number: "); Serial.println(device::versionNumber);
    Serial.print("System EEPROM size: "); Serial.println(device::systemEEPROMSize);
    Serial.print("Profile EEPROM size: "); Serial.println(device::profileEEPROMSize);
  }
  if (strcmp(versionNum, device::versionNumber) == 0) {
    device::userProfile = externalEEPROM.read(6);
    getSetSystemEEPROM(device::EEPROM_GET);
    getSetProfileEEPROM(device::userProfile, device::EEPROM_GET);
  }
  else {
    //showResetMessage();
    for (uint8_t i = 0; i < 6; i++)
      externalEEPROM.put(i, device::versionNumber[i]);
    externalEEPROM.put(6, device::userProfile);
    getSetSystemEEPROM(device::EEPROM_SET);
    for (uint8_t i = 0; i < 5; i++)
      getSetProfileEEPROM(i, device::EEPROM_SET);
    clearPage();
  }
}

template <typename T> void eepromGetSet(uint32_t& a_address, const device::eepromModes a_mode, T &t) {
  if (a_mode == device::EEPROM_GET)
    externalEEPROM.get(a_address, t);
  else if (a_mode == device::EEPROM_SET)
    externalEEPROM.put(a_address, t);
  a_address += sizeof(T);
}

uint32_t getSetSystemEEPROM(const device::eepromModes a_mode) {
  uint32_t address = 7;
  if (device::globalDebug) {
    if (a_mode == device::EEPROM_VERIFY)
      Serial.print("Verifying"); 
    else if (a_mode == device::EEPROM_GET)
      Serial.print("Loading"); 
    else
      Serial.print("Saving"); 
    Serial.print(" system settings\nEEPROM starting at address: "); Serial.println(address);
  }
  eepromGetSet(address, a_mode, sensor::emptyWaterTankDepth);
  eepromGetSet(address, a_mode, sensor::etapeZeroVolumeResistance);
  eepromGetSet(address, a_mode, sensor::etapeMaxVolumeResistance);
  eepromGetSet(address, a_mode, sensor::ecKvalueLow);
  eepromGetSet(address, a_mode, sensor::ecKvalueHigh);
  eepromGetSet(address, a_mode, sensor::phNeutralVoltage);
  eepromGetSet(address, a_mode, sensor::phAcidicVoltage);
  //
  for (char& val : user::profileOneName) {
    eepromGetSet(address, a_mode, val);
  }
  for (char& val : user::profileTwoName) {
    eepromGetSet(address, a_mode, val);
  }
  for (char& val : user::profileThreeName) {
    eepromGetSet(address, a_mode, val);
  }
  for (char& val : user::profileFourName) {
    eepromGetSet(address, a_mode, val);
  }
  for (char& val : user::profileFiveName) {
    eepromGetSet(address, a_mode, val);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    eepromGetSet(address, a_mode, message::logTypeArray[i]);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    for (uint8_t n = 0; n < message::maxCharsPerLog; n++) {
      eepromGetSet(address, a_mode, message::timeStrArray[i][n]);
    }
  }
  for (float& val : sensor::phArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::ecArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::tdsArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::co2Array) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::waterTemperatureArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::waterTemperatureArrayF) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::waterLevelArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::waterLevelArrayInInches) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::fanOneSpeedArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::fanTwoSpeedArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::airTemperatureArray) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::airTemperatureArrayF) {
    eepromGetSet(address, a_mode, val);
  }
  for (float& val : sensor::humidityArray) {
    eepromGetSet(address, a_mode, val);
  }
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(address);
  }
  return address;
}

void getSetProfileEEPROM(const uint8_t a_profile, const device::eepromModes a_mode) {
  if (device::globalDebug) {
    if (a_mode == device::EEPROM_VERIFY)
      Serial.print("Verifying"); 
    else if (a_mode == device::EEPROM_GET)
      Serial.print("Loading"); 
    else
      Serial.print("Saving"); 
    Serial.print(" profile: "); Serial.println(a_profile);
  }
  uint32_t address = device::systemEEPROMSize + (a_profile * device::profileEEPROMSize); 
  getSetProfile(address, a_mode);
  delay(250);
}

uint32_t getSetProfile(uint32_t a_address, const device::eepromModes a_mode) {
  if (device::globalDebug) {
    Serial.print("EEPROM starting at address: "); Serial.println(a_address);
  }
  eepromGetSet(a_address, a_mode, device::minPh);                      
  eepromGetSet(a_address, a_mode, device::maxPh);                      
  eepromGetSet(a_address, a_mode, device::minEc);                      
  eepromGetSet(a_address, a_mode, device::maxEc);                      
  eepromGetSet(a_address, a_mode, device::minWaterTemp);               
  eepromGetSet(a_address, a_mode, device::maxWaterTemp);               
  eepromGetSet(a_address, a_mode, device::minWaterLevel);              
  eepromGetSet(a_address, a_mode, device::maxWaterLevel);              
  eepromGetSet(a_address, a_mode, device::minTds);                     
  eepromGetSet(a_address, a_mode, device::maxTds);                     
  eepromGetSet(a_address, a_mode, device::minAirTemp);                 
  eepromGetSet(a_address, a_mode, device::maxAirTemp);                 
  eepromGetSet(a_address, a_mode, device::minHumidity);                
  eepromGetSet(a_address, a_mode, device::maxHumidity);                
  eepromGetSet(a_address, a_mode, device::minCo2);                     
  eepromGetSet(a_address, a_mode, device::maxCo2);                     
  eepromGetSet(a_address, a_mode, device::minFanOneSpeed);             
  eepromGetSet(a_address, a_mode, device::maxFanOneSpeed);             
  eepromGetSet(a_address, a_mode, device::minFanTwoSpeed);             
  eepromGetSet(a_address, a_mode, device::maxFanTwoSpeed);             
  eepromGetSet(a_address, a_mode, user::lightOnTimeHour);              
  eepromGetSet(a_address, a_mode, user::lightOnTimeMin);               
  eepromGetSet(a_address, a_mode, user::lightOffTimeHour);             
  eepromGetSet(a_address, a_mode, user::lightOffTimeMin);              
  eepromGetSet(a_address, a_mode, user::lightMode);                    
  eepromGetSet(a_address, a_mode, user::doserOneMode);                 
  eepromGetSet(a_address, a_mode, user::doserTwoMode);                 
  eepromGetSet(a_address, a_mode, user::doserThreeMode);               
  eepromGetSet(a_address, a_mode, user::doserFourMode);                
  eepromGetSet(a_address, a_mode, user::doserFiveMode);                
  eepromGetSet(a_address, a_mode, user::doserSixMode);                 
  eepromGetSet(a_address, a_mode, user::doserOneMills);                
  eepromGetSet(a_address, a_mode, user::doserTwoMills);                
  eepromGetSet(a_address, a_mode, user::doserThreeMills);              
  eepromGetSet(a_address, a_mode, user::doserFourMills);               
  eepromGetSet(a_address, a_mode, user::doserFiveMills);               
  eepromGetSet(a_address, a_mode, user::doserSixMills);                
  eepromGetSet(a_address, a_mode, user::targetMinTds);                 
  eepromGetSet(a_address, a_mode, user::targetMaxTds);                 
  eepromGetSet(a_address, a_mode, user::targetMinPh);                  
  eepromGetSet(a_address, a_mode, user::targetMaxPh);                  
  eepromGetSet(a_address, a_mode, user::targetMinEc);                  
  eepromGetSet(a_address, a_mode, user::targetMaxEc);                  
  eepromGetSet(a_address, a_mode, user::targetCo2);                    
  eepromGetSet(a_address, a_mode, user::co2Offset);                    
  eepromGetSet(a_address, a_mode, user::roomWidthCm);                  
  eepromGetSet(a_address, a_mode, user::roomLengthCm);                 
  eepromGetSet(a_address, a_mode, user::roomHeightCm);                 
  eepromGetSet(a_address, a_mode, user::co2FlowrateLtrs);              
  eepromGetSet(a_address, a_mode, user::roomWidthInches);              
  eepromGetSet(a_address, a_mode, user::roomLengthInches);             
  eepromGetSet(a_address, a_mode, user::roomHeightInches);             
  eepromGetSet(a_address, a_mode, user::co2FlowrateFeet3);             
  eepromGetSet(a_address, a_mode, user::co2CheckTimeMinute);           
  eepromGetSet(a_address, a_mode, user::co2CheckTimeHour);             
  eepromGetSet(a_address, a_mode, user::enableManualCo2Duration);      
  eepromGetSet(a_address, a_mode, user::disableCo2Control);            
  eepromGetSet(a_address, a_mode, user::manualCo2GasDuration);         
  eepromGetSet(a_address, a_mode, user::disableFansTimer);             
  eepromGetSet(a_address, a_mode, user::targetMinWaterTemp);           
  eepromGetSet(a_address, a_mode, user::targetMaxWaterTemp);           
  eepromGetSet(a_address, a_mode, user::targetMinWaterHeight);         
  eepromGetSet(a_address, a_mode, user::targetMaxWaterHeight);         
  eepromGetSet(a_address, a_mode, user::targetMinWaterHeightInches);   
  eepromGetSet(a_address, a_mode, user::targetMaxWaterHeightInches);   
  eepromGetSet(a_address, a_mode, user::waterTankLength);              
  eepromGetSet(a_address, a_mode, user::waterTankLengthInches);        
  eepromGetSet(a_address, a_mode, user::waterTankWidth);               
  eepromGetSet(a_address, a_mode, user::waterTankWidthInches);         
  eepromGetSet(a_address, a_mode, user::targetMinFanOneSpeed);         
  eepromGetSet(a_address, a_mode, user::targetMaxFanOneSpeed);         
  eepromGetSet(a_address, a_mode, user::targetMinFanTwoSpeed);         
  eepromGetSet(a_address, a_mode, user::targetMaxFanTwoSpeed);         
  eepromGetSet(a_address, a_mode, user::targetMinAirTemp);             
  eepromGetSet(a_address, a_mode, user::targetMaxAirTemp);             
  eepromGetSet(a_address, a_mode, user::targetMinAirTempF);            
  eepromGetSet(a_address, a_mode, user::targetMaxAirTempF);            
  eepromGetSet(a_address, a_mode, user::targetMinWaterTempF);          
  eepromGetSet(a_address, a_mode, user::targetMaxWaterTempF);          
  eepromGetSet(a_address, a_mode, user::targetMinHumidity);            
  eepromGetSet(a_address, a_mode, user::targetMaxHumidity);            
  eepromGetSet(a_address, a_mode, wifi::wifiEnabled);                  
  eepromGetSet(a_address, a_mode, user::doserOneSpeed);                
  eepromGetSet(a_address, a_mode, user::doserTwoSpeed);                
  eepromGetSet(a_address, a_mode, user::doserThreeSpeed);              
  eepromGetSet(a_address, a_mode, user::doserFourSpeed);               
  eepromGetSet(a_address, a_mode, user::doserFiveSpeed);               
  eepromGetSet(a_address, a_mode, user::doserSixSpeed);                
  eepromGetSet(a_address, a_mode, user::dosingInterval);               
  eepromGetSet(a_address, a_mode, user::refillDoserOneMills);          
  eepromGetSet(a_address, a_mode, user::refillDoserTwoMills);          
  eepromGetSet(a_address, a_mode, user::refillDoserThreeMills);        
  eepromGetSet(a_address, a_mode, user::refillDoserFourMills);         
  eepromGetSet(a_address, a_mode, user::refillDoserFiveMills);         
  eepromGetSet(a_address, a_mode, user::refillDoserSixMills);          
  eepromGetSet(a_address, a_mode, user::disableDrainAndRefill);        
  eepromGetSet(a_address, a_mode, user::autoFillHour);                 
  eepromGetSet(a_address, a_mode, user::autoFillMinute);               
  eepromGetSet(a_address, a_mode, user::drainTimeout);                 
  eepromGetSet(a_address, a_mode, user::backgroundColor);              
  eepromGetSet(a_address, a_mode, user::graphInterval);                
  eepromGetSet(a_address, a_mode, user::afkTime);                      
  eepromGetSet(a_address, a_mode, user::convertToF);                   
  eepromGetSet(a_address, a_mode, user::convertToInches);              
  eepromGetSet(a_address, a_mode, user::convertToTds);                 
  eepromGetSet(a_address, a_mode, user::tdsErrorMargin);               
  eepromGetSet(a_address, a_mode, user::phErrorMargin);                
  eepromGetSet(a_address, a_mode, user::ecErrorMargin);                
  eepromGetSet(a_address, a_mode, user::co2ErrorMargin);               
  eepromGetSet(a_address, a_mode, user::airTempErrorMargin);           
  eepromGetSet(a_address, a_mode, user::airTempErrorMarginF);          
  eepromGetSet(a_address, a_mode, user::humidityErrorMargin);          
  eepromGetSet(a_address, a_mode, user::waterTempErrorMargin);         
  eepromGetSet(a_address, a_mode, user::waterTempErrorMarginF);        
  eepromGetSet(a_address, a_mode, user::waterHeightErrorMargin);       
  eepromGetSet(a_address, a_mode, user::waterHeightErrorMarginInches);
  eepromGetSet(a_address, a_mode, user::disableAirTempWarnings);      
  eepromGetSet(a_address, a_mode, user::disableHumidityWarnings);      
  eepromGetSet(a_address, a_mode, user::disablePpmWarnings);      
  eepromGetSet(a_address, a_mode, user::disableEcWarnings);         
  eepromGetSet(a_address, a_mode, user::disablePhWarnings);          
  eepromGetSet(a_address, a_mode, user::disableCo2Warnings);         
  eepromGetSet(a_address, a_mode, user::disableWaterTempWarnings);    
  eepromGetSet(a_address, a_mode, user::disableWaterHeightWarnings);  
  eepromGetSet(a_address, a_mode, user::disableLED);         
  eepromGetSet(a_address, a_mode, user::disableBeeper);  
  eepromGetSet(a_address, a_mode, user::fansControlTemperature);      
  eepromGetSet(a_address, a_mode, user::fansControlHumidity);        
  eepromGetSet(a_address, a_mode, user::fanOneFixedSpeed);           
  eepromGetSet(a_address, a_mode, user::fanTwoFixedSpeed);      
  eepromGetSet(a_address, a_mode, user::numberOfDosers);      
  eepromGetSet(a_address, a_mode, user::heightSensor);  
  eepromGetSet(a_address, a_mode, sensor::sensorArrayPos);     
  eepromGetSet(a_address, a_mode, device::conversionType);
  for (const bool& val : user::autoFillDays) {
    eepromGetSet(a_address, a_mode, val);   
  }
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(a_address);
  }
  return a_address;
}