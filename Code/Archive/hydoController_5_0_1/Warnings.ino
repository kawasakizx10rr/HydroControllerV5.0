// This function works out if there is a warning to display
// based on the sensor values compared against the users target
// setpoints +/- offset +/- errorMargin
void displayWarnings() {
  device::globalErrorState = device::NO_WARNING;
  static uint8_t previousErrorState = 0;
  static unsigned long prevWarnMillis = millis();
  if (device::sensorsReady && millis() - prevWarnMillis >= 1000UL) {
    setErrorState(sensor::ec, user::targetMinEc, user::targetMaxEc, user::ecErrorMargin, user::disableEcWarnings, device::ecErrorState);
    setErrorState(sensor::tds, user::targetMinTds, user::targetMaxTds, user::tdsErrorMargin, user::disableEcWarnings, device::ecErrorState);
    setErrorState(sensor::ph, user::targetMinPh, user::targetMaxPh, user::phErrorMargin, user::disablePhWarnings, device::phErrorState);
    if (!user::convertToF)
      setErrorState(sensor::waterTemp, user::targetMinWaterTemp, user::targetMaxWaterTemp, user::waterTempErrorMargin, user::disableWaterTempWarnings, device::waterTempErrorState);
    else
      setErrorState(convertToF(sensor::waterTemp), user::targetMinWaterTempF, user::targetMaxWaterTempF, user::waterTempErrorMarginF, user::disableWaterTempWarnings, device::waterTempErrorState);
    if (!user::convertToInches)
      setErrorState(sensor::waterLevel, user::targetMinWaterHeight, user::targetMaxWaterHeight, user::waterHeightErrorMargin, user::disableWaterHeightWarnings, device::waterLevelErrorState);
    else
      setErrorState(sensor::waterLevelInches, user::targetMinWaterHeightInches, user::targetMaxWaterHeightInches, user::waterHeightErrorMarginInches, user::disableWaterHeightWarnings, device::waterLevelErrorState);
    if (!user::convertToF)
      setErrorState(sensor::airTemp, user::targetMinAirTemp, user::targetMaxAirTemp, user::airTempErrorMargin, user::disableAirTempWarnings, device::airTempErrorState);
    else
      setErrorState(convertToF(sensor::airTemp), user::targetMinAirTempF, user::targetMaxAirTempF, user::airTempErrorMarginF, user::disableAirTempWarnings, device::airTempErrorState);
    setErrorState(sensor::humidity, user::targetMinHumidity, user::targetMaxHumidity, user::humidityErrorMargin, user::disableHumidityWarnings, device::humidityErrorState);
    //
    if (sensor::co2 <= (user::targetCo2 - user::co2Offset - user::co2ErrorMargin)) {
      if (!user::disableCo2Warnings)
        device::globalErrorState = device::MAJOR_WARNING;
      device::co2ErrorState = device::MAJOR_WARNING;
    }
    else if (sensor::co2 < user::targetCo2) {
      if (!user::disableCo2Warnings && device::globalErrorState != device::MAJOR_WARNING)
        device::globalErrorState = device::MINOR_WARNING;
      device::co2ErrorState = device::MINOR_WARNING;
    }
    else {
      device::co2ErrorState = device::NO_WARNING;
    }
    setAlarm();
    prevWarnMillis = millis();
  }
}

void setErrorState(const float& a_sensor, const float& a_targetMin, const float& a_targetMax, const float& a_errorMargin, const float& a_disableWarning, uint8_t& a_userErrorState) {
  if (a_sensor <= (a_targetMin - a_errorMargin) || a_sensor >= (a_targetMax + a_errorMargin)) {
    if (!a_disableWarning)
      device::globalErrorState = device::MAJOR_WARNING;
    a_userErrorState = device::MAJOR_WARNING;
  }
  else if (a_sensor < a_targetMin || a_sensor > a_targetMax) {
    if (!a_disableWarning && device::globalErrorState != device::MAJOR_WARNING)
      device::globalErrorState = device::MINOR_WARNING;
    a_userErrorState = device::MINOR_WARNING;
  }
  else {
    a_userErrorState = device::NO_WARNING;
  }
}

void setAlarm() {
  static uint8_t alarmCounter = 0;
  static uint8_t previousAlarmState = -1;
  // no alarm
  if (device::globalErrorState == device::NO_WARNING && previousAlarmState != device::NO_WARNING) {
    analogWriteChnl(pca9685Channel::speaker, 0);
    setRgbLed(0, 0, 4096);
  }
  // minor alarm (3 seconds off, 1 second on)
  else if (device::globalErrorState == device::MINOR_WARNING) {
    if (alarmCounter == 0) { // on
      analogWriteChnl(pca9685Channel::ledRed, 4096);
      setRgbLed(4096, 4096, 0); // YELLOW
    }
    else if (alarmCounter == 1) { // off after 1 second
      analogWriteChnl(pca9685Channel::ledBlue, 0);
      setRgbLed(0, 0, 0);
    }
    else if (alarmCounter >= 4) { // reset counter after 3 more seconds
      alarmCounter = 0;
    }
  }
  // major alarm (1 second off, 1 second on)
  else if (device::globalErrorState == device::MAJOR_WARNING) {
    if (alarmCounter == 0) { // on
      analogWriteChnl(pca9685Channel::ledRed, 4096);
      setRgbLed(4096, 0, 0); // RED
    }
    else if (alarmCounter == 1) { // off after 1 second
      analogWriteChnl(pca9685Channel::ledBlue, 0);
      setRgbLed(0, 0, 0);
    }
    else if (alarmCounter >= 2) { // reset counter after 1 more second
      alarmCounter = 0;
    }
  }
  // 
  if (previousAlarmState != device::globalErrorState) {
    alarmCounter = 0;
    previousAlarmState = device::globalErrorState;
  }
  alarmCounter++;
}