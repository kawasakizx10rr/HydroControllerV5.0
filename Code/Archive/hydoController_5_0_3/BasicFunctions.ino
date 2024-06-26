void initializeDevice() {
  if (device::globalDebug)
    printf("SPI max speed: %luMhz\n", MAXSPISPEED / 1000000UL);
  tft.begin(RA8875_800x480);
  tft.touchBegin();
  tft.touchEnable(true);
  tft.backlight(true);
  tft.displayOn(true);
  if (device::globalDebug)
    printf("Hydro Master v%s started...\n", device::versionNumber);
  tft.fillWindow(RA8875_BLACK);
  tft.setFont(&akashi_36px_Regular);
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.setCursor(120, 200);
  tft.print("Hydro master");
  tft.setFontScale(1);
  tft.setCursor(120, 276);
  tft.print("version "); tft.print(device::versionNumber);
  int startX = 305;
  //
  tft.setFont(&Arial_22px_Regular);
  tft.print(120, 370, "Initilizing device...");
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.print(120, 370, "Loading EEPROM...");
  delay(200);
  externalEEPROM.setMemoryType(256);
  if (!externalEEPROM.begin()) {
   tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
   tft.setTextColor(RA8875_RED, RA8875_BLACK);
   tft.print(120, 370, "Failed to load EEPROM");
   //while(true){};
  }
  //initializeEEPROM();
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, "Loading 328P...");
  delay(200);
  if (!i2cScanner(device::slaveAddress)) {
    tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, "Failed to load Atmel 328P");
    //while(true){};
  }
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, "Loading DS3231...");
  delay(200);
  if (!i2cScanner(URTCLIB_ADDRESS)) {
    tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, "Failed to load DS3231");
    while(true){};
  }
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, "Loading DS18B20...");
  delay(200);
  if (!dallasTemperature.begin()) {
    tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, "Failed to load DS18B20");
    while(true){};
  }
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, "Loading HDC1080...");
  delay(200);
  if(!hdc.begin()) {
    tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
    tft.print(120, 370, "Failed to load HDC1080");
    while(true){};
  }
  //
  //ads.setGain(GAIN_ONE); 
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, "Loading ADS1015...");
  delay(200);
  if(!ads.begin()) {
      tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
      tft.setTextColor(RA8875_RED, RA8875_BLACK);
      tft.print(120, 370, "Failed to load ADS1015");
      while(true){};
  }
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.print(120, 370, "Loading PCA9685...");
  delay(200);
  if (!pwm.begin()) {
      tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
      tft.setTextColor(RA8875_RED, RA8875_BLACK);
      tft.print(120, 370, "Failed to load PCA9685");
      while(true){};
  }
  //
  tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
  tft.print(120, 370, "Loading sensors...");
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
  if (user::lightMode == 0 && restartLightingTimer()) {
    analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
    device::lightOn = true;
    device::lightSwitchedOnHour = rtc.hour();
    device::lightSwitchedOnMin = rtc.minute();
    if (device::globalDebug)
      printf("Auto started light on boot\n");
  }
  // // Load the Co2 sensor
  Serial2.write(sensor::co2Request, 9);
  delay(50);
  Serial2.flush();

  ph.begin(sensor::phAcidicVoltage, sensor::phNeutralVoltage);
  ec.begin(sensor::ecKvalueLow, sensor::ecKvalueHigh);

  setRgbLed(0, 0, 4096);
  // Dont adjust, set graph data out of range of viewed precison, thus 0
  if (sensor::sensorArrayPos == 0) {
    sensor::phArray[0] = 0.001; 
    sensor::ecArray[0] = 0.001;
    sensor::tdsArray[0] = 0.01;
    sensor::co2Array[0] = 0.01;
    sensor::waterTemperatureArray[0] = 0.001;
    sensor::waterTemperatureArrayF[0] = 0.001;
    sensor::waterLevelArray[0] = 0.01;
    sensor::waterLevelArrayInInches[0] = 0.001;
    sensor::airTemperatureArray[0] = 0.001;
    sensor::airTemperatureArrayF[0] = 0.001;
    sensor::humidityArray[0] = 0.01;
    sensor::fanOneSpeedArray[0] = 0.01;
    sensor::fanTwoSpeedArray[0] = 0.01;
    sensor::sensorArrayPos = 1;
  }
  //
  tft.fillCircle(startX, 340, 14, RA8875_WHITE);
  delay(250);
  startX += 38;
  // HTML call back functions from user interactions
  server.on("/", handleRoot);
  server.on("/getPageData", getPageData);
  server.on("/setValue", setValue);
  if (wifi::wifiEnabled) {
    tft.fillRect(120, 370, 400, 24, RA8875_BLACK);
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print(120, 370, "Loading web server...");
    // Connect to Wi-Fi network with SSID and password
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(wifi::ssid, wifi::password, wifi::hiddenNetwork);
    // Print connection details
    String IP = WiFi.softAPIP().toString();
    if (device::globalDebug) {
      printf("Started AP on IP address: %s\n", IP);
      printf("AP SSID: %s\n", wifi::ssid);
      printf("AP password: %s\n", wifi::password);
    }
    // Start the Wifi portal
    server.begin();  //Start web server
    delay(2000);
  }

  device::previousDosingMinute = rtc.minute();
  device::previousDosingHour = rtc.hour();

  // save log message, system started
  saveLogMessage(0);
  
  tft.fillWindow(user::backgroundColor);
  frame();
  OuterMenuIcons();
  tft.setFont(&akashi_36px_Regular);
  drawPages();
  tft.setTextColor(RA8875_WHITE, user::backgroundColor);
  display::lastTouchX = tft.width();

  //userSetupGuide(); // JUST FOR TESTING
}

void analogWriteChnl(const uint8_t& a_channel, const uint16_t& a_dutyCycle) {
  if (device::globalDebug)
    printf("Channel %d set to ", a_channel);
  if(a_dutyCycle == 0) {
    if (device::globalDebug)
      printf("OFF\n");
    pwm.setPWM(a_channel, 0, 4096);        // turns a_channel fully off
  }
  else if(a_dutyCycle == 4096) {
    if (device::globalDebug)
      printf("ON\n");
    pwm.setPWM(a_channel, 4096, 0);        // turns a_channel fully on
  }
  else {
    if (device::globalDebug)
      printf("%d\n", a_dutyCycle);
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
  // Wire.beginTransmission(device::slaveAddress);
  // Wire.write(a_fanOneSpeed);
  // Wire.write(a_fanTwoSpeed);
  // Wire.endTransmission();
}

void setTimer(uint8_t& a_min, uint8_t& a_hour, const unsigned int& a_interalInMinutes) {
  unsigned int hours = a_interalInMinutes / 60;
  unsigned int minutes = a_interalInMinutes % 60;
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
  if (device::globalDebug) 
    printf("Fan will be turn back on at %d:%d:00\n", a_hour, a_min);
}

void setRgbLed(const int a_red, const int a_green, const int a_blue) {
  if (!user::disableLED) {
    if (device::globalDebug)
      printf("setRgbLed(%d, %d, %d);\n", a_red, a_green, a_blue);
    analogWriteChnl(pca9685Channel::ledRed, a_red);
    analogWriteChnl(pca9685Channel::ledBlue, a_green);
    analogWriteChnl(pca9685Channel::ledGreen, a_blue);
  }
  else {
    if (device::globalDebug)
      printf("RGB LED disabled\n");
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

// Adjust any (uint8_t,int,float or long) value by x increment within the range of min and max
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
  if (device::globalDebug)
    printf("Adjust value = %.2f\n", a_val);
  return a_val;
}

unsigned long adjustInterval(unsigned long a_val, const int a_increment) {
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
  if (device::globalDebug)
    printf("Adjust Interval = %ul\n", a_val);
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
    printf("Setting RTC rtc\n");
  // Day-of-Week: monday = 1, sunday = 7
  rtc.set(0, user::rtcMinute, user::rtcHour, user::rtcDayOfWeek + 1, user::rtcDay, user::rtcMonth, user::rtcYear - 2000);
  delay(100);
  rtc.refresh();
}

// prime a doser at a given speed in PWM 0-255
void prime(const uint8_t& a_doser, const uint8_t& a_channel, const int& a_speed) {
  static uint8_t previousDoser = 0;
  static int currentDosingAmount = 0;
  static unsigned long primeRunTime = 0;
  device::primeTouchTime = millis();
  if (a_speed != 0) {
    if (a_doser != previousDoser) {
      if (device::globalDebug)
        printf("Priming doser: %d at PWM: %d\n", a_doser, a_speed);
      analogWriteChnl(a_channel, a_speed);
      device::doserIsPriming[a_doser-1] = true;
      currentDosingAmount = 0;
      primeRunTime = millis();
      previousDoser = a_doser;
    }
    if (millis() - primeRunTime >= 1000UL) {
      primeRunTime = millis();
      if (device::globalDebug)
        printf("Primed %d mls\n", ++currentDosingAmount);
      beep();
    }
  }
  else { // If not priming turn all off dosers, just to be safe
    if (device::globalDebug)
      printf("Stopping doser %d\n", a_doser);
    analogWriteChnl(a_channel, 0);
    device::doserIsPriming[a_doser-1] = false;
    previousDoser = 0;
    currentDosingAmount = 0;
    primeRunTime = 0;
  }
}

// Save a log message to the array and push back the array when full
void saveLogMessage(const uint8_t& a_logType) {
  static bool pushBackLogs = false;
  rtc.refresh();
  const char* t_mon = rtc.getMonthStr();
  const char* t_time = rtc.getTimeStr();
  char t_day[8];
  itoa(rtc.day(), t_day, 10);
  // Push back log array if full
  int arraypos = message::systemLogPos;
  if (message::systemLogPos == message::maxLogs) {
    if (device::globalDebug)
      printf("Pushing logs back...\n");
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
  const char* logPretext = message::notificationsArray[a_logType];
  if (device::globalDebug)
    printf("Added new log type %d at position %d, LOG: %s %s\n", a_logType, arraypos, logPretext, message::timeStrArray[arraypos]);
  if (message::systemLogPos < message::maxLogs) 
    message::systemLogPos++;
}

// Convert a float to int, with a precison of 2 decimal places
int fltToInt(const float& a_value) {
  int val = a_value * 100;
  return val;
}

// Convert a float to int, with a precison of 3 decimal places
int doubleToInt(const float& a_value) {
  int val = a_value * 1000;
  return val;
}

// convert a celsius value to ferinheight
float convertToF(const float& a_value) {
  return (a_value * 1.8) + 32;
}

// convert a centimeter value to inches
float convertToInch(const float& a_value) {
  return a_value / 2.54;
}

// when the graph arrays are full push back the array values by 1
void pushBackDataArrays() {
  static bool graphFirstSave = true;
  static unsigned long graphPreviousMillis = 0;
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
}

// update the graph values at x position
void updateGraphArrays(const uint8_t& a_arrayPosition) {
  sensor::phArray[a_arrayPosition] = sensor::ph <= 0 ? 0.001 : sensor::ph;
  sensor::ecArray[a_arrayPosition] = sensor::ec <= 0 ? 0.001 : sensor::ec;
  sensor::tdsArray[a_arrayPosition] = sensor::tds <= 0 ? 0.01 : sensor::tds;
  sensor::co2Array[a_arrayPosition] = sensor::co2 <= 0 ? 0.01 : sensor::co2;
  sensor::waterTemperatureArray[a_arrayPosition] = sensor::waterTemp <= 0 ? 0.001 : sensor::waterTemp;
  sensor::waterTemperatureArrayF[a_arrayPosition] = convertToF(sensor::waterTemp) <= 0 ? 0.001 : convertToF(sensor::waterTemp);
  sensor::waterLevelArray[a_arrayPosition] = sensor::waterLevel <= 0 ? 0.01 : sensor::waterLevel;
  sensor::waterLevelArrayInInches[a_arrayPosition] = sensor::waterLevelInches <= 0 ? 0.001 : sensor::waterLevelInches;
  sensor::airTemperatureArray[a_arrayPosition] = sensor::airTemp <= 0 ? 0.001 : sensor::airTemp;
  sensor::airTemperatureArrayF[a_arrayPosition] = convertToF(sensor::airTemp) <= 0 ? 0.001 : convertToF(sensor::airTemp);
  sensor::humidityArray[a_arrayPosition] = sensor::humidity <= 0 ? 0.01 : sensor::humidity;
  sensor::fanOneSpeedArray[a_arrayPosition] = device::fanOneSpeed <= 0 ? 0.01 : device::fanOneSpeed;
  sensor::fanTwoSpeedArray[a_arrayPosition] = device::fanTwoSpeed <= 0 ? 0.01 : device::fanTwoSpeed;
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
float averageFltArray(const float* a_array, const int a_length) {
  float rtnVal = 0;
  for (int i = 0; i < a_length; i++)
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

bool hasChanged(const float a_val, const float a_val2, const int a_precision) {
  long long val = a_precision != 0 ? a_val * pow(10, a_precision) : a_val;
  long long val2 = a_precision != 0 ? a_val2 * pow(10, a_precision) : a_val2;
  if (val != val2) {
    //printf("change in a_val: %.2f, a_val2: %.2f, epsilon: %.2f\n", a_val, a_val2, a_epsilon);
    return true;
  }
  return false;
}

void restartWifi() {
  if (wifi::wifiEnabled) {
    WiFi.softAPdisconnect(false);
    WiFi.enableAP(false);
    server.stop(); // Stop web server   
    delay(100);
    WiFi.softAP(wifi::ssid, wifi::password, wifi::hiddenNetwork);
    WiFi.enableAP(true);
    server.begin(); // Start web server server.stop(); // Stop web server   
    String IP = WiFi.softAPIP().toString();
    if (device::globalDebug) {
      printf("Started AP on IP address: %s\n", IP);
      printf("AP SSID: %s\n", wifi::ssid);
      printf("AP password: %s\n", wifi::password);
    }
  }
}

void addCharToStr(char* a_charPtr, const int a_arrayLen, const char a_char) {
  for (int i = 0; i < a_arrayLen; i++) {
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

float lessThan(const float a_val, const float a_target, const int& a_precision) {
  if (hasChanged(a_val, a_target, a_precision) && a_val < a_target)
    return true;
  return false;
}

float greaterThan(const float a_val, const float a_target, const int& a_precision) {
  if (hasChanged(a_val, a_target, a_precision) && a_val > a_target)
    return true;
  return false;
}

void adjustFanMode(const float a_sensor, const float a_target, device::controlOptions& a_fanOption, const sensor::airStates a_state, const char* a_str, const float a_percent) {
  if (greaterThan(a_sensor, a_target, 2) && a_state == sensor::IS_FALLING) {
    if (device::globalDebug)
      printf("The %s: %.2f is > max target: %.2f by %.2f%%, but the %s is falling\n", a_str, a_sensor, a_target, a_percent, a_str);
    a_fanOption = device::SPEED_IDLE;
  }
  else if (greaterThan(a_sensor, a_target, 2) && a_state == sensor::IS_RISING) {
    if (device::globalDebug)
      printf("The %s: %.2f is > max target: %.2f by %.2f%%, and the %s is rising\n", a_str, a_sensor, a_target, a_percent, a_str);
    a_fanOption = device::SPEED_UP; //device::SPEED_MAX;
  }
  else if (greaterThan(a_sensor, a_target, 2) && a_state == sensor::IS_SAME) {
    if (device::globalDebug)
      printf("The %s: %.2f is > max target: %.2f by %.2f%%, yet the %s remains the same\n", a_str, a_sensor, a_target, a_percent, a_str);
    a_fanOption = device::SPEED_UP;
  }
  else if (lessThan(a_sensor, a_target, 2) && a_state == sensor::IS_FALLING) {
    if (device::globalDebug)
      printf("The %s: %.2f is < min target: %.2f by %.2f%%, and the %s is falling\n", a_str, a_sensor, a_target, a_percent, a_str);
    a_fanOption = device::SPEED_DOWN; //device::SPEED_MIN;
  }
  else if (lessThan(a_sensor, a_target, 2) && a_state == sensor::IS_RISING) {
    if (device::globalDebug)
      printf("The %s: %.2f is < min target: %.2f by %.2f%%, but the %s is rising\n", a_str, a_sensor, a_target, a_percent, a_str);
    a_fanOption = device::SPEED_IDLE;
  }
  else if (lessThan(a_sensor, a_target, 2) && a_state == sensor::IS_SAME) {
    if (device::globalDebug)
      printf("The %s: %.2f is < min target: %.2f by %.2f%%, yet the %s remains the same\n", a_str, a_sensor, a_target, a_percent, a_str);
    a_fanOption = device::SPEED_DOWN;
  }
  else if (a_state == sensor::IS_FALLING) {
    if (device::globalDebug)
      printf("The %s: %.2f is with min max targets, but the %s is falling\n", a_str, a_sensor, a_str);
    if (!(user::fansControlTemperature && user::fansControlHumidity))
      a_fanOption = device::SPEED_DOWN;
  }
  else if (a_state == sensor::IS_RISING) {
    if (device::globalDebug)
      printf("The %s: %.2f is with min max targets, but the %s is rising.\n", a_str, a_sensor, a_str);
    if (!(user::fansControlTemperature && user::fansControlHumidity))
      a_fanOption = device::SPEED_UP;
  }
  else if (a_state == sensor::IS_SAME) {
    if (device::globalDebug)
      printf("The %s: %.2f is with min max targets, and the %s remains the same\n", a_str, a_sensor, a_str);
    if (!(user::fansControlTemperature && user::fansControlHumidity))
      a_fanOption = device::SPEED_IDLE;
  }
  if (device::globalDebug) {    
    printf("The fan mode for %s is set to ", a_str);
    if (a_fanOption == device::SPEED_IDLE)
      printf("idle\n");
    else if (a_fanOption == device::SPEED_MIN)
      printf("min speed\n");
    else if (a_fanOption == device::SPEED_MAX)
      printf("max speed\n");
    else if (a_fanOption == device::SPEED_DOWN)
      printf("speed down\n");
    else if (a_fanOption == device::SPEED_UP)
      printf("speed up\n");
  }
}

bool i2cScanner(const uint8_t a_address) {
  if (device::globalDebug)
    printf("Scanning for I2C address 0x%02X...\n", a_address);
  Wire.begin();
  Wire.beginTransmission(a_address);
  if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
    if (device::globalDebug)
      printf("Found address: 0x%02X", a_address);
    return true;
  }
  if (device::globalDebug)
    printf("Failed to find address: 0x%02X", a_address);
  return false;
}