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

template <typename T> void getSet(uint32_t& a_address, const device::eepromModes a_mode, T &t) {
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
  getSet(address, a_mode, sensor::emptyWaterTankDepth);
  getSet(address, a_mode, sensor::etapeZeroVolumeResistance);
  getSet(address, a_mode, sensor::etapeMaxVolumeResistance);
  getSet(address, a_mode, sensor::ecKvalueLow);
  getSet(address, a_mode, sensor::ecKvalueHigh);
  getSet(address, a_mode, sensor::phNeutralVoltage);
  getSet(address, a_mode, sensor::phAcidicVoltage);
  //
  for (char& val : user::profileOneName) {
    getSet(address, a_mode, val);
  }
  for (char& val : user::profileTwoName) {
    getSet(address, a_mode, val);
  }
  for (char& val : user::profileThreeName) {
    getSet(address, a_mode, val);
  }
  for (char& val : user::profileFourName) {
    getSet(address, a_mode, val);
  }
  for (char& val : user::profileFiveName) {
    getSet(address, a_mode, val);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    getSet(address, a_mode, message::logTypeArray[i]);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    for (uint8_t n = 0; n < message::maxCharsPerLog; n++) {
      getSet(address, a_mode, message::timeStrArray[i][n]);
    }
  }
  for (float& val : sensor::phArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::ecArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::tdsArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::co2Array) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::waterTemperatureArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::waterTemperatureArrayF) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::waterLevelArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::waterLevelArrayInInches) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::fanOneSpeedArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::fanTwoSpeedArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::airTemperatureArray) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::airTemperatureArrayF) {
    getSet(address, a_mode, val);
  }
  for (float& val : sensor::humidityArray) {
    getSet(address, a_mode, val);
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
  getSet(a_address, a_mode, device::minPh);                      
  getSet(a_address, a_mode, device::maxPh);                      
  getSet(a_address, a_mode, device::minEc);                      
  getSet(a_address, a_mode, device::maxEc);                      
  getSet(a_address, a_mode, device::minWaterTemp);               
  getSet(a_address, a_mode, device::maxWaterTemp);               
  getSet(a_address, a_mode, device::minWaterLevel);              
  getSet(a_address, a_mode, device::maxWaterLevel);              
  getSet(a_address, a_mode, device::minTds);                     
  getSet(a_address, a_mode, device::maxTds);                     
  getSet(a_address, a_mode, device::minAirTemp);                 
  getSet(a_address, a_mode, device::maxAirTemp);                 
  getSet(a_address, a_mode, device::minHumidity);                
  getSet(a_address, a_mode, device::maxHumidity);                
  getSet(a_address, a_mode, device::minCo2);                     
  getSet(a_address, a_mode, device::maxCo2);                     
  getSet(a_address, a_mode, device::minFanOneSpeed);             
  getSet(a_address, a_mode, device::maxFanOneSpeed);             
  getSet(a_address, a_mode, device::minFanTwoSpeed);             
  getSet(a_address, a_mode, device::maxFanTwoSpeed);             
  getSet(a_address, a_mode, user::lightOnTimeHour);              
  getSet(a_address, a_mode, user::lightOnTimeMin);               
  getSet(a_address, a_mode, user::lightOffTimeHour);             
  getSet(a_address, a_mode, user::lightOffTimeMin);              
  getSet(a_address, a_mode, user::lightMode);                    
  getSet(a_address, a_mode, user::doserOneMode);                 
  getSet(a_address, a_mode, user::doserTwoMode);                 
  getSet(a_address, a_mode, user::doserThreeMode);               
  getSet(a_address, a_mode, user::doserFourMode);                
  getSet(a_address, a_mode, user::doserFiveMode);                
  getSet(a_address, a_mode, user::doserSixMode);                 
  getSet(a_address, a_mode, user::doserOneMills);                
  getSet(a_address, a_mode, user::doserTwoMills);                
  getSet(a_address, a_mode, user::doserThreeMills);              
  getSet(a_address, a_mode, user::doserFourMills);               
  getSet(a_address, a_mode, user::doserFiveMills);               
  getSet(a_address, a_mode, user::doserSixMills);                
  getSet(a_address, a_mode, user::targetMinTds);                 
  getSet(a_address, a_mode, user::targetMaxTds);                 
  getSet(a_address, a_mode, user::targetMinPh);                  
  getSet(a_address, a_mode, user::targetMaxPh);                  
  getSet(a_address, a_mode, user::targetMinEc);                  
  getSet(a_address, a_mode, user::targetMaxEc);                  
  getSet(a_address, a_mode, user::targetCo2);                    
  getSet(a_address, a_mode, user::co2Offset);                    
  getSet(a_address, a_mode, user::roomWidthCm);                  
  getSet(a_address, a_mode, user::roomLengthCm);                 
  getSet(a_address, a_mode, user::roomHeightCm);                 
  getSet(a_address, a_mode, user::co2FlowrateLtrs);              
  getSet(a_address, a_mode, user::roomWidthInches);              
  getSet(a_address, a_mode, user::roomLengthInches);             
  getSet(a_address, a_mode, user::roomHeightInches);             
  getSet(a_address, a_mode, user::co2FlowrateFeet3);             
  getSet(a_address, a_mode, user::co2CheckTimeMinute);           
  getSet(a_address, a_mode, user::co2CheckTimeHour);             
  getSet(a_address, a_mode, user::enableManualCo2Duration);      
  getSet(a_address, a_mode, user::disableCo2Control);            
  getSet(a_address, a_mode, user::manualCo2GasDuration);         
  getSet(a_address, a_mode, user::disableFansTimer);             
  getSet(a_address, a_mode, user::targetMinWaterTemp);           
  getSet(a_address, a_mode, user::targetMaxWaterTemp);           
  getSet(a_address, a_mode, user::targetMinWaterHeight);         
  getSet(a_address, a_mode, user::targetMaxWaterHeight);         
  getSet(a_address, a_mode, user::targetMinWaterHeightInches);   
  getSet(a_address, a_mode, user::targetMaxWaterHeightInches);   
  getSet(a_address, a_mode, user::waterTankLength);              
  getSet(a_address, a_mode, user::waterTankLengthInches);        
  getSet(a_address, a_mode, user::waterTankWidth);               
  getSet(a_address, a_mode, user::waterTankWidthInches);         
  getSet(a_address, a_mode, user::targetMinFanOneSpeed);         
  getSet(a_address, a_mode, user::targetMaxFanOneSpeed);         
  getSet(a_address, a_mode, user::targetMinFanTwoSpeed);         
  getSet(a_address, a_mode, user::targetMaxFanTwoSpeed);         
  getSet(a_address, a_mode, user::targetMinAirTemp);             
  getSet(a_address, a_mode, user::targetMaxAirTemp);             
  getSet(a_address, a_mode, user::targetMinAirTempF);            
  getSet(a_address, a_mode, user::targetMaxAirTempF);            
  getSet(a_address, a_mode, user::targetMinWaterTempF);          
  getSet(a_address, a_mode, user::targetMaxWaterTempF);          
  getSet(a_address, a_mode, user::targetMinHumidity);            
  getSet(a_address, a_mode, user::targetMaxHumidity);            
  getSet(a_address, a_mode, wifi::wifiEnabled);                  
  getSet(a_address, a_mode, user::doserOneSpeed);                
  getSet(a_address, a_mode, user::doserTwoSpeed);                
  getSet(a_address, a_mode, user::doserThreeSpeed);              
  getSet(a_address, a_mode, user::doserFourSpeed);               
  getSet(a_address, a_mode, user::doserFiveSpeed);               
  getSet(a_address, a_mode, user::doserSixSpeed);                
  getSet(a_address, a_mode, user::dosingInterval);               
  getSet(a_address, a_mode, user::refillDoserOneMills);          
  getSet(a_address, a_mode, user::refillDoserTwoMills);          
  getSet(a_address, a_mode, user::refillDoserThreeMills);        
  getSet(a_address, a_mode, user::refillDoserFourMills);         
  getSet(a_address, a_mode, user::refillDoserFiveMills);         
  getSet(a_address, a_mode, user::refillDoserSixMills);          
  getSet(a_address, a_mode, user::disableDrainAndRefill);        
  getSet(a_address, a_mode, user::autoFillHour);                 
  getSet(a_address, a_mode, user::autoFillMinute);               
  getSet(a_address, a_mode, user::drainTimeout);                 
  getSet(a_address, a_mode, user::backgroundColor);              
  getSet(a_address, a_mode, user::graphInterval);                
  getSet(a_address, a_mode, user::afkTime);                      
  getSet(a_address, a_mode, user::convertToF);                   
  getSet(a_address, a_mode, user::convertToInches);              
  getSet(a_address, a_mode, user::convertToTds);                 
  getSet(a_address, a_mode, user::tdsErrorMargin);               
  getSet(a_address, a_mode, user::phErrorMargin);                
  getSet(a_address, a_mode, user::ecErrorMargin);                
  getSet(a_address, a_mode, user::co2ErrorMargin);               
  getSet(a_address, a_mode, user::airTempErrorMargin);           
  getSet(a_address, a_mode, user::airTempErrorMarginF);          
  getSet(a_address, a_mode, user::humidityErrorMargin);          
  getSet(a_address, a_mode, user::waterTempErrorMargin);         
  getSet(a_address, a_mode, user::waterTempErrorMarginF);        
  getSet(a_address, a_mode, user::waterHeightErrorMargin);       
  getSet(a_address, a_mode, user::waterHeightErrorMarginInches);
  getSet(a_address, a_mode, user::disableAirTempWarnings);      
  getSet(a_address, a_mode, user::disableHumidityWarnings);      
  getSet(a_address, a_mode, user::disablePpmWarnings);      
  getSet(a_address, a_mode, user::disableEcWarnings);         
  getSet(a_address, a_mode, user::disablePhWarnings);          
  getSet(a_address, a_mode, user::disableCo2Warnings);         
  getSet(a_address, a_mode, user::disableWaterTempWarnings);    
  getSet(a_address, a_mode, user::disableWaterHeightWarnings);  
  getSet(a_address, a_mode, user::disableLED);         
  getSet(a_address, a_mode, user::disableBeeper);  
  getSet(a_address, a_mode, user::fansControlTemperature);      
  getSet(a_address, a_mode, user::fansControlHumidity);        
  getSet(a_address, a_mode, user::fanOneFixedSpeed);           
  getSet(a_address, a_mode, user::fanTwoFixedSpeed);      
  getSet(a_address, a_mode, user::numberOfDosers);      
  getSet(a_address, a_mode, user::useEtapeSensor);  
  getSet(a_address, a_mode, sensor::sensorArrayPos);     
  getSet(a_address, a_mode, device::conversionType);
  for (const bool& val : user::autoFillDays) {
    getSet(a_address, a_mode, val);   
  }
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(a_address);
  }
  return a_address;
}