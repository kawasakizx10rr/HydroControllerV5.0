void initializeEEPROM() {
  //EEPROM.begin(4096);
  char versionNum[6] {'0', '0', '0', '0', '0', '\0'};
  for (uint8_t i = 0; i < 6; i++)
    versionNum[i] = externalEEPROM.read(i);
  if (device::globalDebug) {
    Serial.print("Current version number: "); Serial.println(versionNum);
    Serial.print("Software version number: "); Serial.println(device::versionNumber);
  }
  if (strcmp(versionNum, device::versionNumber) == 0) {
    device::userProfile = externalEEPROM.read(6);
    loadSystemEEPROM();
    loadUserEEPROM(device::userProfile);
  }
  else {
    //showResetMessage();
    saveSystemEEPROM();
    for (uint8_t i = 0; i < 5; i++)
      saveUserEEPROM(i);
    for (uint8_t i = 0; i < 6; i++)
      externalEEPROM.put(i, device::versionNumber[i]);
    externalEEPROM.put(6, device::userProfile); // set profile to 0
    //EEPROM.commit();
    clearPage();
  }
}

void saveSystemEEPROM() {
  int address = 7;
  if (device::globalDebug) {
    Serial.print("Saving system settings\nEEPROM starting at address: "); Serial.println(address);
  }
  //externalEEPROM.put(address, sensor::ecCalibration);             address += sizeof(sensor::ecCalibration);
  externalEEPROM.put(address, sensor::emptyWaterTankDepth);       address += sizeof(sensor::emptyWaterTankDepth);
  externalEEPROM.put(address, sensor::etapeZeroVolumeResistance); address += sizeof(sensor::etapeZeroVolumeResistance);
  externalEEPROM.put(address, sensor::etapeMaxVolumeResistance);  address += sizeof(sensor::etapeMaxVolumeResistance);
  externalEEPROM.put(address, sensor::ecKvalueLow);               address += sizeof(sensor::ecKvalueLow);
  externalEEPROM.put(address, sensor::ecKvalueHigh);              address += sizeof(sensor::ecKvalueHigh);
  externalEEPROM.put(address, sensor::phNeutralVoltage);          address += sizeof(sensor::phNeutralVoltage);
  externalEEPROM.put(address, sensor::phAcidicVoltage);           address += sizeof(sensor::phAcidicVoltage);
  //
  for (const char& val : user::profileOneName) {
    externalEEPROM.put(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileTwoName) {
    externalEEPROM.put(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileThreeName) {
    externalEEPROM.put(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileFourName) {
    externalEEPROM.put(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileFiveName) {
    externalEEPROM.put(address, val);
    address += sizeof(val);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    externalEEPROM.put(address++, message::logTypeArray[i]);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    for (uint8_t n = 0; n < message::maxCharsPerLog; n++) {
      externalEEPROM.put(address++, message::timeStrArray[i][n]);
    }
  }
  for (const float& val : sensor::phArray) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::ecArray) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::tdsArray) {
    externalEEPROM.put(address, int(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::co2Array) {
    externalEEPROM.put(address, int(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::waterTemperatureArray) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::waterTemperatureArrayF) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::waterLevelArray) {
    externalEEPROM.put(address, int(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::waterLevelArrayInInches) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::fanOneSpeedArray) {
    externalEEPROM.put(address, int(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::fanTwoSpeedArray) {
    externalEEPROM.put(address, int(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::airTemperatureArray) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::airTemperatureArrayF) {
    externalEEPROM.put(address, fltToInt(val));
    address += sizeof(int);
  }
  for (const float& val : sensor::humidityArray) {
    externalEEPROM.put(address, int(val));
    address += sizeof(int);
  }
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(address);
  }
}

void loadSystemEEPROM() {
  int address = 8;
  if (device::globalDebug) {
    Serial.print("Loading system settings\nEEPROM starting at address: "); Serial.println(address);
  }
  //externalEEPROM.get(address, sensor::ecCalibration);             address += sizeof(sensor::ecCalibration);
  externalEEPROM.get(address, sensor::emptyWaterTankDepth);       address += sizeof(sensor::emptyWaterTankDepth);
  externalEEPROM.get(address, sensor::etapeZeroVolumeResistance); address += sizeof(sensor::etapeZeroVolumeResistance);
  externalEEPROM.get(address, sensor::etapeMaxVolumeResistance);  address += sizeof(sensor::etapeMaxVolumeResistance);
  externalEEPROM.get(address, sensor::ecKvalueLow);               address += sizeof(sensor::ecKvalueLow);
  externalEEPROM.get(address, sensor::ecKvalueHigh);              address += sizeof(sensor::ecKvalueHigh);
  externalEEPROM.get(address, sensor::phNeutralVoltage);          address += sizeof(sensor::phNeutralVoltage);
  externalEEPROM.get(address, sensor::phAcidicVoltage);           address += sizeof(sensor::phAcidicVoltage);
  for (const char& val : user::profileOneName) {
    externalEEPROM.get(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileTwoName) {
    externalEEPROM.get(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileThreeName) {
    externalEEPROM.get(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileFourName) {
    externalEEPROM.get(address, val);
    address += sizeof(val);
  }
  for (const char& val : user::profileFiveName) {
    externalEEPROM.get(address, val);
    address += sizeof(val);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    externalEEPROM.get(address++, message::logTypeArray[i]);
  }
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    for (uint8_t n = 0; n < message::maxCharsPerLog; n++) {
      externalEEPROM.get(address++, message::timeStrArray[i][n]);
    }
  }
  loadEepromFltArray(sensor::phArray, sensor::maxSensorArrayVals, address);
  loadEepromFltArray(sensor::ecArray, sensor::maxSensorArrayVals, address);
  loadEepromIntArray(sensor::tdsArray, sensor::maxSensorArrayVals, address);
  loadEepromIntArray(sensor::co2Array, sensor::maxSensorArrayVals, address);
  loadEepromFltArray(sensor::waterTemperatureArray, sensor::maxSensorArrayVals, address);
  loadEepromFltArray(sensor::waterTemperatureArrayF, sensor::maxSensorArrayVals, address);
  loadEepromIntArray(sensor::waterLevelArray, sensor::maxSensorArrayVals, address);
  loadEepromFltArray(sensor::waterLevelArrayInInches, sensor::maxSensorArrayVals, address);
  loadEepromIntArray(sensor::fanOneSpeedArray, sensor::maxSensorArrayVals, address);
  loadEepromIntArray(sensor::fanTwoSpeedArray, sensor::maxSensorArrayVals, address);
  loadEepromFltArray(sensor::airTemperatureArray, sensor::maxSensorArrayVals, address);
  loadEepromFltArray(sensor::airTemperatureArrayF, sensor::maxSensorArrayVals, address);
  loadEepromIntArray(sensor::humidityArray, sensor::maxSensorArrayVals, address);
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(address);
  }
}

void loadUserEEPROM(const uint8_t a_profile) {
  if (device::globalDebug) {
    Serial.print("Loading profile: "); Serial.println(a_profile);
  }
  if (a_profile == 0)
    loadProfile(375);
  else if (a_profile == 1)
    loadProfile(1084); // user eeprom size per profile = 531 bytes   493
  else if (a_profile == 2)
    loadProfile(1793);
  else if (a_profile == 3)
    loadProfile(2502);
  else if (a_profile == 4)
    loadProfile(3211); // to 3920 "176 bytes spare of 4096"
  delay(250);
}

void saveUserEEPROM(const uint8_t a_profile) {
  if (device::globalDebug) {
    Serial.print("Saving profile: "); Serial.println(a_profile);
  }
  if (a_profile == 0)
    saveProfile(375);
  else if (a_profile == 1)
    saveProfile(1084);
  else if (a_profile == 2)
    saveProfile(1793);
  else if (a_profile == 3)
    saveProfile(2502);
  else if (a_profile == 4)
    saveProfile(3211);
  delay(250);
}

void loadEepromFltArray(float* a_array, int a_len, int& a_address) {
  int buffer;
  for (uint8_t i = 0; i < a_len; i++) {
    externalEEPROM.get(a_address, buffer);
    a_array[i] = buffer / 100.0;
    a_address += sizeof(int);
  }
}

void loadEepromIntArray(float* a_array, int a_len, int& a_address) {
  int buffer;
  for (uint8_t i = 0; i < a_len; i++) {
    externalEEPROM.get(a_address, buffer);
    a_array[i] = buffer;
    a_address += sizeof(int);
  }
}

void loadProfile(int a_address) {
  if (device::globalDebug) {
    Serial.print("EEPROM starting at address: "); Serial.println(a_address);
  }
  externalEEPROM.get(a_address, device::minPh);                      a_address += sizeof(device::minPh);
  externalEEPROM.get(a_address, device::maxPh);                      a_address += sizeof(device::maxPh);
  externalEEPROM.get(a_address, device::minEc);                      a_address += sizeof(device::minEc);
  externalEEPROM.get(a_address, device::maxEc);                      a_address += sizeof(device::maxEc);
  externalEEPROM.get(a_address, device::minWaterTemp);               a_address += sizeof(device::minWaterTemp);
  externalEEPROM.get(a_address, device::maxWaterTemp);               a_address += sizeof(device::maxWaterTemp);
  externalEEPROM.get(a_address, device::minWaterLevel);              a_address += sizeof(device::minWaterLevel);
  externalEEPROM.get(a_address, device::maxWaterLevel);              a_address += sizeof(device::maxWaterLevel);
  externalEEPROM.get(a_address, device::minTds);                     a_address += sizeof(device::minTds);
  externalEEPROM.get(a_address, device::maxTds);                     a_address += sizeof(device::maxTds);
  externalEEPROM.get(a_address, device::minAirTemp);                 a_address += sizeof(device::minAirTemp);
  externalEEPROM.get(a_address, device::maxAirTemp);                 a_address += sizeof(device::maxAirTemp);
  externalEEPROM.get(a_address, device::minHumidity);                a_address += sizeof(device::minHumidity);
  externalEEPROM.get(a_address, device::maxHumidity);                a_address += sizeof(device::maxHumidity);
  externalEEPROM.get(a_address, device::minCo2);                     a_address += sizeof(device::minCo2);
  externalEEPROM.get(a_address, device::maxCo2);                     a_address += sizeof(device::maxCo2);
  externalEEPROM.get(a_address, device::minFanOneSpeed);             a_address += sizeof(device::minFanOneSpeed);
  externalEEPROM.get(a_address, device::maxFanOneSpeed);             a_address += sizeof(device::maxFanOneSpeed);
  externalEEPROM.get(a_address, device::minFanTwoSpeed);             a_address += sizeof(device::minFanTwoSpeed);
  externalEEPROM.get(a_address, device::maxFanTwoSpeed);             a_address += sizeof(device::maxFanTwoSpeed);
  externalEEPROM.get(a_address, user::lightOnTimeHour);              a_address += sizeof(user::lightOnTimeHour);
  externalEEPROM.get(a_address, user::lightOnTimeMin);               a_address += sizeof(user::lightOnTimeMin);
  externalEEPROM.get(a_address, user::lightOffTimeHour);             a_address += sizeof(user::lightOffTimeHour);
  externalEEPROM.get(a_address, user::lightOffTimeMin);              a_address += sizeof(user::lightOffTimeMin);
  externalEEPROM.get(a_address, user::lightMode);                    a_address += sizeof(user::lightMode);
  externalEEPROM.get(a_address, user::doserOneMode);                 a_address += sizeof(user::doserOneMode);
  externalEEPROM.get(a_address, user::doserTwoMode);                 a_address += sizeof(user::doserTwoMode);
  externalEEPROM.get(a_address, user::doserThreeMode);               a_address += sizeof(user::doserThreeMode);
  externalEEPROM.get(a_address, user::doserFourMode);                a_address += sizeof(user::doserFourMode);
  externalEEPROM.get(a_address, user::doserFiveMode);                a_address += sizeof(user::doserFiveMode);
  externalEEPROM.get(a_address, user::doserSixMode);                 a_address += sizeof(user::doserSixMode);
  externalEEPROM.get(a_address, user::doserOneMills);                a_address += sizeof(user::doserOneMills);
  externalEEPROM.get(a_address, user::doserTwoMills);                a_address += sizeof(user::doserTwoMills);
  externalEEPROM.get(a_address, user::doserThreeMills);              a_address += sizeof(user::doserThreeMills);
  externalEEPROM.get(a_address, user::doserFourMills);               a_address += sizeof(user::doserFourMills);
  externalEEPROM.get(a_address, user::doserFiveMills);               a_address += sizeof(user::doserFiveMills);
  externalEEPROM.get(a_address, user::doserSixMills);                a_address += sizeof(user::doserSixMills);
  externalEEPROM.get(a_address, user::targetMinTds);                 a_address += sizeof(user::targetMinTds);
  externalEEPROM.get(a_address, user::targetMaxTds);                 a_address += sizeof(user::targetMaxTds);
  externalEEPROM.get(a_address, user::targetMinPh);                  a_address += sizeof(user::targetMinPh);
  externalEEPROM.get(a_address, user::targetMaxPh);                  a_address += sizeof(user::targetMaxPh);
  externalEEPROM.get(a_address, user::targetMinEc);                  a_address += sizeof(user::targetMinEc);
  externalEEPROM.get(a_address, user::targetMaxEc);                  a_address += sizeof(user::targetMaxEc);
  externalEEPROM.get(a_address, user::targetCo2);                    a_address += sizeof(user::targetCo2);
  externalEEPROM.get(a_address, user::co2Offset);                    a_address += sizeof(user::co2Offset);
  externalEEPROM.get(a_address, user::roomWidthCm);                  a_address += sizeof(user::roomWidthCm);
  externalEEPROM.get(a_address, user::roomLengthCm);                 a_address += sizeof(user::roomLengthCm);
  externalEEPROM.get(a_address, user::roomHeightCm);                 a_address += sizeof(user::roomHeightCm);
  externalEEPROM.get(a_address, user::co2FlowrateLtrs);              a_address += sizeof(user::co2FlowrateLtrs);
  externalEEPROM.get(a_address, user::roomWidthInches);              a_address += sizeof(user::roomWidthInches);
  externalEEPROM.get(a_address, user::roomLengthInches);             a_address += sizeof(user::roomLengthInches);
  externalEEPROM.get(a_address, user::roomHeightInches);             a_address += sizeof(user::roomHeightInches);
  externalEEPROM.get(a_address, user::co2FlowrateFeet3);             a_address += sizeof(user::co2FlowrateFeet3);
  externalEEPROM.get(a_address, user::co2CheckTimeMinute);           a_address += sizeof(user::co2CheckTimeMinute);
  externalEEPROM.get(a_address, user::co2CheckTimeHour);             a_address += sizeof(user::co2CheckTimeHour);
  externalEEPROM.get(a_address, user::enableManualCo2Duration);      a_address += sizeof(user::enableManualCo2Duration);
  externalEEPROM.get(a_address, user::disableCo2Control);            a_address += sizeof(user::disableCo2Control);
  externalEEPROM.get(a_address, user::manualCo2GasDuration);         a_address += sizeof(user::manualCo2GasDuration);
  externalEEPROM.get(a_address, user::disableFansTimer);             a_address += sizeof(user::disableFansTimer);
  externalEEPROM.get(a_address, user::targetMinWaterTemp);           a_address += sizeof(user::targetMinWaterTemp);
  externalEEPROM.get(a_address, user::targetMaxWaterTemp);           a_address += sizeof(user::targetMaxWaterTemp);
  externalEEPROM.get(a_address, user::targetMinWaterHeight);         a_address += sizeof(user::targetMinWaterHeight);
  externalEEPROM.get(a_address, user::targetMaxWaterHeight);         a_address += sizeof(user::targetMaxWaterHeight);
  externalEEPROM.get(a_address, user::targetMinWaterHeightInches);   a_address += sizeof(user::targetMinWaterHeightInches);
  externalEEPROM.get(a_address, user::targetMaxWaterHeightInches);   a_address += sizeof(user::targetMaxWaterHeightInches);
  externalEEPROM.get(a_address, user::waterTankLength);              a_address += sizeof(user::waterTankLength);
  externalEEPROM.get(a_address, user::waterTankLengthInches);        a_address += sizeof(user::waterTankLengthInches);
  externalEEPROM.get(a_address, user::waterTankWidth);               a_address += sizeof(user::waterTankWidth);
  externalEEPROM.get(a_address, user::waterTankWidthInches);         a_address += sizeof(user::waterTankWidthInches);
  externalEEPROM.get(a_address, user::targetMinFanOneSpeed);         a_address += sizeof(user::targetMinFanOneSpeed);
  externalEEPROM.get(a_address, user::targetMaxFanOneSpeed);         a_address += sizeof(user::targetMaxFanOneSpeed);
  externalEEPROM.get(a_address, user::targetMinFanTwoSpeed);         a_address += sizeof(user::targetMinFanTwoSpeed);
  externalEEPROM.get(a_address, user::targetMaxFanTwoSpeed);         a_address += sizeof(user::targetMaxFanTwoSpeed);
  externalEEPROM.get(a_address, user::targetMinAirTemp);             a_address += sizeof(user::targetMinAirTemp);
  externalEEPROM.get(a_address, user::targetMaxAirTemp);             a_address += sizeof(user::targetMaxAirTemp);
  externalEEPROM.get(a_address, user::targetMinAirTempF);            a_address += sizeof(user::targetMinAirTempF);
  externalEEPROM.get(a_address, user::targetMaxAirTempF);            a_address += sizeof(user::targetMaxAirTempF);
  externalEEPROM.get(a_address, user::targetMinWaterTempF);          a_address += sizeof(user::targetMinWaterTempF);
  externalEEPROM.get(a_address, user::targetMaxWaterTempF);          a_address += sizeof(user::targetMaxWaterTempF);
  externalEEPROM.get(a_address, user::targetMinHumidity);            a_address += sizeof(user::targetMinHumidity);
  externalEEPROM.get(a_address, user::targetMaxHumidity);            a_address += sizeof(user::targetMaxHumidity);
  externalEEPROM.get(a_address, wifi::wifiEnabled);                  a_address += sizeof(wifi::wifiEnabled);
  externalEEPROM.get(a_address, user::doserOneSpeed);                a_address += sizeof(user::doserOneSpeed);
  externalEEPROM.get(a_address, user::doserTwoSpeed);                a_address += sizeof(user::doserTwoSpeed);
  externalEEPROM.get(a_address, user::doserThreeSpeed);              a_address += sizeof(user::doserThreeSpeed);
  externalEEPROM.get(a_address, user::doserFourSpeed);               a_address += sizeof(user::doserFourSpeed);
  externalEEPROM.get(a_address, user::doserFiveSpeed);               a_address += sizeof(user::doserFiveSpeed);
  externalEEPROM.get(a_address, user::doserSixSpeed);                a_address += sizeof(user::doserSixSpeed);
  externalEEPROM.get(a_address, user::dosingInterval);               a_address += sizeof(user::dosingInterval);
  externalEEPROM.get(a_address, user::refillDoserOneMills);          a_address += sizeof(user::refillDoserOneMills);
  externalEEPROM.get(a_address, user::refillDoserTwoMills);          a_address += sizeof(user::refillDoserTwoMills);
  externalEEPROM.get(a_address, user::refillDoserThreeMills);        a_address += sizeof(user::refillDoserThreeMills);
  externalEEPROM.get(a_address, user::refillDoserFourMills);         a_address += sizeof(user::refillDoserFourMills);
  externalEEPROM.get(a_address, user::refillDoserFiveMills);         a_address += sizeof(user::refillDoserFiveMills);
  externalEEPROM.get(a_address, user::refillDoserSixMills);          a_address += sizeof(user::refillDoserSixMills);
  externalEEPROM.get(a_address, user::disableDrainAndRefill);        a_address += sizeof(user::disableDrainAndRefill);
  externalEEPROM.get(a_address, user::autoFillHour);                 a_address += sizeof(user::autoFillHour);
  externalEEPROM.get(a_address, user::autoFillMinute);               a_address += sizeof(user::autoFillMinute);
  externalEEPROM.get(a_address, user::drainTimeout);                 a_address += sizeof(user::drainTimeout);
  externalEEPROM.get(a_address, user::backgroundColor);              a_address += sizeof(user::backgroundColor);
  externalEEPROM.get(a_address, user::graphInterval);                a_address += sizeof(user::graphInterval);
  externalEEPROM.get(a_address, user::afkTime);                      a_address += sizeof(user::afkTime);
  externalEEPROM.get(a_address, user::convertToF);                   a_address += sizeof(user::convertToF);
  externalEEPROM.get(a_address, user::convertToInches);              a_address += sizeof(user::convertToInches);
  externalEEPROM.get(a_address, user::convertToTds);                 a_address += sizeof(user::convertToTds);
  externalEEPROM.get(a_address, user::tdsErrorMargin);               a_address += sizeof(user::tdsErrorMargin);
  externalEEPROM.get(a_address, user::phErrorMargin);                a_address += sizeof(user::phErrorMargin);
  externalEEPROM.get(a_address, user::ecErrorMargin);                a_address += sizeof(user::ecErrorMargin);
  externalEEPROM.get(a_address, user::co2ErrorMargin);               a_address += sizeof(user::co2ErrorMargin);
  externalEEPROM.get(a_address, user::airTempErrorMargin);           a_address += sizeof(user::airTempErrorMargin);
  externalEEPROM.get(a_address, user::airTempErrorMarginF);          a_address += sizeof(user::airTempErrorMarginF);
  externalEEPROM.get(a_address, user::humidityErrorMargin);          a_address += sizeof(user::humidityErrorMargin);
  externalEEPROM.get(a_address, user::waterTempErrorMargin);         a_address += sizeof(user::waterTempErrorMargin);
  externalEEPROM.get(a_address, user::waterTempErrorMarginF);        a_address += sizeof(user::waterTempErrorMarginF);
  externalEEPROM.get(a_address, user::waterHeightErrorMargin);       a_address += sizeof(user::waterHeightErrorMargin);
  externalEEPROM.get(a_address, user::waterHeightErrorMarginInches); a_address += sizeof(user::waterHeightErrorMarginInches);
  externalEEPROM.get(a_address, user::disableAirTempWarnings);       a_address += sizeof(user::disableAirTempWarnings);
  externalEEPROM.get(a_address, user::disableHumidityWarnings);      a_address += sizeof(user::disableHumidityWarnings);
  externalEEPROM.get(a_address, user::disablePpmWarnings);           a_address += sizeof(user::disablePpmWarnings);
  externalEEPROM.get(a_address, user::disableEcWarnings);            a_address += sizeof(user::disableEcWarnings);
  externalEEPROM.get(a_address, user::disablePhWarnings);            a_address += sizeof(user::disablePhWarnings);
  externalEEPROM.get(a_address, user::disableCo2Warnings);           a_address += sizeof(user::disableCo2Warnings);
  externalEEPROM.get(a_address, user::disableWaterTempWarnings);     a_address += sizeof(user::disableWaterTempWarnings);
  externalEEPROM.get(a_address, user::disableWaterHeightWarnings);   a_address += sizeof(user::disableWaterHeightWarnings);
  externalEEPROM.get(a_address, user::disableLED);                   a_address += sizeof(user::disableLED);
  externalEEPROM.get(a_address, user::disableBeeper);                a_address += sizeof(user::disableBeeper);
  externalEEPROM.get(a_address, user::disableMachineLearning);       a_address += sizeof(user::disableMachineLearning);
  externalEEPROM.get(a_address, user::fansControlTemperature);       a_address += sizeof(user::fansControlTemperature);
  externalEEPROM.get(a_address, user::fansControlHumidity);          a_address += sizeof(user::fansControlHumidity);
  externalEEPROM.get(a_address, user::fanOneFixedSpeed);             a_address += sizeof(user::fanOneFixedSpeed);
  externalEEPROM.get(a_address, user::fanTwoFixedSpeed);             a_address += sizeof(user::fanTwoFixedSpeed);
  externalEEPROM.get(a_address, user::numberOfDosers);               a_address += sizeof(user::numberOfDosers);
  externalEEPROM.get(a_address, user::useEtapeSensor);               a_address += sizeof(user::useEtapeSensor);
  externalEEPROM.get(a_address, sensor::sensorArrayPos);              a_address += sizeof(sensor::sensorArrayPos);
  externalEEPROM.get(a_address, device::conversionType);             a_address += sizeof(device::conversionType);
  for (const bool& val : user::autoFillDays) {
    externalEEPROM.get(a_address, val);
    a_address += sizeof(val);
  }
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(a_address);
  }
}

void saveProfile(int a_address) {
  if (device::globalDebug) {
    Serial.print("EEPROM starting at address: "); Serial.println(a_address);
  }
  externalEEPROM.put(a_address, device::minPh);                      a_address += sizeof(device::minPh);
  externalEEPROM.put(a_address, device::maxPh);                      a_address += sizeof(device::maxPh);
  externalEEPROM.put(a_address, device::minEc);                      a_address += sizeof(device::minEc);
  externalEEPROM.put(a_address, device::maxEc);                      a_address += sizeof(device::maxEc);
  externalEEPROM.put(a_address, device::minWaterTemp);               a_address += sizeof(device::minWaterTemp);
  externalEEPROM.put(a_address, device::maxWaterTemp);               a_address += sizeof(device::maxWaterTemp);
  externalEEPROM.put(a_address, device::minWaterLevel);              a_address += sizeof(device::minWaterLevel);
  externalEEPROM.put(a_address, device::maxWaterLevel);              a_address += sizeof(device::maxWaterLevel);
  externalEEPROM.put(a_address, device::minTds);                     a_address += sizeof(device::minTds);
  externalEEPROM.put(a_address, device::maxTds);                     a_address += sizeof(device::maxTds);
  externalEEPROM.put(a_address, device::minAirTemp);                 a_address += sizeof(device::minAirTemp);
  externalEEPROM.put(a_address, device::maxAirTemp);                 a_address += sizeof(device::maxAirTemp);
  externalEEPROM.put(a_address, device::minHumidity);                a_address += sizeof(device::minHumidity);
  externalEEPROM.put(a_address, device::maxHumidity);                a_address += sizeof(device::maxHumidity);
  externalEEPROM.put(a_address, device::minCo2);                     a_address += sizeof(device::minCo2);
  externalEEPROM.put(a_address, device::maxCo2);                     a_address += sizeof(device::maxCo2);
  externalEEPROM.put(a_address, device::minFanOneSpeed);             a_address += sizeof(device::minFanOneSpeed);
  externalEEPROM.put(a_address, device::maxFanOneSpeed);             a_address += sizeof(device::maxFanOneSpeed);
  externalEEPROM.put(a_address, device::minFanTwoSpeed);             a_address += sizeof(device::minFanTwoSpeed);
  externalEEPROM.put(a_address, device::maxFanTwoSpeed);             a_address += sizeof(device::maxFanTwoSpeed);
  externalEEPROM.put(a_address, user::lightOnTimeHour);              a_address += sizeof(user::lightOnTimeHour);
  externalEEPROM.put(a_address, user::lightOnTimeMin);               a_address += sizeof(user::lightOnTimeMin);
  externalEEPROM.put(a_address, user::lightOffTimeHour);             a_address += sizeof(user::lightOffTimeHour);
  externalEEPROM.put(a_address, user::lightOffTimeMin);              a_address += sizeof(user::lightOffTimeMin);
  externalEEPROM.put(a_address, user::lightMode);                    a_address += sizeof(user::lightMode);
  externalEEPROM.put(a_address, user::doserOneMode);                 a_address += sizeof(user::doserOneMode);
  externalEEPROM.put(a_address, user::doserTwoMode);                 a_address += sizeof(user::doserTwoMode);
  externalEEPROM.put(a_address, user::doserThreeMode);               a_address += sizeof(user::doserThreeMode);
  externalEEPROM.put(a_address, user::doserFourMode);                a_address += sizeof(user::doserFourMode);
  externalEEPROM.put(a_address, user::doserFiveMode);                a_address += sizeof(user::doserFiveMode);
  externalEEPROM.put(a_address, user::doserSixMode);                 a_address += sizeof(user::doserSixMode);
  externalEEPROM.put(a_address, user::doserOneMills);                a_address += sizeof(user::doserOneMills);
  externalEEPROM.put(a_address, user::doserTwoMills);                a_address += sizeof(user::doserTwoMills);
  externalEEPROM.put(a_address, user::doserThreeMills);              a_address += sizeof(user::doserThreeMills);
  externalEEPROM.put(a_address, user::doserFourMills);               a_address += sizeof(user::doserFourMills);
  externalEEPROM.put(a_address, user::doserFiveMills);               a_address += sizeof(user::doserFiveMills);
  externalEEPROM.put(a_address, user::doserSixMills);                a_address += sizeof(user::doserSixMills);
  externalEEPROM.put(a_address, user::targetMinTds);                 a_address += sizeof(user::targetMinTds);
  externalEEPROM.put(a_address, user::targetMaxTds);                 a_address += sizeof(user::targetMaxTds);
  externalEEPROM.put(a_address, user::targetMinPh);                  a_address += sizeof(user::targetMinPh);
  externalEEPROM.put(a_address, user::targetMaxPh);                  a_address += sizeof(user::targetMaxPh);
  externalEEPROM.put(a_address, user::targetMinEc);                  a_address += sizeof(user::targetMinEc);
  externalEEPROM.put(a_address, user::targetMaxEc);                  a_address += sizeof(user::targetMaxEc);
  externalEEPROM.put(a_address, user::targetCo2);                    a_address += sizeof(user::targetCo2);
  externalEEPROM.put(a_address, user::co2Offset);                    a_address += sizeof(user::co2Offset);
  externalEEPROM.put(a_address, user::roomWidthCm);                  a_address += sizeof(user::roomWidthCm);
  externalEEPROM.put(a_address, user::roomLengthCm);                 a_address += sizeof(user::roomLengthCm);
  externalEEPROM.put(a_address, user::roomHeightCm);                 a_address += sizeof(user::roomHeightCm);
  externalEEPROM.put(a_address, user::co2FlowrateLtrs);              a_address += sizeof(user::co2FlowrateLtrs);
  externalEEPROM.put(a_address, user::roomWidthInches);              a_address += sizeof(user::roomWidthInches);
  externalEEPROM.put(a_address, user::roomLengthInches);             a_address += sizeof(user::roomLengthInches);
  externalEEPROM.put(a_address, user::roomHeightInches);             a_address += sizeof(user::roomHeightInches);
  externalEEPROM.put(a_address, user::co2FlowrateFeet3);             a_address += sizeof(user::co2FlowrateFeet3);
  externalEEPROM.put(a_address, user::co2CheckTimeMinute);           a_address += sizeof(user::co2CheckTimeMinute);
  externalEEPROM.put(a_address, user::co2CheckTimeHour);             a_address += sizeof(user::co2CheckTimeHour);
  externalEEPROM.put(a_address, user::enableManualCo2Duration);      a_address += sizeof(user::enableManualCo2Duration);
  externalEEPROM.put(a_address, user::disableCo2Control);            a_address += sizeof(user::disableCo2Control);
  externalEEPROM.put(a_address, user::manualCo2GasDuration);         a_address += sizeof(user::manualCo2GasDuration);
  externalEEPROM.put(a_address, user::disableFansTimer);             a_address += sizeof(user::disableFansTimer);
  externalEEPROM.put(a_address, user::targetMinWaterTemp);           a_address += sizeof(user::targetMinWaterTemp);
  externalEEPROM.put(a_address, user::targetMaxWaterTemp);           a_address += sizeof(user::targetMaxWaterTemp);
  externalEEPROM.put(a_address, user::targetMinWaterHeight);         a_address += sizeof(user::targetMinWaterHeight);
  externalEEPROM.put(a_address, user::targetMaxWaterHeight);         a_address += sizeof(user::targetMaxWaterHeight);
  externalEEPROM.put(a_address, user::targetMinWaterHeightInches);   a_address += sizeof(user::targetMinWaterHeightInches);
  externalEEPROM.put(a_address, user::targetMaxWaterHeightInches);   a_address += sizeof(user::targetMaxWaterHeightInches);
  externalEEPROM.put(a_address, user::waterTankLength);              a_address += sizeof(user::waterTankLength);
  externalEEPROM.put(a_address, user::waterTankLengthInches);        a_address += sizeof(user::waterTankLengthInches);
  externalEEPROM.put(a_address, user::waterTankWidth);               a_address += sizeof(user::waterTankWidth);
  externalEEPROM.put(a_address, user::waterTankWidthInches);         a_address += sizeof(user::waterTankWidthInches);
  externalEEPROM.put(a_address, user::targetMinFanOneSpeed);         a_address += sizeof(user::targetMinFanOneSpeed);
  externalEEPROM.put(a_address, user::targetMaxFanOneSpeed);         a_address += sizeof(user::targetMaxFanOneSpeed);
  externalEEPROM.put(a_address, user::targetMinFanTwoSpeed);         a_address += sizeof(user::targetMinFanTwoSpeed);
  externalEEPROM.put(a_address, user::targetMaxFanTwoSpeed);         a_address += sizeof(user::targetMaxFanTwoSpeed);
  externalEEPROM.put(a_address, user::targetMinAirTemp);             a_address += sizeof(user::targetMinAirTemp);
  externalEEPROM.put(a_address, user::targetMaxAirTemp);             a_address += sizeof(user::targetMaxAirTemp);
  externalEEPROM.put(a_address, user::targetMinAirTempF);            a_address += sizeof(user::targetMinAirTempF);
  externalEEPROM.put(a_address, user::targetMaxAirTempF);            a_address += sizeof(user::targetMaxAirTempF);
  externalEEPROM.put(a_address, user::targetMinWaterTempF);          a_address += sizeof(user::targetMinWaterTempF);
  externalEEPROM.put(a_address, user::targetMaxWaterTempF);          a_address += sizeof(user::targetMaxWaterTempF);
  externalEEPROM.put(a_address, user::targetMinHumidity);            a_address += sizeof(user::targetMinHumidity);
  externalEEPROM.put(a_address, user::targetMaxHumidity);            a_address += sizeof(user::targetMaxHumidity);
  externalEEPROM.put(a_address, wifi::wifiEnabled);                  a_address += sizeof(wifi::wifiEnabled);
  externalEEPROM.put(a_address, user::doserOneSpeed);                a_address += sizeof(user::doserOneSpeed);
  externalEEPROM.put(a_address, user::doserTwoSpeed);                a_address += sizeof(user::doserTwoSpeed);
  externalEEPROM.put(a_address, user::doserThreeSpeed);              a_address += sizeof(user::doserThreeSpeed);
  externalEEPROM.put(a_address, user::doserFourSpeed);               a_address += sizeof(user::doserFourSpeed);
  externalEEPROM.put(a_address, user::doserFiveSpeed);               a_address += sizeof(user::doserFiveSpeed);
  externalEEPROM.put(a_address, user::doserSixSpeed);                a_address += sizeof(user::doserSixSpeed);
  externalEEPROM.put(a_address, user::dosingInterval);               a_address += sizeof(user::dosingInterval);
  externalEEPROM.put(a_address, user::refillDoserOneMills);          a_address += sizeof(user::refillDoserOneMills);
  externalEEPROM.put(a_address, user::refillDoserTwoMills);          a_address += sizeof(user::refillDoserTwoMills);
  externalEEPROM.put(a_address, user::refillDoserThreeMills);        a_address += sizeof(user::refillDoserThreeMills);
  externalEEPROM.put(a_address, user::refillDoserFourMills);         a_address += sizeof(user::refillDoserFourMills);
  externalEEPROM.put(a_address, user::refillDoserFiveMills);         a_address += sizeof(user::refillDoserFiveMills);
  externalEEPROM.put(a_address, user::refillDoserSixMills);          a_address += sizeof(user::refillDoserSixMills);
  externalEEPROM.put(a_address, user::disableDrainAndRefill);        a_address += sizeof(user::disableDrainAndRefill);
  externalEEPROM.put(a_address, user::autoFillHour);                 a_address += sizeof(user::autoFillHour);
  externalEEPROM.put(a_address, user::autoFillMinute);               a_address += sizeof(user::autoFillMinute);
  externalEEPROM.put(a_address, user::drainTimeout);                 a_address += sizeof(user::drainTimeout);
  externalEEPROM.put(a_address, user::backgroundColor);              a_address += sizeof(user::backgroundColor);
  externalEEPROM.put(a_address, user::graphInterval);                a_address += sizeof(user::graphInterval);
  externalEEPROM.put(a_address, user::afkTime);                      a_address += sizeof(user::afkTime);
  externalEEPROM.put(a_address, user::convertToF);                   a_address += sizeof(user::convertToF);
  externalEEPROM.put(a_address, user::convertToInches);              a_address += sizeof(user::convertToInches);
  externalEEPROM.put(a_address, user::convertToTds);                 a_address += sizeof(user::convertToTds);
  externalEEPROM.put(a_address, user::tdsErrorMargin);               a_address += sizeof(user::tdsErrorMargin);
  externalEEPROM.put(a_address, user::phErrorMargin);                a_address += sizeof(user::phErrorMargin);
  externalEEPROM.put(a_address, user::ecErrorMargin);                a_address += sizeof(user::ecErrorMargin);
  externalEEPROM.put(a_address, user::co2ErrorMargin);               a_address += sizeof(user::co2ErrorMargin);
  externalEEPROM.put(a_address, user::airTempErrorMargin);           a_address += sizeof(user::airTempErrorMargin);
  externalEEPROM.put(a_address, user::airTempErrorMarginF);          a_address += sizeof(user::airTempErrorMarginF);
  externalEEPROM.put(a_address, user::humidityErrorMargin);          a_address += sizeof(user::humidityErrorMargin);
  externalEEPROM.put(a_address, user::waterTempErrorMargin);         a_address += sizeof(user::waterTempErrorMargin);
  externalEEPROM.put(a_address, user::waterTempErrorMarginF);        a_address += sizeof(user::waterTempErrorMarginF);
  externalEEPROM.put(a_address, user::waterHeightErrorMargin);       a_address += sizeof(user::waterHeightErrorMargin);
  externalEEPROM.put(a_address, user::waterHeightErrorMarginInches); a_address += sizeof(user::waterHeightErrorMarginInches);
  externalEEPROM.put(a_address, user::disableAirTempWarnings);       a_address += sizeof(user::disableAirTempWarnings);
  externalEEPROM.put(a_address, user::disableHumidityWarnings);      a_address += sizeof(user::disableHumidityWarnings);
  externalEEPROM.put(a_address, user::disablePpmWarnings);           a_address += sizeof(user::disablePpmWarnings);
  externalEEPROM.put(a_address, user::disableEcWarnings);            a_address += sizeof(user::disableEcWarnings);
  externalEEPROM.put(a_address, user::disablePhWarnings);            a_address += sizeof(user::disablePhWarnings);
  externalEEPROM.put(a_address, user::disableCo2Warnings);           a_address += sizeof(user::disableCo2Warnings);
  externalEEPROM.put(a_address, user::disableWaterTempWarnings);     a_address += sizeof(user::disableWaterTempWarnings);
  externalEEPROM.put(a_address, user::disableWaterHeightWarnings);   a_address += sizeof(user::disableWaterHeightWarnings);
  externalEEPROM.put(a_address, user::disableLED);                   a_address += sizeof(user::disableLED);
  externalEEPROM.put(a_address, user::disableBeeper);                a_address += sizeof(user::disableBeeper);
  externalEEPROM.put(a_address, user::disableMachineLearning);       a_address += sizeof(user::disableMachineLearning);
  externalEEPROM.put(a_address, user::fansControlTemperature);       a_address += sizeof(user::fansControlTemperature);
  externalEEPROM.put(a_address, user::fansControlHumidity);          a_address += sizeof(user::fansControlHumidity);
  externalEEPROM.put(a_address, user::fanOneFixedSpeed);             a_address += sizeof(user::fanOneFixedSpeed);
  externalEEPROM.put(a_address, user::fanTwoFixedSpeed);             a_address += sizeof(user::fanTwoFixedSpeed);
  externalEEPROM.put(a_address, user::numberOfDosers);               a_address += sizeof(user::numberOfDosers);
  externalEEPROM.put(a_address, user::useEtapeSensor);               a_address += sizeof(user::useEtapeSensor);
  externalEEPROM.put(a_address, sensor::sensorArrayPos);             a_address += sizeof(sensor::sensorArrayPos);
  externalEEPROM.put(a_address, device::conversionType);             a_address += sizeof(device::conversionType);
  for (const bool& val : user::autoFillDays) {
    externalEEPROM.put(a_address, val);
    a_address += sizeof(val);
  }
  if (device::globalDebug) {
    Serial.print("EEPROM ending at address: "); Serial.println(a_address);
  }
}
