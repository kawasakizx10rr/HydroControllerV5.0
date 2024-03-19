void initializeDevice() {
  Wire.begin();
  if (device::globalDebug) {
    Serial.print(F("SPI max speed: ")); Serial.print(MAXSPISPEED / 1000000UL); Serial.println(F("Mhz"));
  }
  tft.begin(RA8875_800x480);
  tft.clearMemory();
  tft.touchBegin();
  tft.touchEnable(true);
  tft.backlight(true);
  tft.displayOn(true);
  tft.brightness(user::brightness);
  //if (device::globalDebug) { // Always show this message so we know if a crash/reset has happended
    Serial.print(F("Hydro Master v")); Serial.print(device::versionNumber); Serial.println(F(" started..."));
  //}
  tft.fillWindow(RA8875_BLACK);
  tft.setFont(&akashi_36px_Regular);
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.setCursor(120, 200);
  tft.print(F("Hydro master"));
  tft.setFontScale(1);
  tft.setCursor(120, 276);
  tft.print(F("version ")); tft.print(device::versionNumber);
  int16_t startX = 305;
  //
  tft.setFont(&Arial_22px_Regular);
  tft.print(120, 370, F("Initilizing device..."));
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
  tft.print(120, 370, F("Loading EEPROM..."));
  delay(200);
  externalEEPROM.setMemoryType(256);
  if (!externalEEPROM.begin()) {
   tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
   tft.setTextColor(RA8875_RED, RA8875_BLACK);
   tft.print(120, 370, F("Failed to load EEPROM"));
   while(true){};
  }
  initializeEEPROM();
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, F("Loading DS3231..."));
  delay(200);
  while (!i2cScanner(URTCLIB_ADDRESS)) {
    tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, F("Error: Failed to load DS3231"));
    delay(200);
  }
  //
  tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, F("Loading DS18B20..."));
  delay(200);
  while (!dallasTemperature.begin()) {
    tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, F("Error: Failed to load DS18B20"));
    delay(200);
  }
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  //ads.setGain(GAIN_ONE); 
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, F("Loading ADS1015..."));
  delay(200);
  if(!ads.begin()) {
      tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
      tft.setTextColor(RA8875_RED, RA8875_BLACK);
      tft.print(120, 370, F("Failed to load ADS1015"));
      while(true){};
  }
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, F("Loading PCA9685..."));
  delay(200);
  if (!pwm.begin()) {
      tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
      tft.setTextColor(RA8875_RED, RA8875_BLACK);
      tft.print(120, 370, F("Failed to load PCA9685"));
      while(true){};
  }
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, F("Loading VL53L0X..."));
  delay(200);
  if (!lox.begin()) { // while
    device::disableVL53L0X = true;
    tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_YELLOW, RA8875_BLACK);
    tft.print(120, 370, F("Warning: Failed to load VL53L0X"));
    delay(200);
  }
  //
  tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, F("Loading 328P..."));
  delay(200);
  while (!i2cScanner(device::unoAddress)) {
    tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, F("Error: Failed to load 328P"));
    delay(200);
  }
  //
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.fillRect(120, 370, 679, 24, RA8875_BLACK);
  tft.print(120, 370, F("Loading sensors..."));
  //
  //pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  analogWriteChnl(pca9685Channel::airHeater, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::co2Solenoid, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 4096 : 0));
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;

  rtc.refresh();
  // If light is set to auto mode check timer
  if (user::lightState == device::AUTO_TIMER) {
    if (user::lightMode == 0 || (user::lightMode == 1 && restartTimer(user::lightOnTimeMin, user::lightOnTimeHour, user::lightOffTimeMin, user::lightOffTimeHour))) {
      analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
      device::lightOn = true;
      if (device::globalDebug)
        Serial.println(F("Auto started light on boot"));
    }
  }
  // If aux 1 relay is set to auto mode check timer
  if (user::auxRelayOneState == device::AUTO_TIMER) {
    if (user::auxRelayOneMode == 0 || (user::auxRelayOneMode == 1 && restartTimer(user::auxRelayOneOnTimeMin, user::auxRelayOneOnTimeHour, user::auxRelayOneOffTimeMin, user::auxRelayOneOffTimeHour))) {
      digitalWrite(pin::auxRelayOne, !device::relayOffState);
      device::auxRelayOneOn = true;
      if (device::globalDebug)
        Serial.println(F("Auto started aux 1 relay on boot"));
    }
  }
  // If light is set to auto mode check timer
  if (user::auxRelayTwoState == device::AUTO_TIMER) {
    if (user::auxRelayTwoMode == 0 || (user::auxRelayTwoMode == 1 && restartTimer(user::auxRelayTwoOnTimeMin, user::auxRelayTwoOnTimeHour, user::auxRelayTwoOffTimeMin, user::auxRelayTwoOffTimeHour))) {
      digitalWrite(pin::auxRelayTwo, !device::relayOffState);
      device::auxRelayTwoOn = true;
      if (device::globalDebug)
        Serial.println(F("Auto started aux 2 relay on boot"));
    }
  }

  ph.begin(sensor::phAcidicVoltage, sensor::phNeutralVoltage);

  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setKvalueLow(sensor::tdsKvalueLow);
  gravityTds.setKvalueHigh(sensor::tdsKvalueHigh);
  if (device::conversionType == device::EU) // European 1 ms/cm (EC 1.0 or CF 10) = 640 ppm
    gravityTds.setTdsFactor(0.64);  // 0.5 for EU, 0.7 for AU and 0.8 for US
  else if (device::conversionType == device::US) // USA 1 ms/cm (EC 1.0 or CF 10) = 500 ppm
    gravityTds.setTdsFactor(0.5);  // 0.5 for EU, 0.7 for AU and 0.8 for US
  else if (device::conversionType == device::AU) // Australian 1 ms/cm (EC 1.0 or CF 10) = 700 ppm
    gravityTds.setTdsFactor(0.7);  // 0.5 for EU, 0.7 for AU and 0.8 for US
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;

  device::previousDosingMinute = rtc.minute();
  device::previousDosingHour = rtc.hour();

  device::prevLightMinute = rtc.minute();
  device::prevAuxRelayOneMinute = rtc.minute();
  device::prevAuxRelayTwoMinute = rtc.minute();

  // Start the AC fans
  if (user::fanOneEnabled)
    device::fanOneJustStarted = 1;
  device::fanOneSpeed = user::targetMinFanOneSpeed;
  if (user::fanTwoEnabled)
    device::fanTwoJustStarted = 1;
  device::fanTwoSpeed = user::targetMinFanTwoSpeed;
  setFanSpeeds(device::fanOneSpeed, device::fanTwoSpeed);

  // HTML call back functions from user interactions
  server.on("/", handleRoot);
  server.on("/getPageData", getPageData);
  server.on("/setValue", setValue);

  // save log message, system started
  saveLogMessage(0);
  
  tft.fillWindow(user::backgroundColor);
  frame();
  OuterMenuIcons();
  tft.setFont(&akashi_36px_Regular);
  drawPages();
  tft.setTextColor(RA8875_WHITE, user::backgroundColor);
  tft.setTouchCalibration(device::calMinX, device::calMinY, device::calMaxX, device::calMaxY);
  display::lastTouchX = tft.width();
}

void analogWriteChnl(const uint8_t& a_channel, const uint16_t& a_dutyCycle) {
  //if (device::globalDebug) {
  //  Serial.print(F("Channel ")); Serial.print(a_channel); Serial.print(F(" set to "));
  //}
  if(a_dutyCycle == 0) {
    //if (device::globalDebug)
      //Serial.println(F("OFF"));
    pwm.setPWM(a_channel, 0, 4096);        // turns a_channel fully off
  }
  else if(a_dutyCycle == 4096) {
    //if (device::globalDebug)
    //  Serial.println(F("ON"));
    pwm.setPWM(a_channel, 4096, 0);        // turns a_channel fully on
  }
  else {
    //if (device::globalDebug)
    //  Serial.println(a_dutyCycle);
    pwm.setPWM(a_channel, 0, a_dutyCycle); // sets a_channel to a_dutyCycle
  }
}

int16_t analogReadChnl(const uint8_t& a_channel) {
  int16_t adc = ads.readADC_SingleEnded(a_channel);
  return adc;
}

float analogReadChnlToVolts(const uint8_t& a_channel) {
  int16_t adc = ads.readADC_SingleEnded(a_channel);
  float volts = ads.computeVolts(adc);
  return volts;
}

void setFanSpeeds(const byte a_fanOneSpeed, const byte a_fanTwoSpeed) {
  // i2c transmission the 328p
  Wire.beginTransmission(device::unoAddress);
  Wire.write(a_fanOneSpeed);
  Wire.write(a_fanTwoSpeed);
  Wire.endTransmission();
}

// called in while loops
void timedEvents() {
  static uint32_t previousMillis = millis();
  if (millis() - previousMillis >= 1000UL) {
    rtc.refresh();
    relayTimers();
    serialDebugger();
    previousMillis = millis();
  }  
}

void serialDebugger() {
  char str[16], charCnt = 0;
  char buffer[16] {0};
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '=') {
      strcpy(str, buffer);
      memset(buffer, 0 , 16);
      charCnt = 0;
    }
    else if (c == '\r' || c == '\n' || c == '\0') {
      setDebugCmd(str, buffer);
      charCnt = 0;
    }   
    else if (c >= 32 && c <= 126 && charCnt < 16) {
      buffer[charCnt++] = c;
    }
  }
}

void setDebugCmd(const char* a_str, const char* a_buffer) {
  if (strcmp(a_str, "debug") == 0)  {  
    device::globalDebug = atoi(a_buffer);
    Serial.print(F("Setting debug to ")); Serial.println(device::globalDebug ? F("true") : F("false"));
  }
  else {
    Serial.print(F("Unknown cmd: ")); Serial.print(a_str); Serial.print(F("=")); Serial.println(a_buffer);
  }
}

void setTimer(uint8_t& a_min, uint8_t& a_hour, const uint16_t& a_interalInMinutes) {
  uint16_t hours = a_interalInMinutes / 60;
  uint16_t minutes = a_interalInMinutes % 60;
  if (rtc.minute() + minutes < 60)
    a_min = rtc.minute() + minutes;
  else {
    a_min = (rtc.minute() + minutes) - 60;
    hours++;
  }
  if (rtc.hour() + hours < 24)
    a_hour = rtc.hour() + hours;
  else
    a_hour = (rtc.hour() + hours) - 24;
  if (device::globalDebug) {
    Serial.print(F("Fan will be turn back on at ")); Serial.print(a_hour); Serial.print(F(":")); Serial.print(a_min); Serial.print(F(":")); Serial.println(F(":00"));
  } 
}

void setRgbLed(const int a_red, const int a_green, const int a_blue) {
  if (!user::disableLED) {
    if (device::globalDebug) {
      Serial.print(F("RGB LED: ")); Serial.print(a_red); Serial.print(F(",")); Serial.print(a_green); Serial.print(F(",")); Serial.println(a_blue);
    }
    analogWriteChnl(pca9685Channel::ledRed, a_red);
    analogWriteChnl(pca9685Channel::ledBlue, a_green);
    analogWriteChnl(pca9685Channel::ledGreen, a_blue);
  }
  else {
    if (device::globalDebug)
      Serial.println(F("RGB LED disabled"));
    analogWriteChnl(pca9685Channel::ledRed, 0);
    analogWriteChnl(pca9685Channel::ledBlue, 0);
    analogWriteChnl(pca9685Channel::ledGreen, 0);
  }
}

void beep() {
  if (!user::disableBeeper) {
    analogWriteChnl(pca9685Channel::speaker, 4096);
    delay(60);
    analogWriteChnl(pca9685Channel::speaker, 0);
  }
}

// Adjust any (uint8_t,int16_t,float or int32_t) value by x increment within the range of min and max
float adjustValue(float a_val, float a_increment, const float& a_min, const float& a_max) {
  beep();
  if (display::lastTouchMillis - display::touchStartMillis > 5000UL)
    display::debounceTime = 0;

  if (a_increment < 0) {
    if (a_val >= (a_min + abs(a_increment)))
      a_val += a_increment;
    if (a_val < a_min)
      a_val = a_min;
  }    
  else if (a_increment > 0) {
    if (a_val <= (a_max - a_increment))
      a_val += a_increment;
    if (a_val > a_max)
      a_val = a_max;
  }
  device::settingsAdjusted = true;
  if (device::globalDebug) {
    Serial.print(F("Adjust value = ")); Serial.println(a_val, 2);
  }
  return a_val;
}

uint32_t adjustInterval(uint32_t a_val, const int16_t a_increment) {
  beep();
  if (display::lastTouchMillis - display::touchStartMillis > 5000UL)
    display::debounceTime = 0;
  if (a_increment < 0) {
    if (a_val > 60)
        a_val -= 60;
    else if (a_val > 0)
      a_val--;
  }    
  else if (a_increment > 0) {
    if (a_val < 60)
        a_val++;
    else
      a_val += 60;
  }
  if (device::globalDebug) {
    Serial.print(F("Adjust Interval = ")); Serial.println(a_val);
  }
  return a_val;
}

// Reset the min max values
void clearMaxMins() {
  using namespace device;
  minPh = 0;
  maxPh = 0;
  minEc = 0;
  maxEc = 0;
  minWaterTemp = 0;
  maxWaterTemp = 0;
  minWaterLevel = 0;
  maxWaterLevel = 0;
  minTds = 0;
  maxTds = 0;
  minCo2 = 0;
  maxCo2 = 0;
  minAirTemp = 0;
  maxAirTemp = 0;
  minHumidity = 0;
  maxHumidity = 0;
  minFanOneSpeed = 0;
  maxFanOneSpeed = 0;
  minFanTwoSpeed = 0;
  maxFanTwoSpeed = 0;
}

// this function flashes the user defined date time to the RTC
void adjustrtc() {
  if (device::globalDebug)
    Serial.println(F("Setting RTC rtc"));
  // Day-of-Week: monday = 1, sunday = 7
  rtc.set(0, user::rtcMinute, user::rtcHour, user::rtcDayOfWeek + 1, user::rtcDay, user::rtcMonth, user::rtcYear);
  delay(100);
  rtc.refresh();
}

// prime a doser at a given speed in PWM 0-255
void prime(const uint8_t& a_doser, const uint8_t& a_pin, const int16_t& a_speed) {
  static uint8_t previousDoser = 0;
  static int16_t currentDosingAmount = 0;
  static uint32_t primeRunTime = 0;
  device::primeTouchTime = millis();
  if (a_speed != 0) {
    if (a_doser != previousDoser) {
      if (device::globalDebug) {
        Serial.print(F("Priming doser: ")); Serial.print(a_doser); Serial.print(F(" at PWM: ")); Serial.println(a_speed);
      }
      analogWrite(a_pin, a_speed);
      bitWrite(device::doserIsPriming, a_doser-1 , true);
      currentDosingAmount = 0;
      primeRunTime = millis();
      previousDoser = a_doser;
    }
    if (millis() - primeRunTime >= 1000UL) {
      primeRunTime = millis();
      ++currentDosingAmount;
      if (device::globalDebug) {
        Serial.print(F("Primed ")); Serial.print(currentDosingAmount); Serial.println(F(" mls"));
      }
      beep();
    }
  }
  else { // If not priming turn all off dosers, just to be safe
    if (device::globalDebug) {
      Serial.print(F("Stopping doser ")); Serial.println(a_doser);
    }
    analogWrite(a_pin, 0);
    bitWrite(device::doserIsPriming, a_doser-1 , false);
    previousDoser = 0;
    currentDosingAmount = 0;
    primeRunTime = 0;
  }
}

// Save a log message to the array and push back the array when full
void saveLogMessage(const uint8_t& a_logType) {
  rtc.refresh();
  const char* t_mon = rtc.getMonthStr();
  const char* t_time = rtc.getTimeStr();
  char t_day[8];
  itoa(rtc.day(), t_day, 10);
  // Push back log array if full
  int arraypos = message::systemLogPos;
  if (message::systemLogPos == message::maxLogs) {
    if (device::globalDebug)
      Serial.println(F("Pushing logs back..."));
    for (uint8_t i = 0; i < message::maxLogs - 1; i++) {
      // push back log type
      message::logTypeArray[i] = message::logTypeArray[i + 1];
      // copy log i + 1 to log i
      for (uint8_t n = 0; n < message::maxCharsPerLog; n++) {
        message::timeStrArray[i][n] = message::timeStrArray[i + 1][n];
      }
    }
    arraypos = message::maxLogs - 1;
  }
  // Add the log message type
  message::logTypeArray[arraypos] = a_logType;
  // Clear the last block
  memset(message::timeStrArray[arraypos], 0, message::maxCharsPerLog);
  // Add the day
  if (atoi(t_day) < 10)
    strcpy(message::timeStrArray[arraypos], "0");
  strcpy(message::timeStrArray[arraypos], t_day);
  strcat(message::timeStrArray[arraypos], " ");
  /// add month
  strcat(message::timeStrArray[arraypos], t_mon);
  strcat(message::timeStrArray[arraypos], " ");
  // Add the time
  strcat(message::timeStrArray[arraypos], t_time);
  if (device::globalDebug) {
    Serial.print(F("Added new log type [")); Serial.print(a_logType); Serial.print(F("] "));
    Serial.print(F("at position ")); Serial.print(arraypos); Serial.print(F(", LOG: "));
    Serial.print(message::notificationsArray[a_logType]);
    Serial.print(F(" ")); Serial.println(message::timeStrArray[arraypos]);
  }
  if (message::systemLogPos < message::maxLogs) 
    message::systemLogPos++;
}

// Convert a float to int16_t, with a precison of 2 decimal places
int16_t fltToInt(const float& a_value) {
  int16_t val = a_value * 100;
  return val;
}

// Convert a float to int16_t, with a precison of 3 decimal places
int16_t doubleToInt(const float& a_value) {
  int16_t val = a_value * 1000;
  return val;
}

// convert a celsius value to ferinheight
float convertToF(const float& a_value) {
  return (a_value * 1.8) + 32;
}

// convert a centimeter value to inches
float convertToInches(const float& a_value) {
  return a_value / 2.54;
}

// when the graph arrays are full push back the array values by 1
void pushBackDataArrays() {
  static bool graphFirstSave = true;
  static uint32_t graphPreviousMillis = 0;
  if ((millis() - graphPreviousMillis >= user::graphInterval * 1000UL) || graphFirstSave) {
    if (sensor::sensorArrayPos < sensor::maxSensorArrayVals) {
      updateGraphArrays(sensor::sensorArrayPos);
      sensor::sensorArrayPos++;
    }
    else {
      for (uint8_t i = 0; i < sensor::maxSensorArrayVals - 1; i++) {
        sensor::phArray[i] = sensor::phArray[i + 1];
        sensor::ecArray[i] = sensor::ecArray[i + 1];
        sensor::tdsArray[i] = sensor::tdsArray[i + 1];
        sensor::co2Array[i] = sensor::co2Array[i + 1];
        sensor::waterTemperatureArray[i] = sensor::waterTemperatureArray[i + 1];
        sensor::waterTemperatureArrayF[i] = sensor::waterTemperatureArrayF[i + 1];
        sensor::waterLevelArray[i] = sensor::waterLevelArray[i + 1];
        sensor::waterLevelArrayInInches[i] = sensor::waterLevelArrayInInches[i + 1];
        sensor::airTemperatureArray[i] = sensor::airTemperatureArray[i + 1];
        sensor::airTemperatureArrayF[i] = sensor::airTemperatureArrayF[i + 1];
        sensor::humidityArray[i] = sensor::humidityArray[i + 1];
        sensor::fanOneSpeedArray[i] = sensor::fanOneSpeedArray[i + 1];
        sensor::fanTwoSpeedArray[i] = sensor::fanTwoSpeedArray[i + 1];
      }
      updateGraphArrays(sensor::maxSensorArrayVals - 1);
    }
    graphFirstSave = false;
    device::newGraphData = true;
    graphPreviousMillis = millis();
  }
  //
  if (sensor::fanArrayPos == sensor::maxFanArrayVals) {
    for (int16_t i = 0; i < sensor::maxFanArrayVals - 1; i++) {
      sensor::fanTemperatureArray[i] = sensor::fanTemperatureArray[i + 1];
      sensor::fanHumidityArray[i] = sensor::fanHumidityArray[i + 1];
    }
    sensor::fanTemperatureArray[sensor::fanArrayPos -1] = (uint16_t)(sensor::airTemp * 100);
    sensor::fanHumidityArray[sensor::fanArrayPos - 1] = (uint16_t)(sensor::humidity * 100);
  }
  else {
    sensor::fanTemperatureArray[sensor::fanArrayPos] = (uint16_t)(sensor::airTemp * 100);
    sensor::fanHumidityArray[sensor::fanArrayPos] = (uint16_t)(sensor::humidity * 100);
    sensor::fanArrayPos++;
  }
}

// update the graph values at x position
void updateGraphArrays(const uint8_t& a_arrayPosition) {
  sensor::phArray[a_arrayPosition] = sensor::ph;
  sensor::ecArray[a_arrayPosition] = sensor::ec;
  sensor::tdsArray[a_arrayPosition] = sensor::tds;
  sensor::co2Array[a_arrayPosition] = sensor::co2;
  sensor::waterTemperatureArray[a_arrayPosition] = sensor::waterTemp;
  sensor::waterTemperatureArrayF[a_arrayPosition] = convertToF(sensor::waterTemp);
  sensor::waterLevelArray[a_arrayPosition] = sensor::waterLevel;
  sensor::waterLevelArrayInInches[a_arrayPosition] = convertToInches( sensor::waterLevel);
  sensor::airTemperatureArray[a_arrayPosition] = sensor::airTemp;
  sensor::airTemperatureArrayF[a_arrayPosition] = convertToF(sensor::airTemp);
  sensor::humidityArray[a_arrayPosition] = sensor::humidity;
  sensor::fanOneSpeedArray[a_arrayPosition] = device::fanOneSpeed;
  sensor::fanTwoSpeedArray[a_arrayPosition] = device::fanTwoSpeed;
}

// Set the min max's, note both min and max could be higher than current max and lower than min at boot, so uses if if
void setMaxMins() {
  // ph max mins
  if (sensor::ph < device::minPh || device::minPh == 0)
    device::minPh = sensor::ph;
  if (sensor::ph > device::maxPh || device::maxPh == 0)
    device::maxPh = sensor::ph;
  // ec max mins
  if (sensor::ec < device::minEc || device::minEc == 0)
    device::minEc = sensor::ec;
  if (sensor::ec > device::maxEc || device::maxEc == 0)
    device::maxEc = sensor::ec;
  // water temp max mins
  if (sensor::waterTemp < device::minWaterTemp || device::minWaterTemp == 0)
    device::minWaterTemp = sensor::waterTemp;
  if (sensor::waterTemp > device::maxWaterTemp || device::maxWaterTemp == 0)
    device::maxWaterTemp = sensor::waterTemp;
  // water level max mins
  if (sensor::waterLevel < device::minWaterLevel || device::minWaterLevel == 0)
    device::minWaterLevel = sensor::waterLevel;
  if (sensor::waterLevel > device::maxWaterLevel || device::maxWaterLevel == 0)
    device::maxWaterLevel = sensor::waterLevel;
  // ppm / tds max mins
  if (sensor::tds < device::minTds || device::minTds == 0)
    device::minTds = sensor::tds;
  if (sensor::tds > device::maxTds || device::maxTds == 0)
    device::maxTds = sensor::tds;
  // co2 max mins
  if (sensor::co2 < device::minCo2 || device::minCo2 == 0)
    device::minCo2 = sensor::co2;
  if (sensor::co2 > device::maxCo2 || device::maxCo2 == 0)
    device::maxCo2 = sensor::co2;
  // air temp max mins
  if (sensor::airTemp < device::minAirTemp || device::minAirTemp == 0)
    device::minAirTemp = sensor::airTemp;
  if (sensor::airTemp > device::maxAirTemp || device::maxAirTemp == 0)
    device::maxAirTemp = sensor::airTemp;
  // humidity max mins
  if (sensor::humidity < device::minHumidity || device::minHumidity == 0)
    device::minHumidity = sensor::humidity;
  if (sensor::humidity > device::maxHumidity || device::maxHumidity  == 0)
    device::maxHumidity = sensor::humidity;
  // fan one max mins
  if (device::fanOneSpeed < device::minFanOneSpeed || device::minFanOneSpeed == 0)
    device::minFanOneSpeed = device::fanOneSpeed;
  if (device::fanOneSpeed > device::maxFanOneSpeed || device::maxFanOneSpeed == 0)
    device::maxFanOneSpeed = device::fanOneSpeed;
  // fan two max mins
  if (device::fanTwoSpeed < device::minFanTwoSpeed || device::minFanTwoSpeed == 0)
    device::minFanTwoSpeed = device::fanTwoSpeed;
  if (device::fanTwoSpeed > device::maxFanTwoSpeed || device::maxFanTwoSpeed  == 0)
    device::maxFanTwoSpeed = device::fanTwoSpeed;
}

// Average any float array
float averageFltArray(const float* a_array, const int16_t a_length) {
  float rtnVal = 0;
  for (int16_t i = 0; i < a_length; i++)
    rtnVal += a_array[i];
  return rtnVal / a_length;
} 

// Clear all system logs
void clearSystemLogs() {
  message::systemLogPos = 0;
  for (uint8_t i = 0; i < message::maxLogs; i++) {
    message::logTypeArray[i] = 0;
    for (uint8_t n = 0; n < message::maxCharsPerLog; n++) {
      message::timeStrArray[i][n] = 0;
    }
  }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  if (in_min == in_max || out_min == out_max)
    return out_max;
  else if (x < in_min)
    return out_min;
  else if (x > in_max)
    return out_max;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool isEqual(const float a_val, const float a_val2, const float a_epsilon) {
  return fabs(a_val - a_val2) < a_epsilon;
}

bool hasChanged(const float a_val, const float a_val2, const int16_t a_precision) {
  uint32_t val = a_precision != 0 ? a_val * pow(10, a_precision) : a_val;
  uint32_t val2 = a_precision != 0 ? a_val2 * pow(10, a_precision) : a_val2;
  if (val != val2) {
    return true;
  }
  return false;
}

void addCharToStr(char* a_charPtr, const int16_t a_arrayLen, const char a_char) {
  for (int16_t i = 0; i < a_arrayLen; i++) {
    if (tft.getStringWidth(a_charPtr) + tft.getStringWidth(a_char) > 336) {
      if (device::intputPosition > 0)
        device::intputPosition--;
      a_charPtr[device::intputPosition] = 0;
    }
    else {
      break;
    }
  }
  a_charPtr[device::intputPosition] = a_char;
  if (device::intputPosition < a_arrayLen - 2)
    device::intputPosition++;
  device::updateKeyboardInput = true;
}

float convertEcToTds(const float a_ec) {
  float rtnVal = 0;
  if (device::conversionType == device::EU) // European 1 ms/cm (EC 1.0 or CF 10) = 640 ppm
    rtnVal = a_ec * 640.0;
  else if (device::conversionType == device::US) // USA 1 ms/cm (EC 1.0 or CF 10) = 500 ppm
    rtnVal = a_ec * 500.0;
  else if (device::conversionType == device::AU) // Australian 1 ms/cm (EC 1.0 or CF 10) = 700 ppm
    rtnVal = a_ec * 700.0;     
  return rtnVal;
}

float convertTdsToEc(const float a_tds) {
  float rtnVal = 0;
  if (device::conversionType == device::EU) // European 1 ms/cm (EC 1.0 or CF 10) = 640 ppm
    rtnVal = a_tds / 640.0;
  else if (device::conversionType == device::US) // USA 1 ms/cm (EC 1.0 or CF 10) = 500 ppm
    rtnVal = a_tds / 500.0;
  else if (device::conversionType == device::AU) // Australian 1 ms/cm (EC 1.0 or CF 10) = 700 ppm
    rtnVal = a_tds / 700.0;     
  return rtnVal;
}

float convertGallonsToLtrs(const float a_gallons) {
  return a_gallons / 3.78541;
}

float lessThan(const float a_val, const float a_target, const int16_t a_precision) {
  uint32_t val = a_val * pow(10, a_precision);
  uint32_t target = a_target * pow(10, a_precision);
  if (hasChanged(a_val, a_target, a_precision) && val < target)
    return true;
  return false;
}

float greaterThan(const float a_val, const float a_target, const int16_t a_precision) {
  uint32_t val = a_val * pow(10, a_precision);
  uint32_t target = a_target * pow(10, a_precision);
  if (hasChanged(a_val, a_target, a_precision) && val > target)
    return true;
  return false;
}

void adjustFanMode(const float a_sensor, device::controlOptions& a_fanOption, const sensor::airStates a_state, const char* a_str, const float a_percent, const float a_minTarget, const float a_maxTarget, const bool a_debug) {
  if (a_debug) {
    Serial.print(F("The ")); Serial.print(a_str); Serial.print(F(" ")); Serial.print(a_sensor, 2); Serial.print(F(" is "));
  }
  if (greaterThan(a_sensor, a_maxTarget, 2) && a_state == sensor::IS_FALLING) {
    if (a_debug) {
      Serial.print(F("greather than the max target ")); Serial.print(a_maxTarget, 2); Serial.print(F(", by ")); Serial.print(a_percent); Serial.print(F("% but the ")); Serial.print(a_str); Serial.print(F(" is falling"));
    }
    a_fanOption = device::SPEED_IDLE;
  }
  else if (greaterThan(a_sensor, a_maxTarget, 2) && a_state == sensor::IS_RISING) {
    if (a_debug) {
      Serial.print(F("greather than the max target ")); Serial.print(a_maxTarget, 2); Serial.print(F(", by ")); Serial.print(a_percent); Serial.print(F("% but the ")); Serial.print(a_str); Serial.print(F(" is rising"));
    }
    a_fanOption = device::SPEED_UP; //device::SPEED_MAX;
  }
  else if (greaterThan(a_sensor, a_maxTarget, 2) && a_state == sensor::IS_SAME) {
    if (a_debug) {
      Serial.print(F("greather than the max target ")); Serial.print(a_maxTarget, 2); Serial.print(F(", by ")); Serial.print(a_percent); Serial.print(F("% yet the ")); Serial.print(a_str); Serial.print(F(" remains the same"));
    }
    a_fanOption = device::SPEED_UP;
  }
  else if (lessThan(a_sensor, a_minTarget, 2) && a_state == sensor::IS_FALLING) {
    if (a_debug) {
      Serial.print(F("less than the min target ")); Serial.print(a_minTarget, 2); Serial.print(F(", by ")); Serial.print(a_percent); Serial.print(F("% but the ")); Serial.print(a_str); Serial.print(F(" is falling"));
    }
    a_fanOption = device::SPEED_DOWN; //device::SPEED_MIN;
  }
  else if (lessThan(a_sensor, a_minTarget, 2) && a_state == sensor::IS_RISING) {
    if (a_debug) {
      Serial.print(F("less than the min target ")); Serial.print(a_minTarget, 2); Serial.print(F(", by ")); Serial.print(a_percent); Serial.print(F("% but the ")); Serial.print(a_str); Serial.print(F(" is rising"));
    }
    a_fanOption = device::SPEED_IDLE;
  }
  else if (lessThan(a_sensor, a_minTarget, 2) && a_state == sensor::IS_SAME) {
    if (a_debug) {
      Serial.print(F("less than the min target ")); Serial.print(a_minTarget, 2); Serial.print(F(", by ")); Serial.print(a_percent); Serial.print(F("% yet the ")); Serial.print(a_str); Serial.print(F(" remains the same"));
    }
    a_fanOption = device::SPEED_DOWN;
  }
  else if (a_state == sensor::IS_FALLING) {
    if (a_debug) {
      Serial.print(F("within the min ")); Serial.print(a_minTarget, 2); Serial.print(F(" and max ")); Serial.print(a_maxTarget, 2); Serial.print(F(" targets, but the ")); Serial.print(a_str); Serial.print(F(" is falling"));
    }
    if (!(user::fansControlTemperature && user::fansControlHumidity))
      a_fanOption = device::SPEED_DOWN_SLOW;
  }
  else if (a_state == sensor::IS_RISING) {
    if (a_debug) {
      Serial.print(F("within the min ")); Serial.print(a_minTarget, 2); Serial.print(F(" and max ")); Serial.print(a_maxTarget, 2); Serial.print(F(" targets, but the ")); Serial.print(a_str); Serial.print(F(" is rising "));
    }
    if (!(user::fansControlTemperature && user::fansControlHumidity))
      a_fanOption = device::SPEED_UP_SLOW;
  }
  else if (a_state == sensor::IS_SAME) {
    if (a_debug) {
      Serial.print(F("within the min ")); Serial.print(a_minTarget, 2); Serial.print(F(" and max ")); Serial.print(a_maxTarget, 2); Serial.print(F(" targets, and the ")); Serial.print(a_str); Serial.print(F(" remains the same"));
    }
    if (!(user::fansControlTemperature && user::fansControlHumidity))
      a_fanOption = device::SPEED_IDLE;
  }
  if (a_debug) {
    Serial.print(F(" using a hysteresis of ")); Serial.println(sensor::airHysteresis, 2);
  }   
  if (a_debug) {    
    Serial.print(F("The fan mode for ")); Serial.print(a_str); Serial.print(F(" is set to "));
    if (a_fanOption == device::SPEED_IDLE)
      Serial.println(F("idle"));
    else if (a_fanOption == device::SPEED_MIN)
      Serial.println(F("min speed"));
    else if (a_fanOption == device::SPEED_MAX)
      Serial.println(F("max speed"));
    else if (a_fanOption == device::SPEED_DOWN)
      Serial.println(F("speed down"));
    else if (a_fanOption == device::SPEED_UP)
      Serial.println(F("speed up"));
    else if (a_fanOption == device::SPEED_DOWN_SLOW)
      Serial.println(F("speed down slowly"));
    else if (a_fanOption == device::SPEED_UP_SLOW)
      Serial.println(F("speed up slowly"));
  }
}

bool i2cScanner(const uint8_t a_address) {
  if (device::globalDebug) {
    Serial.print(F("Scanning for I2C address 0x"));
    if( a_address < 16)
     Serial.print(F("0"));
    Serial.print(a_address, HEX); Serial.print(F("..."));
  }
  Wire.begin();
  Wire.beginTransmission(a_address);
  if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
    if (device::globalDebug) {
      Serial.println(F("success"));
    }
    return true;
  }
  if (device::globalDebug) {
    Serial.println(F("failed"));
  }
  return false;
}

void restartWifi() {
  WiFi.softAPdisconnect(false);
  WiFi.enableAP(false);
  server.stop(); // Stop web server   
  yield();
  delay(500);
  WiFi.softAP(wifi::ssid, wifi::password, wifi::hiddenNetwork);
  WiFi.enableAP(true);
  server.begin(); // Start web server server.stop(); // Stop web server  
  yield(); 
  String IP = WiFi.softAPIP().toString();
  if (device::globalDebug) {
    Serial.print(F("Started AP on IP address: ")); Serial.println(WiFi.softAPIP());
    Serial.print(F("AP SSID: ")); Serial.println(wifi::ssid);
    Serial.print(F("AP password: ")); Serial.println(wifi::password);
  }
} 
