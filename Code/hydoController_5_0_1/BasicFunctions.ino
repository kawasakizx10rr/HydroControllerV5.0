void initializeDevice() {
  printf("Hydro Master v%s started...\n", device::versionNumber);
  tft.fillWindow(RA8875_BLACK);
  tft.setFont(&akashi_36px_Regular);
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(120, 200);
  tft.print("Hydro master");
  tft.setFontScale(1);
  tft.setCursor(120, 276);
  tft.print("version "); tft.print(device::versionNumber);
  int startX = 305;
  for (uint8_t i = 0; i < 6; i++) {
    tft.fillCircle(startX, 340, 14, RA8875_WHITE);
    startX += 38;
    delay(500);
  }
  //externalEEPROM.setMemoryType(256);
  initializeEEPROM();
  //
  dallasTemperature.begin();
  //
  if(!hdc.begin()) {
    printf("Error: Unable to connect HDC1080!\n");
  }
  //
  //ads.setGain(GAIN_ONE); 
  while (!ads.begin()) {
    printf("Failed to initialize ADS\n");
    delay(500);
  }
  //
  pwm.begin();
  //pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  analogWriteChnl(pca9685Channel::airHeater, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::co2Solenoid, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 4096 : 0));
  analogWriteChnl(pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 4096 : 0));
  // If light is set to auto mode check timer
  if (user::lightMode == 0 && restartLightingTimer()) {
    analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
    device::lightOn = true;
    rtc.refresh();
    device::lightSwitchedOnHour = rtc.hour();
    device::lightSwitchedOnMin = rtc.minute();
    printf("Auto started light on boot\n");
  }
  // // Load the Co2 sensor
  Serial2.write(sensor::co2Request, 9);
  delay(50);
  Serial2.flush();
  // // Start the AC fans
  device::fanOneSpeed = user::targetMinFanOneSpeed;
  device::fanTwoSpeed = user::targetMinFanTwoSpeed;
  setFanSpeed(0, device::fanOneSpeed);
  setFanSpeed(1, device::fanTwoSpeed);

  ph.begin(sensor::phAcidicVoltage, sensor::phNeutralVoltage);
  ec.begin(sensor::ecKvalueLow, sensor::ecKvalueHigh);

  setRgbLed(0, 0, 4096);

  // Dont adjust, set graph data out of range of viewed precison, thus 0
  if (device::graphArrayPos == 0) {
    device::phArray[0] = 0.001; 
    device::ecArray[0] = 0.001;
    device::tdsArray[0] = 0.01;
    device::co2Array[0] = 0.01;
    device::waterTemperatureArray[0] = 0.001;
    device::waterTemperatureArrayF[0] = 0.001;
    device::waterLevelArray[0] = 0.01;
    device::waterLevelArrayInInches[0] = 0.001;
    device::airTemperatureArray[0] = 0.001;
    device::airTemperatureArrayF[0] = 0.001;
    device::humidityArray[0] = 0.01;
    device::fanOneSpeedArray[0] = 0.01;
    device::fanTwoSpeedArray[0] = 0.01;
    device::graphArrayPos = 1;
  }
  if (wifi::wifiEnabled) {
  // Connect to Wi-Fi network with SSID and password
  // Remove the password parameter, if you want the AP (Access Point) to be open
  //WiFi.softAP(wifi::ssid, wifi::password, wifi::hiddenNetwork);
  // Print connection details
  ///IPAddress IP = WiFi.softAPIP();
  //printf("Started AP on IP address: %s\n", IP);
  //printf("AP SSID: %s\n", wifi::ssid);
  //printf("AP password: %s\n", wifi::password);
  // HTML call back functions from user interactions
  //server.on("/", handleRoot);  // This is display page
  //server.on("/getData", getData);
  //server.on("/setValue", setValue);
  // Start the Wifi portal
  //server.begin();  //Start server
  //delay(2000);
  }
  rtc.refresh();
  device::previousDosingMinute = rtc.minute();
  device::previousDosingHour = rtc.hour();

  saveLogMessage(0); // save log message, system started

  for (int i = 0; i < device::maxGraphArrayValues; i++) {
    device::phArray[i] = random(1, 14);
    device::co2Array[i] = random(300, 1200);
    device::ecArray[i] = random(100, 1400);
    device::tdsArray[i] = random(50, 500);
    device::waterTemperatureArray[i] = random(1, 40);
    device::waterTemperatureArrayF[i] = random(1, 90);
    device::waterLevelArray[i] = random(1, 30);
    device::waterLevelArrayInInches[i] = random(1, 12);
    device::airTemperatureArray[i] = random(5, 40);
    device::airTemperatureArrayF[i] = random(40, 120);
    device::humidityArray[i] = random(0, 100);
    device::fanOneSpeedArray[i] = random(0, 100);
    device::fanTwoSpeedArray[i] = random(0, 100);
  }
  device::graphArrayPos = device::maxGraphArrayValues;
  
  tft.fillWindow(user::backgroundColor);
  frame();
  OuterMenuIcons();
  display::lastTouchX = tft.width();
}

void analogWriteChnl(const uint8_t& a_channel, const uint16_t& a_dutyCycle) {
  printf("Channel %d set to ", a_channel);
  if(a_dutyCycle == 0) {
    printf("OFF\n");
    pwm.setPWM(a_channel, 0, 4096);        // turns a_channel fully off
  }
  else if(a_dutyCycle == 4096) {
    printf("ON\n");
    pwm.setPWM(a_channel, 4096, 0);        // turns a_channel fully on
  }
  else {
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

void setFanSpeed(const uint8_t a_fan, const uint8_t a_speed) {
  // i2c COMS to the 328p
  //Wire.beginTransmission(device::slaveAddress);
  //Wire.write(a_fan);
  //Wire.write(a_speed);
  //Wire.endTransmission();
}

void setRgbLed(const int a_red, const int a_green, const int a_blue) {
  if (!user::disableLED) {
    printf("setRgbLed(%d, %d, %d);\n", a_red, a_green, a_blue);
    analogWriteChnl(pca9685Channel::ledRed, a_red);
    analogWriteChnl(pca9685Channel::ledBlue, a_green);
    analogWriteChnl(pca9685Channel::ledGreen, a_blue);
  }
  else {
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
  if (display::currentTouch - display::lastTouch > 10000UL)
    a_increment *= 10.0;

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
  printf("Adjust value = %.2f\n", a_val);
  return a_val;
}

unsigned long adjustInterval(unsigned long a_val, const int a_increment) {
  beep();
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
  printf("Setting RTC rtc\n");
  // Day-of-Week: monday = 1, sunday = 7
  rtc.set(0, user::rtcMinute, user::rtcHour, user::rtcDayOfWeek + 1, user::rtcDay, user::rtcMonth, user::rtcYear - 2000);
  delay(100);
  rtc.refresh();
}

// prime a doser at a given speed in PWM 0-255
void prime(const uint8_t& a_doser, const uint8_t& a_pin, const uint8_t& a_speed) {
  static uint8_t previousDoser = 0;
  static int currentDosingAmount = 0;
  static unsigned long primeRunTime = 0;
  if (a_speed != 0) {
    if (a_doser != previousDoser) {
      printf("Priming doser: %d\n", a_doser);
      analogWrite(a_pin, a_speed * 2.55);
      device::isPriming = a_doser;
      currentDosingAmount = 0;
      primeRunTime = millis();
      previousDoser = a_doser;
    }
    if (millis() - primeRunTime >= 1000UL) {
      primeRunTime = millis();
      printf("Primed %d mls\n", ++currentDosingAmount);
      beep();
    }
  }
  else { // If not priming turn all off dosers, just to be safe
    printf("Stopping dosers\n");
    analogWriteChnl(pca9685Channel::doserOne, 0);
    analogWriteChnl(pca9685Channel::doserTwo, 0);
    analogWriteChnl(pca9685Channel::doserThree, 0);
    analogWriteChnl(pca9685Channel::doserFour, 0);
    analogWriteChnl(pca9685Channel::doserFive, 0);
    analogWriteChnl(pca9685Channel::doserSix, 0);
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
    if (device::graphArrayPos < device::maxGraphArrayValues) {
      updateGraphArrays(device::graphArrayPos);
      device::graphArrayPos++;
    }
    else {
      for (uint8_t i = 0; i < device::maxGraphArrayValues - 1; i++) {
        device::phArray[i] = device::phArray[i + 1];
        device::ecArray[i] = device::ecArray[i + 1];
        device::tdsArray[i] = device::tdsArray[i + 1];
        device::co2Array[i] = device::co2Array[i + 1];
        device::waterTemperatureArray[i] = device::waterTemperatureArray[i + 1];
        device::waterTemperatureArrayF[i] = device::waterTemperatureArrayF[i + 1];
        device::waterLevelArray[i] = device::waterLevelArray[i + 1];
        device::waterLevelArrayInInches[i] = device::waterLevelArrayInInches[i + 1];
        device::airTemperatureArray[i] = device::airTemperatureArray[i + 1];
        device::airTemperatureArrayF[i] = device::airTemperatureArrayF[i + 1];
        device::humidityArray[i] = device::humidityArray[i + 1];
        device::fanOneSpeedArray[i] = device::fanOneSpeedArray[i + 1];
        device::fanTwoSpeedArray[i] = device::fanTwoSpeedArray[i + 1];
      }
      updateGraphArrays(device::maxGraphArrayValues - 1);
    }
    graphFirstSave = false;
    device::newGraphData = true;
    graphPreviousMillis = millis();
  }
}

// update the graph values at x position
void updateGraphArrays(const uint8_t& a_arrayPosition) {
  device::phArray[a_arrayPosition] = sensor::ph <= 0 ? 0.001 : sensor::ph;
  device::ecArray[a_arrayPosition] = sensor::ec <= 0 ? 0.001 : sensor::ec;
  device::tdsArray[a_arrayPosition] = sensor::tds <= 0 ? 0.01 : sensor::tds;
  device::co2Array[a_arrayPosition] = sensor::co2 <= 0 ? 0.01 : sensor::co2;
  device::waterTemperatureArray[a_arrayPosition] = sensor::waterTemp <= 0 ? 0.001 : sensor::waterTemp;
  device::waterTemperatureArrayF[a_arrayPosition] = convertToF(sensor::waterTemp) <= 0 ? 0.001 : convertToF(sensor::waterTemp);
  device::waterLevelArray[a_arrayPosition] = sensor::waterLevel <= 0 ? 0.01 : sensor::waterLevel;
  device::waterLevelArrayInInches[a_arrayPosition] = sensor::waterLevelInches <= 0 ? 0.001 : sensor::waterLevelInches;
  device::airTemperatureArray[a_arrayPosition] = sensor::airTemp <= 0 ? 0.001 : sensor::airTemp;
  device::airTemperatureArrayF[a_arrayPosition] = convertToF(sensor::airTemp) <= 0 ? 0.001 : convertToF(sensor::airTemp);
  device::humidityArray[a_arrayPosition] = sensor::humidity <= 0 ? 0.01 : sensor::humidity;
  device::fanOneSpeedArray[a_arrayPosition] = device::fanOneSpeed <= 0 ? 0.01 : device::fanOneSpeed;
  device::fanTwoSpeedArray[a_arrayPosition] = device::fanTwoSpeed <= 0 ? 0.01 : device::fanTwoSpeed;
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
float averageArray(const float* a_array, const uint8_t& a_size) {
  float average = 0;
  for (uint8_t i = 0; i < a_size; i++) {
    average += a_array[i];
  }
  return average / (float)a_size;
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
  if (in_min == in_max) {
    out_min -= 0.01;
    out_max += 0.01;
  } 
  else if (out_min == out_max)
    return out_max;
  else if (x < in_min)
    return out_min;
  else if (x > in_max)
    return out_max;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool isEqual(const float& a_val, const float& a_val2, const float& epsilon) {
  return fabs(a_val - a_val2) < epsilon;
}

bool hasChanged(const float& a_val, float& a_prevVal, const float& epsilon) {
  long val = a_val * (epsilon * 100.0);
  long prevVal = a_prevVal * (epsilon * 100.0);
  if (val != prevVal) {
    //printf("epsilon: %.2f\n", epsilon);
    //printf("change in a_val: %.2f\n" a_prevVal: %.2f, a_val, a_prevVal);
    a_prevVal = a_val;
    return true;
  }
  return false;
}