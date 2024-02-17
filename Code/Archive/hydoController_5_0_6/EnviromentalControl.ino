void envriomentalControl() {
 // Get current date time
 static unsigned long previousMillis = millis();
  if (millis() - previousMillis >= 1000UL) {
    rtc.refresh();
    previousMillis = millis();
    //Serial.print(F("Free memory: ")); Serial.print(esp_get_free_heap_size()); Serial.println(F(" bytes"));
  }
  lightingControl();
  cyclicTimers();
  acControl();
  waterLevelControl();
  waterTemperatureControl();
  co2Control();
  airControl();
  waterEcPhControl();
}

// Control the water level
void waterLevelControl() {
  bool startDraining = false, startRefilling = false;
  static uint8_t previousDate = 0;
  unsigned long previousMillis = 0;
  uint8_t continueRefilling = device::NOT_SET;
  uint8_t continueDraining = device::NOT_SET;
  unsigned long lastTouch = millis();

  if (device::sensorsReady) {
    // auto drain and refill tank on set dates and time
    if (!user::disableDrainAndRefill && rtc.day() != previousDate) {
      for (uint8_t i = 0; i < 31; i++) {
        if (user::autoFillDays[i] && rtc.day() == i + 1 && rtc.hour() == user::autoFillHour && rtc.minute() >= user::autoFillMinute) {
          if (device::globalDebug) {
            Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
            Serial.println(F("Auto drain time matched, about to start the auto drain process"));
          }
          saveLogMessage(1);
          startDraining = true;
          previousDate = rtc.day();
          break;
        }
      }
    }

    // Show a dialog asking the user if they want to cancel the auto drain process before it starts
    if (startDraining) {
      launchDrainNotification(continueDraining, startDraining);
      lastTouch = millis() + 5000UL;
    }

    // Show a dialog asking the user if they want to abort the drain process while its draining
    if (startDraining && continueDraining != device::CANCEL) {
      if (device::globalDebug)
        Serial.println(F("About to show the drain abort dialog, and start draining"));
      const float waterTarget = user::convertToInches ? user::targetMinWaterHeight / 2.5 : user::targetMinWaterHeight;
      abortMessage(message::cancelDraining, user::convertToInches ? "\"" : "cm", waterTarget, -1, 1);
    }

    // Start draining the water from the tank
    int previousWaterLevel = sensor::waterLevel;
    previousMillis = millis();
    while (startDraining && continueDraining != device::CANCEL) {
      if (sensor::waterLevel > 0) { // drain tank till empty
        // Turn drain pump relay on
        analogWriteChnl(device::pca9685Channel::outletPump, (device::relayOffState == HIGH ? 0 : 4096));
        // timer checking water level is still decresing else bail after 1 minute * drainTimeout
        if (millis() - previousMillis >= 60000UL * user::drainTimeout) { // put timer value in settings GUI !
          analogWriteChnl(device::pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
          startDraining = false;
          startRefilling = true;
          if (device::globalDebug)
            Serial.println(F("Failed to pump any further water out of the tank, quiting drain process"));
        }
        int waterHeight = sensor::emptyWaterTankDepth - getWaterHeight();
        if (waterHeight >= 0)
          sensor::waterLevel = waterHeight;

        if (sensor::waterLevel < previousWaterLevel) {
          previousWaterLevel = sensor::waterLevel;
          previousMillis = millis();
        }
      }
      else {
        if (device::globalDebug)
          Serial.println(F("Draining complete"));
        analogWriteChnl(device::pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
        startDraining = false;
        startRefilling = true;
      }

      waterLevelControlTouch(lastTouch, startDraining);
      static unsigned long previousMillis = millis();
      if (millis() - previousMillis >= 1000UL) {
        rtc.refresh();
        cyclicTimers();
        previousMillis = millis();
      }  
    }

    // refill tank when water is low at any date or time
    if (!user::disableDrainAndRefill && !startRefilling) {
      static unsigned long previousDelayMillis = millis(); // give the user 5 minutes to disable "refill When Low" else if the water level is below min this will keep showing a dialog over and over..
      if (millis() - previousDelayMillis >= 300000UL) {
        if ((sensor::waterLevel < user::targetMinWaterHeight && !user::convertToInches) || (sensor::waterLevelInches < user::targetMinWaterHeightInches && user::convertToInches)) {
          startRefilling = true;
          if (device::globalDebug)
            Serial.println(F("The water level is below the min target, starting refill process"));
        }
        previousDelayMillis = millis();
      }
    }

    // Show a dialog asking the user if they want to cancel the refill process before it starts
    launchRefillNotification(startRefilling, continueRefilling);

    // Show a dialog asking the user if they want to abort the refill process while its refilling
    if (startRefilling && continueRefilling != device::CANCEL) {
      if (device::globalDebug)
        Serial.println(F("About to show refill abort dialog, and refill tank"));
      const float waterTarget = user::convertToInches ? user::targetMaxWaterHeightInches : user::targetMaxWaterHeight;
      abortMessage(message::cancelRefilling, user::convertToInches ? "\"" : "cm", waterTarget, -1, 1);
      saveLogMessage(2);

      lastTouch = millis() + 5000UL;
      previousWaterLevel = sensor::waterLevel;
      previousMillis = millis();

      while (startRefilling && continueRefilling != device::CANCEL) {
        if (!refillTank(lastTouch, previousMillis, previousWaterLevel)) {
          startRefilling = false;
          clearPage();
          display::refreshPage = true;
        }
        static unsigned long previousMillis = millis();
        if (millis() - previousMillis >= 1000UL) {
          rtc.refresh();
          cyclicTimers();
          previousMillis = millis();
        }  
      }
    }
  }
}

// Refill the tanks water and run doers all available dosers
bool refillTank(const unsigned long& a_lastTouch, unsigned long& a_previousMillis, int& a_previousWaterLevel) {
  static bool startRefilling = true;
  static bool lockPump = false;
  static bool enabledDosers[6] {false, false, false, false, false, false};
  static unsigned long previousDoserMillis = 0;
  const float waterLevel = user::convertToInches ? sensor::waterLevelInches : sensor::waterLevel;
  const float waterTarget = user::convertToInches ? user::targetMaxWaterHeightInches : user::targetMaxWaterHeight;

  if (device::globalDebug) {
    Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
  }
  if (waterLevel < waterTarget && startRefilling) {
    // turn on the inlet water pump
    analogWriteChnl(device::pca9685Channel::inletPump, (device::relayOffState == HIGH ? 0 : 4096));
    // timer checking water level is still decresing else bail after 1 minute * drainTimeout
    if (millis() - a_previousMillis >= 60000UL * user::drainTimeout) {
      if (device::globalDebug)
        Serial.println(F("Failed to pump in any further water, quiting refill process and starting dosing"));
      analogWriteChnl(device::pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
      startRefilling = false;
    }
    else if (waterLevel >= waterTarget) {
      if (device::globalDebug)
        Serial.println(F("Finished pumping in water, quiting refill process and starting dosing"));
      analogWriteChnl(device::pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
      startRefilling = false;
    }
  }

  // get current water height
  sensor::waterLevel = sensor::emptyWaterTankDepth - getWaterHeight();
  if (sensor::waterLevel < a_previousWaterLevel) {
    a_previousWaterLevel = sensor::waterLevel;
    a_previousMillis = millis();
  }
  else if (!startRefilling && !lockPump) {
    if (device::globalDebug)
      Serial.println(F("Refilling complete, starting dosing"));
    for (uint8_t i = 0; i < user::numberOfDosers; i++)
      enabledDosers[i] = true;
    lockPump = true;
    previousDoserMillis = millis();
  }

  // run dosers
  if (lockPump) {
    if (enabledDosers[0] && user::doserOneMode != device::DOSER_OFF)
      enabledDosers[0] = runDoser(1, device::pca9685Channel::doserOne, user::doserOneSpeed, user::refillDoserOneMills, previousDoserMillis);
    else if (enabledDosers[1] && user::doserTwoMode != device::DOSER_OFF)
      enabledDosers[1] = runDoser(2, device::pca9685Channel::doserTwo, user::doserTwoSpeed, user::refillDoserTwoMills, previousDoserMillis);
    else if (enabledDosers[2] && user::doserThreeMode != device::DOSER_OFF)
      enabledDosers[2] = runDoser(3, device::pca9685Channel::doserThree, user::doserThreeSpeed, user::refillDoserThreeMills, previousDoserMillis);
    else if (enabledDosers[3] && user::doserFourMode != device::DOSER_OFF)
      enabledDosers[3] = runDoser(4, device::pca9685Channel::doserFour, user::doserFourSpeed, user::refillDoserFourMills, previousDoserMillis);
    else if (enabledDosers[4] && user::doserFiveMode != device::DOSER_OFF)
      enabledDosers[4] = runDoser(5, device::pca9685Channel::doserFive, user::doserFiveSpeed, user::refillDoserFiveMills, previousDoserMillis);
    else if (enabledDosers[5] && user::doserSixMode != device::DOSER_OFF)
      enabledDosers[5] = runDoser(6, device::pca9685Channel::doserSix, user::doserSixSpeed, user::refillDoserSixMills, previousDoserMillis);
    else {
      if (device::globalDebug)
        Serial.println(F("Dosing is complete"));
      lockPump = false;
      device::dosingTimerHourCounter = 0;
      return false;
    }
  }
  // touch event - cancel button
  if (refillTankTouch(a_lastTouch))
    return false;
  return true;
}

// Control the water temperature
void waterTemperatureControl() {
  // Control the water heater
  float waterTemp = sensor::waterTemp;
  float minTemp = user::targetMinWaterTemp;
  static uint8_t prevWaterHeaterMin = 69;
  if (rtc.minute() != prevWaterHeaterMin) {
    if (user::convertToF) {
      waterTemp = convertToF(sensor::waterTemp);
      minTemp = user::targetMinWaterTempF;
    } 
    if (waterTemp <= minTemp && !device::waterHeaterIsOn) {
      device::waterHeaterIsOn = true;
      if (device::globalDebug) {
        Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
        Serial.println(F("Water heater on"));
      }
      analogWriteChnl(device::pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 0 : 4096));
      saveLogMessage(13); // save log message, water heater on
    }
    else if (waterTemp > minTemp && device::waterHeaterIsOn) {
      device::waterHeaterIsOn = false;
      if (device::globalDebug) {
        Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
        Serial.println(F("Water heater off"));
      }
      analogWriteChnl(device::pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 4096 : 0));
      saveLogMessage(14); // save log message, water heater off
    }
    prevWaterHeaterMin = rtc.minute();
  }
}

// Control the external lighting
void lightingControl() {
  using namespace user;
  static uint8_t prviousLightMode = 3;
  if (lightMode == 0) { // timer mode
    if (lightOnTimeHour == rtc.hour() && lightOnTimeMin == rtc.minute() && !device::lightOn) {
      analogWriteChnl(device::pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
      if (device::globalDebug) {
        Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
        Serial.println(F("Light on"));
      }       
      device::lightOn = true;
      device::lightSwitchedOnHour = rtc.hour();
      device::lightSwitchedOnMin = rtc.minute();
      saveLogMessage(4);
    }
    else if (lightOffTimeHour == rtc.hour() && lightOffTimeMin == rtc.minute() && device::lightOn) {
      analogWriteChnl(device::pca9685Channel::light, 0);
      if (device::globalDebug) {
        Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
        Serial.println(F("Light off"));
      } 
      device::lightOn = false;
      saveLogMessage(5);
    }
  }
  else if (lightMode == 1 && lightMode != prviousLightMode) { // constant on
    analogWriteChnl(device::pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
      Serial.println(F("Light const on"));
    } 
    device::lightOn = true;
    device::lightSwitchedOnHour = rtc.hour();
    device::lightSwitchedOnMin = rtc.minute();
  }
  else if (lightMode == 2 && lightMode != prviousLightMode) { // constant off
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
      Serial.println(F("Light const off"));
    } 
    analogWriteChnl(device::pca9685Channel::light, (device::relayOffState == HIGH ? 4096 : 0));
    device::lightOn = false;
  }
  prviousLightMode = lightMode;
  // Log any occourance of the light turning off when it should be on "once per day"
  static int previousLightOnDay = 0;
  if (device::lightOn && previousLightOnDay != rtc.day()) {
    if (device::lightSwitchedOnMin <= 57 && rtc.minute() >= device::lightSwitchedOnMin + 2) {
      if (sensor::ldr < 1000)
        saveLogMessage(6);
    }
    else if (device::lightSwitchedOnMin == 58 && rtc.minute() == 0)  {
      if (sensor::ldr < 1000)
        saveLogMessage(6);
    }
    else if (device::lightSwitchedOnMin > 58 && rtc.minute() >= 2)  {
      if (sensor::ldr < 1000)
        saveLogMessage(6);
    }
    previousLightOnDay = rtc.day();
  }
}

void cyclicTimers() {
  // Aux relay 1
  if (user::auxRelayOneMode == 0) 
    cyclicTimer(device::auxRelayOneSwitchedOnOffMin, device::auxRelayOneSwitchedOnOffHour, user::auxRelayOneTimer, device::auxRelayOneOn, pin::auxRelayOne, '1'); 
  // Aux relay 2
  if (user::auxRelayTwoMode == 0) 
    cyclicTimer(device::auxRelayTwoSwitchedOnOffMin, device::auxRelayTwoSwitchedOnOffHour, user::auxRelayTwoTimer, device::auxRelayTwoOn, pin::auxRelayTwo, '2');  
}

void cyclicTimer(uint8_t& a_switchedOnOffMin, uint8_t& a_switchedOnOffHour, const int a_duration, bool& a_auxRelayOnOff, uint8_t a_pin, const char a_num) {
  int onOffHour = a_switchedOnOffHour + (int)(a_duration / 60);
  int onOffMin = a_switchedOnOffMin + a_duration % 60;
  if (onOffMin >= 60) {
    onOffHour++;
    onOffMin -= 60;
  }
  if (onOffHour >= 24) 
    onOffHour -= 24;
  if (rtc.minute() == onOffMin && rtc.hour() == onOffHour) {
    a_auxRelayOnOff = !a_auxRelayOnOff;
    a_switchedOnOffHour = rtc.hour();
    a_switchedOnOffMin = rtc.minute();
    digitalWrite(a_pin, a_auxRelayOnOff);
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
      Serial.print(F("Aux relay ")); Serial.print(a_num); Serial.print(F(" turned ")); a_auxRelayOnOff ? Serial.println(F("on")) : Serial.println(F("off"));
    }
  }  
}

// Check if the current time is in between the users start and end time
bool restartLightingTimer() {
  using namespace user;
  int startMinutesSinceMidnight = lightOnTimeMin + 60 * lightOnTimeHour;
  int endMinutesSinceMidnight = lightOnTimeHour + 60 * lightOffTimeHour;
  int currentMinutesSinceMidnight = rtc.minute() + 60 * rtc.hour();
  if (startMinutesSinceMidnight < endMinutesSinceMidnight) {
    return ((currentMinutesSinceMidnight >= startMinutesSinceMidnight) && (currentMinutesSinceMidnight < endMinutesSinceMidnight));
  }
  else {
    return !((currentMinutesSinceMidnight >= endMinutesSinceMidnight) && (currentMinutesSinceMidnight < startMinutesSinceMidnight));
  }
}

// controls the enviroments co2 levels
void co2Control() {
  bool startCo2Relay = false;
  static uint8_t previousDate = 0;
  unsigned long co2RunTime = millis();
  uint8_t continueCo2Control = device::NOT_SET;
  unsigned long lastTouch = millis();
  sensor::co2GasTime = 0;
  if (device::sensorsReady && !user::disableCo2Control) {
    // Check if it is time to start the Co2 adjustment
    if (user::co2CheckTimeHour == rtc.hour() && user::co2CheckTimeMinute >= rtc.minute() && rtc.day() != previousDate) {
      if (device::globalDebug) {
        Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
      }
      // set the gas time, manual time or calculated
      if (sensor::co2 < (user::targetCo2 - user::co2Offset)) {
        if (user::enableManualCo2Duration)
          sensor::co2GasTime = user::manualCo2GasDuration * 60000UL;
        else
          generateCo2GasTime();
      }
      // start Co2 adjustment
      if (sensor::co2GasTime > 0) {
        // show a notification asking the user if they want to cancel the Co2 adjustment before it starts
        launchCo2Notification(continueCo2Control, previousDate);
        // If not canceled by the user continue with Co2 adjustment
        if (continueCo2Control != device::CANCEL) {
          // if the user has disableFansTimer > 0 then turn off the fans for x minutes
          if (user::disableFansTimer > 0) {
            setTimer(device::co2TurnFansBackOnMinute, device::co2TurnFansBackOnHour, user::disableFansTimer);
            setFanSpeeds(0, 0);
            device::co2DisabledFans = true;
          }
          previousDate = rtc.day();
          startCo2Relay = true;
          // turn on the Co2 solnoide
          analogWriteChnl(device::pca9685Channel::co2Solenoid, (device::relayOffState == HIGH ? 0 : 4096));
          if (device::globalDebug)
            Serial.println(F("About to show Co2 abort dialog, and start pumping Co2"));
          // Show a dialog asking the user if they want to abort the Co2 adjustment while its pumping
          continueMessage(message::cancelCo2, sensor::co2GasTime / 60000.0, 0, false, true, false);
          co2RunTime = millis() + sensor::co2GasTime;
          lastTouch = millis() + 5000UL;
        }
      }
    }
    // Refresh the dialog with the remaining gas time
    abortCo2Notification(startCo2Relay, lastTouch, co2RunTime);
  }
}

// Control one or two 3rd party AC fans and a 3rd party AC air heater
// to maintain the air temperature and or humidity.
void airControl() {
  static unsigned long previousMillis = millis();
  static int previousFanOneSpeed = 200;
  static int previousFanTwoSpeed = 200;
  float tempPercent = 0, humPercent = 0;

  if (device::sensorsReady && millis() - previousMillis >= 2000UL) {
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
    }
    // If Co2 has disabled the fans for x durations, check to see if we can turn the fans back on else do nothing
    if (device::co2DisabledFans && device::co2TurnFansBackOnHour == rtc.hour() && device::co2TurnFansBackOnMinute >= rtc.minute())
      device::co2DisabledFans = false;
    else if (device::co2DisabledFans)
      return;

    // Check to see if the temperature and or humidity is out of range of the user targets
    device::controlOptions tempOption = device::SPEED_IDLE;
    device::controlOptions humOption = device::SPEED_IDLE;
    device::controlOptions fanMode = device::SPEED_IDLE;
    const float maxAirTemp = user::convertToF ? user::targetMaxAirTempF : user::targetMaxAirTemp;
    const float minAirTemp = user::convertToF ? user::targetMinAirTempF : user::targetMinAirTemp;
    const float airTemp = user::convertToF ? convertToF(sensor::airTemp) : sensor::airTemp;
    // Check to see if the temperature is out of range
    if (airTemp > maxAirTemp && user::fansControlTemperature) {
      float tempOutOfRange = abs(maxAirTemp - airTemp); 
      tempPercent = tempOutOfRange / (maxAirTemp / 100.0);
    }
    else if (airTemp < minAirTemp && user::fansControlTemperature) {
      float tempOutOfRange = abs(minAirTemp - airTemp);
      tempPercent = tempOutOfRange / (minAirTemp / 100.0);
      //Serial.println(F("SPEED_DOWN tempPercent: %d", tempPercent);
    }

    // Check to see if the humidity is out of range
    if (sensor::humidity > user::targetMaxHumidity && user::fansControlHumidity)  {
      float humOutOfRange = abs(user::targetMaxHumidity - sensor::humidity);
      humPercent = humOutOfRange / (user::targetMaxHumidity / 100.0);
      //Serial.println(F("SPEED_UP humPercent: %d", humPercent);
    }
    else if (sensor::humidity < user::targetMinHumidity && user::fansControlHumidity)  {
      float humOutOfRange = abs(user::targetMinHumidity - sensor::humidity);
      humPercent = humOutOfRange / (user::targetMinHumidity / 100.0);
      //Serial.println(F("SPEED_DOWN humPercent: %d", humPercent);
    }

    // Control the air heater
    static uint8_t prevAirHeaterMin = 69;
    if (rtc.minute() != prevAirHeaterMin) {
      if (airTemp <= minAirTemp && !device::airHeaterIsOn) {
        device::airHeaterIsOn = true;
        if (device::globalDebug)   
          Serial.println(F("air heater on"));
        analogWriteChnl(device::pca9685Channel::airHeater, (device::relayOffState == HIGH ? 0 : 4096));
        saveLogMessage(11); // save log message, air heater on
      }
      else if (airTemp > minAirTemp &&device::airHeaterIsOn) {
        device::airHeaterIsOn = false;
        if (device::globalDebug)   
          Serial.println(F("air heater off"));
        analogWriteChnl(device::pca9685Channel::airHeater, (device::relayOffState == HIGH ? 4096 : 0));
        saveLogMessage(12); // save log message, air heater off
      }
      prevAirHeaterMin = rtc.minute();
    }

    float averageTemp = averageFltArray(sensor::fanTemperatureArray, sensor::fanArrayPos);
    sensor::airStates tempAirState = sensor::IS_SAME;
    if (hasChanged(airTemp, averageTemp, 1)) {
      if (airTemp < averageTemp)
        tempAirState = sensor::IS_FALLING;
      else if (airTemp > averageTemp)
        tempAirState = sensor::IS_RISING;
      //Serial.println(tempAirState);
    }
    float averageHum = averageFltArray(sensor::fanHumidityArray, sensor::fanArrayPos);
    sensor::airStates humAirState = sensor::IS_SAME;
    if (hasChanged(sensor::humidity, averageHum, 1)) {
      if (sensor::humidity < averageHum)
        humAirState = sensor::IS_FALLING;
      else if (sensor::humidity > averageHum)
        humAirState = sensor::IS_RISING;
      //Serial.println(humAirState);
    }

    if (user::fansControlTemperature) {
      if (device::globalDebug) {
        Serial.print(F("Air temp: ")); Serial.print(airTemp, 2); Serial.print(F("c, average temp: ")); Serial.print(averageTemp, 2); Serial.print(F("c, over ")); Serial.print(sensor::fanArrayPos); Serial.println(F(" reads @ 2 sec per read"));
      }     
      adjustFanMode(airTemp, tempOption, tempAirState, "air temp", tempPercent, minAirTemp, maxAirTemp);
    }
    if (user::fansControlHumidity) {
      if (device::globalDebug) {
        Serial.print(F("Humidity: ")); Serial.print(sensor::humidity, 2); Serial.print(F("%, averageHum: ")); Serial.print(averageHum, 2); Serial.println(F("%, over ")); Serial.print(sensor::fanArrayPos); Serial.println(F(" reads @ 2 sec per read"));
      }   
      adjustFanMode(sensor::humidity, humOption, humAirState, "humidity", humPercent, user::targetMinHumidity, user::targetMaxHumidity);
    }

    // Temperature has priority over humidity
    if (tempOption != device::SPEED_IDLE || !user::fansControlHumidity) {
      fanMode = tempOption;
      if (device::globalDebug) {
        Serial.print(F("Fans set to control temperature")); Serial.print(user::fansControlHumidity ? F(", as temperature takes priority\n") : F("\n"));
      }     
    }
    else {
      fanMode = humOption;
      if (device::globalDebug) {
        Serial.print(F("Fans set to control humidity")); Serial.print(user::fansControlTemperature ? F(", as the temperature is in range\n") : F("\n"));
      }        
    }
    
    // adjust the fan speeds
    if (fanMode == device::SPEED_MAX)
      device::fanOneSpeed = user::targetMaxFanOneSpeed;
    else if (fanMode == device::SPEED_MIN)
      device::fanOneSpeed = user::targetMinFanOneSpeed;
    else if (fanMode == device::SPEED_UP && device::fanOneSpeed < user::targetMaxFanOneSpeed)
      device::fanOneSpeed++;
    else if (fanMode == device::SPEED_UP_SLOW && device::fanOneSpeed < user::targetMaxFanOneSpeed)
      device::fanOneSpeed+=0.25;
    else if (fanMode == device::SPEED_DOWN && device::fanOneSpeed > user::targetMinFanOneSpeed)
      device::fanOneSpeed--;
    else if (fanMode == device::SPEED_DOWN_SLOW && device::fanOneSpeed > user::targetMinFanOneSpeed)
      device::fanOneSpeed-=0.25;

    if (fanMode == device::SPEED_MAX)
      device::fanTwoSpeed = user::targetMaxFanTwoSpeed;
    else if (fanMode == device::SPEED_MIN)
      device::fanTwoSpeed = user::targetMinFanTwoSpeed;
    else if (fanMode == device::SPEED_UP && device::fanTwoSpeed < user::targetMaxFanTwoSpeed)
      device::fanTwoSpeed++;
    else if (fanMode == device::SPEED_UP_SLOW && device::fanTwoSpeed < user::targetMaxFanTwoSpeed)
      device::fanTwoSpeed+=0.25;
    else if (fanMode == device::SPEED_DOWN && device::fanTwoSpeed > user::targetMinFanTwoSpeed)
      device::fanTwoSpeed--;
    else if (fanMode == device::SPEED_DOWN_SLOW && device::fanTwoSpeed > user::targetMinFanTwoSpeed)
      device::fanTwoSpeed-=0.25;

    // Check to see if the fan speed is less than or greater than the min or max speed and if so adjust the current fans speed
    if (device::fanOneSpeed < user::targetMinFanOneSpeed) 
      device::fanOneSpeed = user::targetMinFanOneSpeed;   
    else if (device::fanOneSpeed > user::targetMaxFanOneSpeed) 
      device::fanOneSpeed = user::targetMaxFanOneSpeed;   
    if (device::fanTwoSpeed < user::targetMinFanTwoSpeed) 
      device::fanTwoSpeed = user::targetMinFanTwoSpeed;  
    else if (device::fanTwoSpeed > user::targetMaxFanTwoSpeed) 
      device::fanTwoSpeed = user::targetMaxFanTwoSpeed;
    


    // Send the new fan speeds to the Atmel328P
    if ((int)device::fanOneSpeed != previousFanOneSpeed || (int)device::fanTwoSpeed != previousFanTwoSpeed) {
      if (device::globalDebug) {
        Serial.print(F("Fan one speed: ")); Serial.print(device::fanOneSpeed, 2); Serial.print(F("%, fan two speed: ")); Serial.print(device::fanTwoSpeed, 2); Serial.println(F("%"));
      }
      setFanSpeeds((uint8_t)device::fanOneSpeed, (uint8_t)device::fanTwoSpeed);
      previousFanOneSpeed = (int)device::fanOneSpeed;
      previousFanTwoSpeed = (int)device::fanTwoSpeed;
    }
    previousMillis = millis();
  }
}

// Control the waters EC and or PH and or PMM
void waterEcPhControl() {
  enum adjustmentModes {UNADJUSTED, ADJUSTED_EC, ADJUSTED_PH};
  static adjustmentModes adjustmentMode = UNADJUSTED;
  //static adjustmentModes lastAdjustmentMode = UNADJUSTED;
  // Every hour increment the dosing hour counter
  if (device::previousDosingHour != rtc.hour() && rtc.minute() >= device::previousDosingMinute) {
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
    }
    device::dosingTimerHourCounter++;
    device::previousDosingHour = rtc.hour();
    if (device::globalDebug) {
      Serial.print(F("Dosing Timer Hour Counter: ")); Serial.println(device::dosingTimerHourCounter);
    }
  }
  // If the dosing hour counter is greather than or equals the dosingInterval then run the dosing logic
  if (device::dosingTimerHourCounter >= user::dosingInterval) {
    if (!device::currentlyDosing && device::sensorsReady) {
      if (adjustmentMode == UNADJUSTED || adjustmentMode == ADJUSTED_PH) {
        if (user::convertToTds)
          adjustWaterEc();
        else
          adjustWaterTds();
        adjustmentMode = ADJUSTED_EC;
      }
      else {
        adjustWaterPh();
        adjustmentMode = ADJUSTED_PH;
      }
    }
  }
}

void adjustWaterEc() {
  if (device::globalDebug)
    Serial.println(F("Attempting to adjust the EC..."));
  if (sensor::ec < user::targetMinEc) {
    float percentage = percentOutOfRange(user::targetMinEc, sensor::ec);
    if (device::globalDebug) {
      Serial.print(F("EC is lower than min target by: ")); Serial.print(percentage, 2); Serial.println(F("%"));
    }
    // Display a notification to cancel or continue with the starting of the dosing
    unsigned long lastTouch = millis();
    launchDosingNotification(percentage, 1, lastTouch);
    // Work out dosing amount
    int dosingAmount = 0;
    if (device::continueDosing != device::CANCEL) {
      float dosingMls[6] {0, 0, 0, 0, 0, 0};
      bool enabledDosers[6] {false, false, false, false, false, false};
      const uint8_t currentDoserModes[] {user::doserOneMode, user::doserTwoMode, user::doserThreeMode, user::doserFourMode, user::doserFiveMode, user::doserSixMode};
      const int currentDoserMls[] {user::doserOneMills, user::doserTwoMills, user::doserThreeMills, user::doserFourMills, user::doserFiveMills, user::doserSixMills};
      if (user::doserOneMode == device::DOSER_EC || user::doserOneMode == device::DOSER_EC_OP) 
        enabledDosers[0] = true;
      if (user::doserTwoMode == device::DOSER_EC || user::doserTwoMode == device::DOSER_EC_OP)
        enabledDosers[1] = true;
      if (user::doserThreeMode == device::DOSER_EC || user::doserThreeMode == device::DOSER_EC_OP)
        enabledDosers[2] = true;
      if (user::doserFourMode == device::DOSER_EC || user::doserFourMode == device::DOSER_EC_OP) 
        enabledDosers[3] = true;
      if (user::numberOfDosers >= 5 && (user::doserFiveMode == device::DOSER_EC || user::doserFiveMode == device::DOSER_EC_OP)) 
        enabledDosers[4] = true;
      if (user::numberOfDosers >= 6 && (user::doserSixMode == device::DOSER_EC || user::doserSixMode == device::DOSER_EC_OP))
        enabledDosers[5] = true;
      int numEnabledDosers = 0;
      for (int i = 0; i < 6; i++) {
        if (enabledDosers[i] && currentDoserModes[i] == device::DOSER_EC)
          numEnabledDosers++;
      }
      //    
      saveLogMessage(9);
      //
      if (user::ecDosingMode == user::PRECISE) {
        if (device::globalDebug)
          Serial.println(F("EC/TDS dosing mode set to precise"));
        // Work out the exact amout of nutrients to dose
        float waterVolumeLtrs = (user::convertToInches ? sensor::waterVolumeLtrs : convertGallonsToLtrs(sensor::waterVolumeGallons));
        dosingAmount = (user::targetMinEc - sensor::ec) * waterVolumeLtrs / sensor::ecSolution;
        float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
        for (int i = 0; i < 6; i++) {
          if (enabledDosers[i] && currentDoserModes[i] == device::DOSER_EC)
            dosingMls[i] = mlsPerDoser;
          else if (enabledDosers[i] && currentDoserModes[i] == device::DOSER_EC_OP)
            dosingMls[i] = currentDoserMls[i];
        }
        if (device::globalDebug) {
          Serial.print(F("Calculated ")); Serial.print(dosingAmount, 2); Serial.print(F(" mls of nutrients, divided by ")); Serial.print(numEnabledDosers); Serial.print(F(" EC dosers = ")); Serial.println(mlsPerDoser, 2);
        }
      }
      //
      else {
        if (device::globalDebug)
          Serial.println(F("EC/TDS dosing mode set to incremental"));
        for (int i = 0; i < 6; i++) {
          if (enabledDosers[i])
            dosingMls[i] = currentDoserMls[i];
        }
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        if (device::globalDebug)
          Serial.println(F("Starting dosing..."));
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 1, lastTouch);
      }     
    }
    device::dosingTimerHourCounter = 0;
  }
}

void adjustWaterTds() {
  if (device::globalDebug)
    Serial.println(F("Attempting to adjust the TDS..."));
  if (sensor::tds < user::targetMinTds) {
    float percentage = percentOutOfRange(user::targetMinTds, sensor::tds);
    if (device::globalDebug) {
      Serial.print(F("TDS is lower than min target by: ")); Serial.print(percentage, 2); Serial.println(F("%"));
    }
    // Display a notification to cancel or continue with the starting of the dosing
    unsigned long lastTouch = millis();
    launchDosingNotification(percentage, 1, lastTouch);
    // Work out dosing amount
    int dosingAmount = 0;
    if (device::continueDosing != device::CANCEL) {
      float dosingMls[6] {0, 0, 0, 0, 0, 0};
      bool enabledDosers[6] {false, false, false, false, false, false};
      const uint8_t currentDoserModes[] {user::doserOneMode, user::doserTwoMode, user::doserThreeMode, user::doserFourMode, user::doserFiveMode, user::doserSixMode};
      const int currentDoserMls[] {user::doserOneMills, user::doserTwoMills, user::doserThreeMills, user::doserFourMills, user::doserFiveMills, user::doserSixMills};
      if (user::doserOneMode == device::DOSER_EC || user::doserOneMode == device::DOSER_EC_OP) 
        enabledDosers[0] = true;
      if (user::doserTwoMode == device::DOSER_EC || user::doserTwoMode == device::DOSER_EC_OP)
        enabledDosers[1] = true;
      if (user::doserThreeMode == device::DOSER_EC || user::doserThreeMode == device::DOSER_EC_OP)
        enabledDosers[2] = true;
      if (user::doserFourMode == device::DOSER_EC || user::doserFourMode == device::DOSER_EC_OP) 
        enabledDosers[3] = true;
      if (user::numberOfDosers >= 5 && (user::doserFiveMode == device::DOSER_EC || user::doserFiveMode == device::DOSER_EC_OP)) 
        enabledDosers[4] = true;
      if (user::numberOfDosers >= 6 && (user::doserSixMode == device::DOSER_EC || user::doserSixMode == device::DOSER_EC_OP))
        enabledDosers[5] = true;
      int numEnabledDosers = 0;
      for (int i = 0; i < 6; i++) {
        if (enabledDosers[i] && currentDoserModes[i] == device::DOSER_EC)
          numEnabledDosers++;
      }
      //    
      saveLogMessage(9);
      //
      if (user::ecDosingMode == user::PRECISE) {
        if (device::globalDebug)
          Serial.println(F("EC/TDS dosing mode set to precise"));
        // Work out the exact amout of nutrients to dose
        float waterVolumeLtrs = (user::convertToInches ? sensor::waterVolumeLtrs : convertGallonsToLtrs(sensor::waterVolumeGallons));
        dosingAmount = (user::targetMinTds - sensor::tds) * waterVolumeLtrs / sensor::tdsSolution;
        float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
        for (int i = 0; i < 6; i++) {
          if (enabledDosers[i] && currentDoserModes[i] == device::DOSER_EC)
            dosingMls[i] = mlsPerDoser;
          else if (enabledDosers[i] && currentDoserModes[i] == device::DOSER_EC_OP)
            dosingMls[i] = currentDoserMls[i];
        }
        if (device::globalDebug) {
          Serial.print(F("Calculated ")); Serial.print(dosingAmount, 2); Serial.print(F(" mls of nutrients, divided by ")); Serial.print(numEnabledDosers); Serial.print(F(" EC dosers = ")); Serial.println(mlsPerDoser, 2);
        }
      }
      //
      else {
        if (device::globalDebug)
          Serial.println(F("EC/TDS dosing mode set to incremental"));
        for (int i = 0; i < 6; i++) {
          if (enabledDosers[i])
            dosingMls[i] = currentDoserMls[i];
        }
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        if (device::globalDebug)
          Serial.println(F("Starting dosing..."));
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 1, lastTouch);
      }     
    }
    device::dosingTimerHourCounter = 0;
  }
}

void adjustWaterPh() {
  if (device::globalDebug)
    Serial.println(F("Attempting to adjust the PH..."));
  if (sensor::ph < user::targetMinPh || sensor::ph > user::targetMaxPh) {
    float percentage = percentOutOfRange(sensor::ph < user::targetMinPh ? user::targetMinPh : user::targetMaxPh, sensor::ph);
    if (device::globalDebug) {
      Serial.print(F("PH is ")); Serial.print(sensor::ph < user::targetMinPh ? F("lower") : F("higher")); Serial.print(F(" than min target by ")); Serial.println(percentage, 2);
    }
    // Display a notification to cancel or continue with the starting of the dosing
    unsigned long lastTouch = millis();
    launchDosingNotification(percentage, 0, lastTouch);
    // Work out dosing amount
    int dosingAmount = 0;
    if (device::continueDosing != device::CANCEL) {
      bool enabledDosers[6] {false, false, false, false, false, false};
      float dosingMls[6] {0, 0, 0, 0, 0, 0};
      // Adjust PH up
      if (sensor::ph < user::targetMinPh) {
        if (user::doserOneMode == device::DOSER_PH_UP) 
          enabledDosers[0] = true;
        else if (user::doserTwoMode == device::DOSER_PH_UP) 
          enabledDosers[1] = true;
        else if (user::doserThreeMode == device::DOSER_PH_UP) 
          enabledDosers[2] = true;
        else if (user::doserFourMode == device::DOSER_PH_UP) 
          enabledDosers[3] = true;
        else if (user::numberOfDosers >= 5 && user::doserFiveMode == device::DOSER_PH_UP) 
          enabledDosers[4] = true;
        else if (user::numberOfDosers >= 6 && user::doserSixMode == device::DOSER_PH_UP) 
          enabledDosers[5] = true;
      }
      // Adjust PH down
      else if (sensor::ph > user::targetMinPh) {
        if (user::doserOneMode == device::DOSER_PH_DOWN) 
          enabledDosers[0] = true;
        else if (user::doserTwoMode == device::DOSER_PH_DOWN) 
          enabledDosers[1] = true;
        else if (user::doserThreeMode == device::DOSER_PH_DOWN) 
          enabledDosers[2] = true;
        else if (user::doserFourMode == device::DOSER_PH_DOWN) 
          enabledDosers[3] = true;
        else if (user::numberOfDosers >= 5 && user::doserFiveMode == device::DOSER_PH_DOWN) 
          enabledDosers[4] = true;
        else if (user::numberOfDosers >= 6 && user::doserSixMode == device::DOSER_PH_DOWN) 
          enabledDosers[5] = true;
      }   
      int numEnabledDosers = 0;
      for (int i = 0; i < 6; i++) {
        if (enabledDosers[i])
          numEnabledDosers++;
      }
      sensor::ph < user::targetMinPh ? saveLogMessage(8) : saveLogMessage(7);
      float waterVolumeLtrs = (user::convertToInches ? sensor::waterVolumeLtrs : convertGallonsToLtrs(sensor::waterVolumeGallons));
      if (user::phDosingMode == user::PRECISE) {
        if (device::globalDebug)
          Serial.println(F("PH dosing mode set to precise"));
        if (sensor::ph < user::targetMinPh) {
          // Work out the exact amout of PH up to dose
          dosingAmount = (user::targetMinPh - sensor::ph) * waterVolumeLtrs / sensor::phDownSolution;
          float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
          for (int i = 0; i < 6; i++) {
            if (enabledDosers[i])
              dosingMls[i] = mlsPerDoser;
          }
          if (device::globalDebug) {
            Serial.print(F("Calculated ")); Serial.print(dosingAmount, 2); Serial.print(F(" mls of PH up solution, divided by ")); Serial.print(numEnabledDosers); Serial.print(F(" PH up dosers = ")); Serial.println(mlsPerDoser, 2);
          }
        }
        else if (sensor::ph > user::targetMaxPh) {
          // Work out the exact amout of PH down to dose
          dosingAmount = (sensor::ph - user::targetMaxPh) * waterVolumeLtrs / sensor::phUpSolution;
          float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
          for (int i = 0; i < 6; i++) {
            if (enabledDosers[i])
              dosingMls[i] = mlsPerDoser;
          }
          if (device::globalDebug) {
            Serial.print(F("Calculated ")); Serial.print(dosingAmount, 2); Serial.print(F(" mls of PH down solution, divided by ")); Serial.print(numEnabledDosers); Serial.print(F(" PH down dosers = ")); Serial.println(mlsPerDoser, 2);
          }
        } 
      }
      else {
        Serial.println(F("PH dosing mode set to incremental"));
        if (enabledDosers[0]) 
          dosingMls[0] = user::doserOneMills;
        else if (enabledDosers[1]) 
          dosingMls[1] = user::doserTwoMills;
        else if (enabledDosers[2]) 
          dosingMls[2] = user::doserThreeMills;
        else if (enabledDosers[3]) 
          dosingMls[3] = user::doserFourMills;
        else if (enabledDosers[4]) 
          dosingMls[4] = user::doserFiveMills;
        else if (enabledDosers[5]) 
          dosingMls[5] = user::doserSixMills; 
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        if (device::globalDebug)
          Serial.println(F("Starting dosing..."));
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 0, lastTouch);
      }        
    }
    device::dosingTimerHourCounter = 0;
  }
}

// Return the percent out of range
float percentOutOfRange(const float & a_setPoint, const float & a_val) {
  float outOfRange = abs(a_setPoint - a_val);
  float percent = a_setPoint / 100.0;
  return outOfRange / percent;
}

void runDosers(bool* a_enabledDosers, float* a_dosingMls, const float a_percent, const int a_dosingMode, unsigned long a_lastTouch) {
  uint8_t previousDoserNum = 0;
  device::currentDoserNum = 0;
  const char* str[3] = {"PH", "EC", "TDS"};
  unsigned long previousDoserMillis = millis();
  while (device::currentlyDosing) { // timing is critical we must use a while loop
    // show continue dialog
    if (device::currentDoserNum != previousDoserNum) {
      abortMessage(message::cancelDosing, str[a_dosingMode], a_percent, device::currentDoserNum, 2);
      previousDoserNum = device::currentDoserNum;
    }
    if (a_enabledDosers[0])
      a_enabledDosers[0] = runDoser(1, device::pca9685Channel::doserOne, user::doserOneSpeed, a_dosingMls[0], previousDoserMillis);
    else if (a_enabledDosers[1])
      a_enabledDosers[1] = runDoser(2, device::pca9685Channel::doserTwo, user::doserTwoSpeed, a_dosingMls[1], previousDoserMillis);
    else if (a_enabledDosers[2])
      a_enabledDosers[2] = runDoser(3, device::pca9685Channel::doserThree, user::doserThreeSpeed, a_dosingMls[2], previousDoserMillis);
    else if (a_enabledDosers[3])
      a_enabledDosers[3] = runDoser(4, device::pca9685Channel::doserFour, user::doserFourSpeed, a_dosingMls[3], previousDoserMillis);
    else if (a_enabledDosers[4])
      a_enabledDosers[4] = runDoser(5, device::pca9685Channel::doserFive, user::doserFiveSpeed, a_dosingMls[4], previousDoserMillis);
    else if (a_enabledDosers[5])
      a_enabledDosers[5] = runDoser(6, device::pca9685Channel::doserSix, user::doserSixSpeed, a_dosingMls[5], previousDoserMillis);
    else if (device::currentlyDosing) {
      if (device::globalDebug)
        Serial.println(F("Dosing complete"));
      device::currentlyDosing = false;
      clearPage();
      display::refreshPage = true;
    }
    // touch event - cancel button
    runDosersTouch(a_enabledDosers, a_lastTouch);
    static unsigned long previousMillis = millis();
    if (millis() - previousMillis >= 1000UL) {
      rtc.refresh();
      cyclicTimers();
      previousMillis = millis();
    }  
  }
}

// Run a given doser for a_mls * 1000 on a_doserPin at a_doserSpeed
bool runDoser(const uint8_t& a_doserNum, const uint8_t& a_doserChnl, const int& a_doserSpeed, const float& a_mls, unsigned long& a_previousDoserMillis) {
  static unsigned long debugPreviousMillis = 0;
  if (millis() - a_previousDoserMillis <= 1000UL * a_mls) {
    if (device::globalDebug && a_previousDoserMillis != debugPreviousMillis) {
      Serial.print(F("Starting doser: ")); Serial.print(a_doserNum); Serial.print(F(", pumping ")); Serial.print(a_mls, 2); Serial.println(F("mls"));
      debugPreviousMillis = a_previousDoserMillis;
    }
    analogWriteChnl(a_doserChnl, a_doserSpeed);
    device::currentDoserNum = a_doserNum;
  }
  else {
    if (device::globalDebug) {
      Serial.print(F("Stopping doser: ")); Serial.println(a_doserNum);
      Serial.print(F("Doser run time: ")); Serial.println(millis() - a_previousDoserMillis);
    }
    analogWriteChnl(a_doserChnl, 0);
    a_previousDoserMillis = millis();
    return false;
  }
  return true;
}
/*
void humidifier() {
  if (device::globalDebug) {
    Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
  }
  static bool humidifierIsOn = false;
  if (sensor::humidity < user::targetMinHumidity) {
    Serial.println(F("Humidity is less than min target, turning humidifier on"));
    //digitalWrite(pin::humidifier, HIGH);
    humidifierIsOn = true;
  }
  else {
    Serial.println(F("Turning humidifier off"));
    //digitalWrite(pin::humidifier, LOW);     
    humidifierIsOn = false;
  }
}
*/
void acControl() {
  if (millis() - device::irPreviousMillis >= (user::acPollTime * 1000UL)) {
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
    }
    float tempPercent = 0, humPercent = 0;
    const float maxAirTemp = user::convertToF ? user::targetMaxAirTempF : user::targetMaxAirTemp;
    const float minAirTemp = user::convertToF ? user::targetMinAirTempF : user::targetMinAirTemp;
    const float airTemp = user::convertToF ? convertToF(sensor::airTemp) : sensor::airTemp;
    // Check to see if the temperature is out of range
    if (airTemp > maxAirTemp) {
      float tempOutOfRange = abs(maxAirTemp - airTemp); 
      tempPercent = tempOutOfRange / (maxAirTemp / 100.0);
    }
    else if (airTemp < minAirTemp) {
      float tempOutOfRange = abs(minAirTemp - airTemp);
      tempPercent = tempOutOfRange / (minAirTemp / 100.0);
      //Serial.println(F("SPEED_DOWN tempPercent: %d", tempPercent);
    }
    // Check to see if the humidity is out of range
    if (sensor::humidity > user::targetMaxHumidity)  {
      float humOutOfRange = abs(user::targetMaxHumidity - sensor::humidity);
      humPercent = humOutOfRange / (user::targetMaxHumidity / 100.0);
      //Serial.println(F("SPEED_UP humPercent: %d", humPercent);
    }
    else if (sensor::humidity < user::targetMinHumidity)  {
      float humOutOfRange = abs(user::targetMinHumidity - sensor::humidity);
      humPercent = humOutOfRange / (user::targetMinHumidity / 100.0);
      //Serial.println(F("SPEED_DOWN humPercent: %d", humPercent);
    }
    if (device::globalDebug) {
      Serial.print(F("Temp out by: ")); Serial.print(tempPercent); Serial.println(F("%"));
      Serial.print(F("Hum out by ")); Serial.print(humPercent); Serial.println(F("%"));
    }
    if (user::disableAcTemp) {
      tempPercent = 0;
      if (device::globalDebug)
        Serial.println(F("Temperature control is disabled, setting temperature percent to zero!"));
    }
    if (user::disableAcHum) {
      humPercent = 0;
      if (device::globalDebug) 
        Serial.println(F("Humidity control is disabled, setting humidity percent to zero!"));
    }
    // Temperature or humidity is out, so work out which IR command needs to be sent
    // Check if the tempPercent > humPercent
    int percent = 0, fanMode = 0;
    device::acModes acMode = device::AC_UNSET;
    if (device::globalDebug) 
      Serial.print(F("Setting AC mode to "));
    if (tempPercent >= humPercent && tempPercent >= 0.01) {
      if (airTemp > maxAirTemp)  { //  && acMode != AC_COOL
        if (device::globalDebug) 
          Serial.println(F("cool"));
        acMode = device::AC_COOL;
      }
      else { //  if (acMode != AC_HEAT)
        if (device::globalDebug) 
          Serial.println(F("heat"));
        acMode = device::AC_HEAT;
      }
      percent = tempPercent;
    }
    // Send the humidity IR command
    else if (humPercent >= 0.01) { // && acMode != AC_DEHUM
      if (device::globalDebug) 
        Serial.println(F("dehum"));
      acMode = device::AC_DEHUM;
      percent = humPercent;
    }
    else { // if (acMode != AC_FAN)
      Serial.println(F("fan"));
      acMode = device::AC_FAN;
      percent = 8;
    }
    float temperature = minAirTemp + (maxAirTemp - minAirTemp) / 2.0;
    if (device::globalDebug) {
      Serial.print("Target temperature: "); Serial.print(temperature); user::convertToF ? Serial.println(F(" F")) : Serial.println(F(" C"));
      Serial.print("Fan mode: ");
    }
    if (percent <= 4) {
      if (device::globalDebug) 
        Serial.println(F("low"));
      fanMode = 0;
    }
    else if (percent <= 8) {
      if (device::globalDebug) 
        Serial.println(F("medium"));
      fanMode = 1;
    }
    else {
      if (device::globalDebug) 
        Serial.println(F("high"));
      fanMode = 2;
    }
    sendIRCode(acMode, (int)temperature, fanMode);
    device::irPreviousMillis = millis();
  }
}

void sendIRCode(const device::acModes a_acMode, const int a_temperature, const int a_fanMode) {
  const decode_type_t protocol = (decode_type_t)irTitleIndexs[user::selectedIrPos];
  ac.next.protocol = protocol;  // Change the protocol used.
  if (device::globalDebug) {
    Serial.print("Loaded protocol: "); Serial.println(typeToString(ac.next.protocol));
  }
  //ac.next.model = 1;  // Some A/Cs have different models. Try just the first.
  if (a_acMode == device::AC_COOL)
    ac.next.mode = stdAc::opmode_t::kCool;
  else if (a_acMode == device::AC_HEAT)
    ac.next.mode = stdAc::opmode_t::kHeat;
  else if (a_acMode == device::AC_DEHUM)
    ac.next.mode = stdAc::opmode_t::kDry;
  else // if (a_acMode == device::AC_FAN)
    ac.next.mode = stdAc::opmode_t::kFan;
  if (a_fanMode == 0)
    ac.next.fanspeed = stdAc::fanspeed_t::kLow;
  else  if (a_fanMode == 1)
    ac.next.fanspeed = stdAc::fanspeed_t::kMedium;
  else
    ac.next.fanspeed = stdAc::fanspeed_t::kHigh;
  ac.next.celsius = !user::convertToF;  // Use Celsius for temp units. False = Fahrenheit
  ac.next.degrees = a_temperature;
  ac.next.swingv = user::acSwingVertical ? stdAc::swingv_t::kAuto : stdAc::swingv_t::kOff;  // Don't swing the fan up or down.
  ac.next.swingh = user::acSwingHorizontal ? stdAc::swingh_t::kAuto : stdAc::swingh_t::kOff;  // Don't swing the fan left or right.
  ac.next.light = false;  // Turn off any LED/Lights/Display that we can.
  ac.next.beep = true;  // Turn off any beep from the A/C if we can.
  ac.next.econo = false;  // Turn off any economy modes if we can.
  ac.next.filter = false;  // Turn off any Ion/Mold/Health filters if we can.
  ac.next.turbo = user::acTurboMode;  // Don't use any turbo/powerful/etc modes.
  ac.next.quiet = false;  // Don't use any quiet/silent/etc modes.
  ac.next.sleep = -1;  // Don't set any sleep time or modes.
  ac.next.clean = false;  // Turn off any Cleaning options if we can.
  ac.next.clock = -1;  // Don't set any current time if we can avoid it.
  ac.next.power = true;  // Set poer state to on
  ac.sendAc();  // Send the message.
  if (device::globalDebug) 
    Serial.println(F("Sent IR cmd"));
}