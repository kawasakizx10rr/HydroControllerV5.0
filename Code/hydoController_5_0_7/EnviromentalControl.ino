void envriomentalControl() {
 // Get current date time
 static uint32_t previousMillis = millis();
  if (millis() - previousMillis >= 1000UL) {
    rtc.refresh();
    previousMillis = millis();
  }
  relayTimers();
  waterLevelControl();
  waterTemperatureControl();
  co2Control();
  airControl();
  acControl();
  waterEcPhControl();
}

// Control the water level
void waterLevelControl() {
  bool startDraining = false, startRefilling = false;
  static uint8_t previousDate = 0;
  uint8_t continueRefilling = device::NOT_SET;
  uint8_t continueDraining = device::NOT_SET;
  // wait until the sensors have data
  if (device::sensorsReady) {
    // auto drain and refill tank on set dates and time
    if (!user::disableDrainAndRefill && rtc.day() != previousDate) {
      for (uint8_t i = 0; i < 31; i++) {
        if (bitRead(user::autoFillDays, i) && rtc.day() == i + 1 && rtc.hour() == user::autoFillHour && rtc.minute() >= user::autoFillMinute) {
          if (device::globalDebug) {  
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
      display::lastTouchMillis = millis();
    }
    // Show a dialog asking the user if they want to abort the drain process while its draining
    if (startDraining && continueDraining != device::CANCEL) {
      if (device::globalDebug)
        Serial.println(F("About to show the drain abort dialog, and start draining"));
      const uint16_t waterTarget = user::convertToInches ? user::targetMinWaterHeight / 2.5 : user::targetMinWaterHeight;
      abortMessage(message::cancelDraining, user::convertToInches ? "\"" : "cm", waterTarget, -1, -1, 1, false);
    }
    // Start draining the water from the tank
    int16_t previousWaterLevel = sensor::waterLevel;
    device::prevMillis = millis();
    bool outletPumpIsOn = false;
    while (startDraining && continueDraining != device::CANCEL) {
      if (sensor::waterLevel > 0) { // drain tank till empty
        // Turn drain pump relay on
        if (!outletPumpIsOn) {
          if (device::globalDebug)
            Serial.println(F("Turning on the outlet pump"));  
           analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 0 : 4096));
          outletPumpIsOn = true;
        }      
        // timer checking water level is still decresing else bail after 1 minute * drainTimeout
        if (millis() - device::prevMillis >= 60000UL * user::drainTimeout) { // put timer value in settings GUI !
          startDraining = false;
          startRefilling = true;
          if (device::globalDebug)
            Serial.print(F("Water level unchanged for ")); Serial.print(user::drainTimeout); Serial.println(F(" minutes, turning off the outlet pump"));
        }
        // Update water height every 100ms
        if (millis() - device::sensorPreviousMillis >= 100UL) {
          if (user::heightSensor != user::ETAPE)
            sensor::waterLevel = sensor::emptyWaterTankDepth - getWaterHeight();
          else
            sensor::waterLevel = getWaterHeight();
          if (sensor::waterLevel < 0)
            sensor::waterLevel = 0;
          // CHeck if the water level has changed, if so reset the timeout
          if (sensor::waterLevel < previousWaterLevel) {
            previousWaterLevel = sensor::waterLevel;
            device::prevMillis = millis();
          }      
          device::sensorPreviousMillis = millis();
        }
      }
      else {
        if (device::globalDebug)
          Serial.println(F("Draining complete"));
        startDraining = false;
        startRefilling = true;
      }
      // cancel button touch event for abort draining dialog
      const uint16_t startX = 166, startY = 166;
      if (tft.touched()) {
        tft.touchReadPixel(&display::touch_x, &display::touch_y);
        if (millis() - display::lastTouchMillis >= 5000UL) {
          if (display::touch_x >= startX + 200 && display::touch_x <= startX + 400 && display::touch_y >= startY + 200 && display::touch_y <= startY + 250) { // Cancel
            if (device::globalDebug)
              Serial.println(F("Water drain aborted"));
            beep();
            startDraining = false;
            clearPage();
            display::refreshPage = true;
          }
        }
      }
      timedEvents(); 
    }
    // Turn off the outlet pump
    if (outletPumpIsOn) {
      if (device::globalDebug)
        Serial.println(F("Turning off the outlet pump"));  
      analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
      outletPumpIsOn = false;
    }
    // refill tank when water is low at any date or time
    if (!user::disableDrainAndRefill && !startRefilling) {
      static uint32_t previousDelayMillis = millis(); // give the user 5 minutes to disable "refill When Low" else if the water level is below min this will keep showing a dialog over and over..
      if (millis() - previousDelayMillis >= 300000UL) {
        if ((sensor::waterLevel < user::targetMinWaterHeight && !user::convertToInches) || (convertToInches(sensor::waterLevel) < user::targetMinWaterHeightInches && user::convertToInches)) {
          startRefilling = true;
          if (device::globalDebug) {  
            Serial.println(F("The water level is below the min target, starting refill process"));
          }         
        }
        previousDelayMillis = millis();
      }
    }
    // Show a dialog asking the user if they want to cancel the refill process before it starts
    launchRefillNotification(startRefilling, continueRefilling);
    // If the user did not click cancel then start filling the tank
    if (startRefilling && continueRefilling != device::CANCEL) {
      // Show a dialog asking the user if they want to abort the refill process while its refilling
      if (device::globalDebug)
        Serial.println(F("About to show refill abort dialog, and refill tank"));
      const uint16_t waterTarget = user::convertToInches ? user::targetMaxWaterHeightInches : user::targetMaxWaterHeight;
      abortMessage(message::cancelRefilling, user::convertToInches ? "\"" : "cm", waterTarget, -1, -1, 1, false);
      saveLogMessage(2);
      // setup parameters
      previousWaterLevel = sensor::waterLevel;
      display::lastTouchMillis = millis();
      device::prevMillis = millis();
      bool startRefilling = true, runRefillDosers = false, inletPumpIsOn = true;
      // turn on the inlet water pump
      if (device::globalDebug)
        Serial.println(F("Turning on the inlet pump"));   
      analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 0 : 4096));
      // Refill the tank and run the refill dosers
      while (refillTank(device::prevMillis, previousWaterLevel, startRefilling, runRefillDosers, inletPumpIsOn)) {
        timedEvents(); 
      }    
      // Refill complete
      device::dosingTimerHourCounter = 0;
      clearPage();
      display::refreshPage = true;
    }
  }
}

// Refill the tanks water and run doers all available dosers
uint8_t refillTank(uint32_t& a_previousMillis, int16_t& a_previousWaterLevel, bool& a_startRefilling, bool& a_runRefillDosers, bool& a_inletPumpIsOn) {
  bool rtn = true;
  static bool enabledDosers[6] {false, false, false, false, false, false};
  // Update the water height every 100ms
  if (millis() - device::sensorPreviousMillis >= 100UL) {
    if (user::heightSensor != user::ETAPE)
      sensor::waterLevel = sensor::emptyWaterTankDepth - getWaterHeight();
    else
      sensor::waterLevel = getWaterHeight();
    if (sensor::waterLevel < 0)
      sensor::waterLevel = 0;
    // Check to see if the water level is increasing
    if (sensor::waterLevel > a_previousWaterLevel) {
      a_previousWaterLevel = sensor::waterLevel;
      a_previousMillis = millis();
    }
    device::sensorPreviousMillis = millis();
  }
  // Check if the water height has reached the target or timed out after user::drainTimeout minutes
  if (a_startRefilling) {  
    const float waterLevel = user::convertToInches ? convertToInches(sensor::waterLevel) : sensor::waterLevel;
    const uint16_t waterTarget = user::convertToInches ? user::targetMaxWaterHeightInches : user::targetMaxWaterHeight;
    // If its reached the max height then stop the inlet pump and start the dosing pumps 
    if (waterLevel >= waterTarget) {
      if (device::globalDebug)
        Serial.println(F("Finished pumping in water, quiting refill process and starting dosing"));
      a_startRefilling = false;
      a_runRefillDosers = true;
      for (uint8_t i = 0; i < user::numberOfDosers; i++)
        enabledDosers[i] = true;
    }
    // timer checking water level is still incresing else bail after 1 minute * drainTimeout
    if (millis() - a_previousMillis >= 60000UL * user::drainTimeout) {
      if (device::globalDebug) {
        Serial.print(F("Water level unchanged for ")); Serial.print(user::drainTimeout); Serial.println(F(" minutes, quiting refill process and aborting dosing"));
      }
      saveLogMessage(15);
      a_startRefilling = false;
      a_runRefillDosers = false;
      rtn = false;
    }
  }
  // touch event - cancel button
  const uint16_t startX = 166, startY = 166;
  if (tft.touched()) {
    tft.touchReadPixel(&display::touch_x, &display::touch_y);
    if (millis() - display::lastTouchMillis >= 3000UL) {
      if (display::touch_x >= startX + 200 && display::touch_x <= startX + 400 && display::touch_y >= startY + 200 && display::touch_y <= startY + 250) { // Cancel      
        if (device::globalDebug)
          Serial.println(F("Water refill and or dosing aborted"));
        beep();
        a_startRefilling = false;
        a_runRefillDosers = false;
        rtn = false;
        analogWriteChnl(pca9685Channel::doserOne, 0);
        analogWriteChnl(pca9685Channel::doserTwo, 0);
        analogWriteChnl(pca9685Channel::doserThree, 0);
        analogWriteChnl(pca9685Channel::doserFour, 0);
        analogWriteChnl(pca9685Channel::doserFive, 0);
        analogWriteChnl(pca9685Channel::doserSix, 0);
      }
    }
  }
  // Turn off the inlet pump
  if (!a_startRefilling && a_inletPumpIsOn) {
    analogWriteChnl(pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
    if (device::globalDebug)
      Serial.println(F("Turning off the inlet pump"));
    a_previousMillis = millis();
    a_inletPumpIsOn = false;
  }
  // run dosers
  if (a_runRefillDosers) {
    if (enabledDosers[0] && user::doserOneMode != device::DOSER_OFF)
      enabledDosers[0] = runDoser(1, pca9685Channel::doserOne, user::doserOneSpeed, user::refillDoserOneMills, a_previousMillis); // runDoser(1, pca9685Channel::doserOne, user::doserOneSpeed, a_dosingMls[0], previousDoserMillis);
    else if (enabledDosers[1] && user::doserTwoMode != device::DOSER_OFF)
      enabledDosers[1] = runDoser(2, pca9685Channel::doserTwo, user::doserTwoSpeed, user::refillDoserTwoMills, a_previousMillis);
    else if (enabledDosers[2] && user::doserThreeMode != device::DOSER_OFF)
      enabledDosers[2] = runDoser(3, pca9685Channel::doserThree, user::doserThreeSpeed, user::refillDoserThreeMills, a_previousMillis);
    else if (enabledDosers[3] && user::doserFourMode != device::DOSER_OFF)
      enabledDosers[3] = runDoser(4, pca9685Channel::doserFour, user::doserFourSpeed, user::refillDoserFourMills, a_previousMillis);
    else if (enabledDosers[4] && user::doserFiveMode != device::DOSER_OFF)
      enabledDosers[4] = runDoser(5, pca9685Channel::doserFive, user::doserFiveSpeed, user::refillDoserFiveMills, a_previousMillis);
    else if (enabledDosers[5] && user::doserSixMode != device::DOSER_OFF)
      enabledDosers[5] = runDoser(6, pca9685Channel::doserSix, user::doserSixSpeed, user::refillDoserSixMills, a_previousMillis);
    else {
      a_runRefillDosers = false;
      rtn = false;
      if (device::globalDebug)
        Serial.println(F("Doising complete"));
    }
  }
  return rtn;
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
        Serial.println(F("Water heater on"));
      }      
      analogWriteChnl(pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 0 : 4096));
      saveLogMessage(13); // save log message, water heater on
    }
    else if (waterTemp > minTemp && device::waterHeaterIsOn) {
      device::waterHeaterIsOn = false;
      if (device::globalDebug) {  
        Serial.println(F("Water heater off"));
      }   
      analogWriteChnl(pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 4096 : 0));
      saveLogMessage(14); // save log message, water heater off
    }
    prevWaterHeaterMin = rtc.minute();
  }
}

// Control the external lighting
void relayTimers() {

  if (rtc.minute() != device::prevLightMinute) {
    device::lightDuration++;
    if (device::globalDebug) { 
      Serial.print(F("Light ")); Serial.print(device::lightOn ? F("on") : F("off")); Serial.print(F(" duration (mins): ")); Serial.println(device::lightDuration);
    }
    device::prevLightMinute = rtc.minute();
  } 
        
  if (rtc.minute() != device::prevAuxRelayOneMinute) {
    device::auxRelayOneDuration++;
    if (device::globalDebug) {
      Serial.print(F("Aux relay 1 ")); Serial.print(device::auxRelayOneOn ? F("on") : F("off")); Serial.print(F(" duration (mins): ")); Serial.println(device::auxRelayOneDuration);
    }
    device::prevAuxRelayOneMinute = rtc.minute();
  } 

  if (rtc.minute() != device::prevAuxRelayTwoMinute) {
    device::auxRelayTwoDuration++;
    if (device::globalDebug) {
      Serial.print(F("Aux relay 2 ")); Serial.print(device::auxRelayTwoOn ? F("on") : F("off")); Serial.print(F(" duration (mins): ")); Serial.println(device::auxRelayTwoDuration);
    }
    device::prevAuxRelayTwoMinute = rtc.minute();
  }  

  if (user::lightState == device::AUTO_TIMER) { 
    if (user::lightMode == 0) {    
      if (device::lightOn && device::lightDuration >= user::lightOnDuration) {
        if (device::globalDebug)
          Serial.println(F("Light on"));
        analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
        device::lightDuration = 0;
        device::lightOn = true;
      }
      else if (!device::lightOn && device::lightDuration >= user::lightOffDuration) {
        if (device::globalDebug)
          Serial.println(F("Light off"));
        analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 4096 : 0));
        device::lightDuration = 0;
        device::lightOn = false;
      }
    }
    else {
      if (user::lightOnTimeHour == rtc.hour() && user::lightOnTimeMin == rtc.minute() && !device::lightOn) {
        analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
        if (device::globalDebug) {
          Serial.println(F("Light on"));
        }
        device::lightDuration = 0;
        device::lightOn = true;
        saveLogMessage(4);
      }
      else if (user::lightOffTimeHour == rtc.hour() && user::lightOffTimeMin == rtc.minute() && device::lightOn) {
        analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 4096 : 0));
        if (device::globalDebug) {
          Serial.println(F("Light off"));
        }
        device::lightDuration = 0;
        device::lightOn = false;
        saveLogMessage(5);
      }
    }
  }
  // ========================================================================================================
  if (user::auxRelayOneState == device::AUTO_TIMER) { 
    if (user::auxRelayOneMode == 0) {       
      if (device::auxRelayOneOn && device::auxRelayOneDuration >= user::auxRelayOneOnDuration) {
        if (device::globalDebug)
          Serial.println(F("Aux relay 1 on"));
        analogWriteChnl(pin::auxRelayOne, !device::relayOffState);
        device::auxRelayOneDuration = 0;
        device::auxRelayOneOn = false;
      }
      else if (!device::auxRelayOneOn && device::auxRelayOneDuration >= user::auxRelayOneOffDuration) {
        if (device::globalDebug)
          Serial.println(F("Aux relay 1 off"));
        digitalWrite(pin::auxRelayOne, device::relayOffState);
        device::auxRelayOneDuration = 0;
        device::auxRelayOneOn = true;
      }
    }
    else {
      if (user::auxRelayOneOnTimeHour == rtc.hour() && user::auxRelayOneOnTimeMin == rtc.minute() && !device::auxRelayOneOn) {
        digitalWrite(pin::auxRelayOne, !device::relayOffState);
        if (device::globalDebug) {
          Serial.println(F("Aux relay 1 on"));
        }
        device::auxRelayOneDuration = 0;
        device::auxRelayOneOn = true;
        saveLogMessage(4);
      }
      else if (user::auxRelayOneOffTimeHour == rtc.hour() && user::auxRelayOneOffTimeMin == rtc.minute() && device::auxRelayOneOn) {
        digitalWrite(pin::auxRelayOne, device::relayOffState);
        if (device::globalDebug) { 
          Serial.println(F("Aux relay 1 off"));
        }
        device::auxRelayOneDuration = 0;
        device::auxRelayOneOn = false;
        saveLogMessage(5);
      }
    }
  }
  // ========================================================================================================
  if (user::auxRelayTwoState == device::AUTO_TIMER) { 
    if (user::auxRelayTwoMode == 0) {     
      if (device::auxRelayTwoOn && device::auxRelayTwoDuration >= user::auxRelayTwoOnDuration) {
        if (device::globalDebug)
          Serial.println(F("Aux relay 2 on"));
        digitalWrite(pin::auxRelayTwo, !device::relayOffState);
        device::auxRelayTwoDuration = 0;
        device::auxRelayTwoOn = false;
      }
      else if (!device::auxRelayTwoOn && device::auxRelayTwoDuration >= user::auxRelayTwoOffDuration) {
        if (device::globalDebug)
          Serial.println(F("Aux relay 2 off"));
        digitalWrite(pin::auxRelayTwo, device::relayOffState);
        device::auxRelayTwoDuration = 0;
        device::auxRelayTwoOn = true;
      }
    }
    else {
      if (user::auxRelayTwoOnTimeHour == rtc.hour() && user::auxRelayTwoOnTimeMin == rtc.minute() && !device::auxRelayTwoOn) {
        digitalWrite(pin::auxRelayTwo, !device::relayOffState);
        if (device::globalDebug) {
          Serial.println(F("Aux relay 2 on"));
        }
        device::auxRelayTwoDuration = 0;
        device::auxRelayTwoOn = true;
        saveLogMessage(4);
      }
      else if (user::auxRelayTwoOffTimeHour == rtc.hour() && user::auxRelayTwoOffTimeMin == rtc.minute() && device::auxRelayTwoOn) {
        digitalWrite(pin::auxRelayTwo, device::relayOffState);
        if (device::globalDebug) {
          Serial.println(F("Aux relay 2 off"));
        }
        device::auxRelayTwoDuration = 0;
        device::auxRelayTwoOn = false;
        saveLogMessage(5);
      }
    }
  }
}

// Check if the current time is in between the users start and end time
bool restartTimer(const uint8_t a_onTimeMin, const uint8_t a_onTimeHour, const uint8_t a_offTimeMin, const uint8_t a_offTimeHour) {
  uint16_t startMinutesSinceMidnight = a_onTimeMin + (60 * a_onTimeHour);
  uint16_t endMinutesSinceMidnight = a_offTimeMin + (60 * a_offTimeHour);
  uint16_t currentMinutesSinceMidnight = rtc.minute() + (60 * rtc.hour());
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
  uint8_t continueCo2Control = device::NOT_SET;
  sensor::co2GasTime = 0;
  if (device::sensorsReady && !user::disableCo2Control) {
    // Check if it is time to start the Co2 adjustment
    if (user::co2CheckTimeHour == rtc.hour() && user::co2CheckTimeMinute >= rtc.minute() && rtc.day() != previousDate) {
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
          // turn on the Co2 solinode
          analogWriteChnl(pca9685Channel::co2Solenoid, (device::relayOffState == HIGH ? 0 : 4096));
          if (device::globalDebug)
            Serial.println(F("About to show Co2 abort dialog, and start pumping Co2"));
          // Show a dialog asking the user if they want to abort the Co2 adjustment while its pumping
          continueMessage(message::cancelCo2, sensor::co2GasTime / 60000, 0, false, true, false);
          display::lastTouchMillis = millis();
        }
      }
    }
    device::prevMillis = millis();
    while (startCo2Relay) {
      if (abortCo2Notification(display::lastTouchMillis, sensor::co2GasTime / 60000))
        break;
      if (millis() - device::prevMillis >= 1000UL) {
        if (sensor::co2GasTime >= 1000UL)
          sensor::co2GasTime -= 1000UL;
        else {
          sensor::co2GasTime = 0;
          break;
        }
        device::prevMillis = millis();
      }
      timedEvents();
    }
    analogWriteChnl(pca9685Channel::co2Solenoid, (device::relayOffState == HIGH ? 4096 : 0));
  }
}

// Control one or two 3rd party AC fans and a 3rd party AC air heater
// to maintain the air temperature and or humidity.
void airControl() {
  static uint32_t previousMillis = millis();
  static float previousFanOneSpeed = 200;
  static float previousFanTwoSpeed = 200;
  float tempPercent = 0, humPercent = 0;
  const bool fanDebug = device::globalDebug; // debug hidden for now while testing other functions
  //
  if (device::fanOneJustStarted || device::fanTwoJustStarted) {
    if (millis() - previousMillis >= 100UL) {
      if (device::fanOneJustStarted == 1) {
        if (fanDebug)   
          Serial.println(F("Spolling up fan one"));
        device::fanOneSpeed = user::targetMaxFanOneSpeed;
        device::fanOneJustStarted = 2;
      }
      else if (device::fanOneJustStarted == 2) {
        if (device::fanOneSpeed > user::targetMinFanOneSpeed)
          device::fanOneSpeed--;
        if (device::fanOneSpeed == user::targetMinFanOneSpeed)
          device::fanOneJustStarted = 0;
      }   
      //
      if (device::fanTwoJustStarted == 1) {
        if (fanDebug)   
          Serial.println(F("Spolling up fan two"));
        device::fanTwoSpeed = user::targetMaxFanTwoSpeed;
        device::fanTwoJustStarted = 2;
      }
      else if (device::fanTwoJustStarted == 2) {
        if (device::fanTwoSpeed > user::targetMinFanTwoSpeed)
          device::fanTwoSpeed--;
        if (device::fanTwoSpeed == user::targetMinFanTwoSpeed)
          device::fanTwoJustStarted = 0;
      } 
      previousMillis = millis();
    }
  }
  //
  else {
    if (device::sensorsReady && millis() - previousMillis >= 2000UL) {
      // If Co2 has disabled the fans for x durations, check to see if we can turn the fans back on else do nothing
      if (device::co2DisabledFans && device::co2TurnFansBackOnHour == rtc.hour() && device::co2TurnFansBackOnMinute >= rtc.minute())
        device::co2DisabledFans = false;
      else if (device::co2DisabledFans)
        return;

      // Check to see if the temperature and or humidity is out of range of the user targets
      device::controlOptions tempOption = device::SPEED_IDLE;
      device::controlOptions humOption = device::SPEED_IDLE;
      device::controlOptions fanMode = device::SPEED_IDLE;
      float maxAirTemp = 0;
      float minAirTemp = 0;
      float maxAirHum = 0;
      float minAirHum = 0;
      float airTemp = user::convertToF ? convertToF(sensor::airTemp) : sensor::airTemp;
      if (device::lightOn) {
        maxAirTemp = user::convertToF ? user::targetDayMaxAirTempF : user::targetDayMaxAirTemp;
        minAirTemp = user::convertToF ? user::targetDayMinAirTempF : user::targetDayMinAirTemp;
        maxAirHum = user::targetDayMinHumidity;
        minAirHum = user::targetDayMaxHumidity;
      }
      else {
        maxAirTemp = user::convertToF ? user::targetNightMaxAirTempF : user::targetNightMaxAirTemp;
        minAirTemp = user::convertToF ? user::targetNightMinAirTempF : user::targetNightMinAirTemp;
        maxAirHum = user::targetNightMinHumidity;
        minAirHum = user::targetNightMaxHumidity;
      }

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
      if (sensor::humidity > maxAirHum && user::fansControlHumidity)  {
        float humOutOfRange = abs(maxAirHum - sensor::humidity);
        humPercent = humOutOfRange / (maxAirHum / 100.0);
        //Serial.println(F("SPEED_UP humPercent: %d", humPercent);
      }
      else if (sensor::humidity < minAirHum && user::fansControlHumidity)  {
        float humOutOfRange = abs(minAirHum - sensor::humidity);
        humPercent = humOutOfRange / (minAirHum / 100.0);
        //Serial.println(F("SPEED_DOWN humPercent: %d", humPercent);
      }

      // Control the air heater
      static uint8_t prevAirHeaterMin = 69;
      if (rtc.minute() != prevAirHeaterMin) {
        if (airTemp <= minAirTemp && !device::airHeaterIsOn) {
          device::airHeaterIsOn = true;
          if (fanDebug)   
            Serial.println(F("air heater on"));
          analogWriteChnl(pca9685Channel::airHeater, (device::relayOffState == HIGH ? 0 : 4096));
          saveLogMessage(11); // save log message, air heater on
        }
        else if (airTemp > minAirTemp &&device::airHeaterIsOn) {
          device::airHeaterIsOn = false;
          if (fanDebug)   
            Serial.println(F("air heater off"));
          analogWriteChnl(pca9685Channel::airHeater, (device::relayOffState == HIGH ? 4096 : 0));
          saveLogMessage(12); // save log message, air heater off
        }
        prevAirHeaterMin = rtc.minute();
      }

      float averageTemp = averageFltArray(sensor::fanTemperatureArray, sensor::fanArrayPos);
      sensor::airStates tempAirState = sensor::IS_SAME;
      if (abs(airTemp - averageTemp) >=sensor:: airHysteresis) {
        //Serial.print(F("Has changed, tempAirState: "));
        if (airTemp < averageTemp)
          tempAirState = sensor::IS_FALLING;
        else if (airTemp > averageTemp)
          tempAirState = sensor::IS_RISING;
        //Serial.println(tempAirState);
      }
      float averageHum = averageFltArray(sensor::fanHumidityArray, sensor::fanArrayPos);
      sensor::airStates humAirState = sensor::IS_SAME;
      if (abs(sensor::humidity - averageHum) >= sensor::airHysteresis)  {
        //Serial.print(F("Has changed, humAirState: "));
        if (sensor::humidity < averageHum)
          humAirState = sensor::IS_FALLING;
        else if (sensor::humidity > averageHum)
          humAirState = sensor::IS_RISING;
        //Serial.println(humAirState);
      }

      if (user::fansControlTemperature) {
        if (fanDebug) {
          Serial.print(F("Air temp: ")); Serial.print(airTemp, 2); Serial.print(F("c, average temp: ")); Serial.print(averageTemp, 2); Serial.print(F("c, over ")); Serial.print(sensor::fanArrayPos); Serial.println(F(" reads @ 2 sec per read"));
        }     
        adjustFanMode(airTemp, tempOption, tempAirState, "air temp", tempPercent, minAirTemp, maxAirTemp, fanDebug);
      }
      if (user::fansControlHumidity) {
        if (fanDebug) {
          Serial.print(F("Humidity: ")); Serial.print(sensor::humidity, 2); Serial.print(F("%, averageHum: ")); Serial.print(averageHum, 2); Serial.println(F("%, over ")); Serial.print(sensor::fanArrayPos); Serial.println(F(" reads @ 2 sec per read"));
        }   
        adjustFanMode(sensor::humidity, humOption, humAirState, "humidity", humPercent, minAirHum, maxAirHum, fanDebug);
      }

      // Temperature has priority over humidity
      if (tempOption != device::SPEED_IDLE || !user::fansControlHumidity) {
        fanMode = tempOption;
        if (fanDebug) {
          Serial.print(F("Fans set to control temperature")); Serial.print(user::fansControlHumidity ? F(", as temperature takes priority\n") : F("\n"));
        }     
      }
      else {
        fanMode = humOption;
        if (fanDebug) {
          Serial.print(F("Fans set to control humidity")); Serial.print(user::fansControlTemperature ? F(", as the temperature is in range\n") : F("\n"));
        }        
      }
      
      // adjust the fan speeds
      if (user::fanOneEnabled && !user::fanOneFixedSpeed) {
        if (fanMode == device::SPEED_MAX)
          device::fanOneSpeed = user::targetMaxFanOneSpeed;
        else if (fanMode == device::SPEED_MIN)
          device::fanOneSpeed = user::targetMinFanOneSpeed;
        else if (fanMode == device::SPEED_UP && device::fanOneSpeed < user::targetMaxFanOneSpeed)
          device::fanOneSpeed++;
        else if (fanMode == device::SPEED_UP_SLOW && device::fanOneSpeed < user::targetMaxFanOneSpeed)
          device::fanOneSpeed += 0.25;
        else if (fanMode == device::SPEED_DOWN && device::fanOneSpeed > user::targetMinFanOneSpeed)
          device::fanOneSpeed--;
        else if (fanMode == device::SPEED_DOWN_SLOW && device::fanOneSpeed > user::targetMinFanOneSpeed)
          device::fanOneSpeed -= 0.25;
        if (device::fanOneSpeed < user::targetMinFanOneSpeed) 
          device::fanOneSpeed = user::targetMinFanOneSpeed;   
        else if (device::fanOneSpeed > user::targetMaxFanOneSpeed) 
          device::fanOneSpeed = user::targetMaxFanOneSpeed;  
      }
      if (user::fanTwoEnabled && !user::fanTwoFixedSpeed) {
        if (fanMode == device::SPEED_MAX)
          device::fanTwoSpeed = user::targetMaxFanTwoSpeed;
        else if (fanMode == device::SPEED_MIN)
          device::fanTwoSpeed = user::targetMinFanTwoSpeed;
        else if (fanMode == device::SPEED_UP && device::fanTwoSpeed < user::targetMaxFanTwoSpeed)
          device::fanTwoSpeed++;
        else if (fanMode == device::SPEED_UP_SLOW && device::fanTwoSpeed < user::targetMaxFanTwoSpeed)
          device::fanTwoSpeed += 0.25;
        else if (fanMode == device::SPEED_DOWN && device::fanTwoSpeed > user::targetMinFanTwoSpeed)
          device::fanTwoSpeed--;
        else if (fanMode == device::SPEED_DOWN_SLOW && device::fanTwoSpeed > user::targetMinFanTwoSpeed)
          device::fanTwoSpeed -= 0.25;
        if (device::fanTwoSpeed < user::targetMinFanTwoSpeed) 
          device::fanTwoSpeed = user::targetMinFanTwoSpeed;  
        else if (device::fanTwoSpeed > user::targetMaxFanTwoSpeed) 
          device::fanTwoSpeed = user::targetMaxFanTwoSpeed;
      }
      previousMillis = millis();
    }
  }
  // Send the new fan speeds to the Atmel328P
  if (device::fanOneSpeed != previousFanOneSpeed || device::fanTwoSpeed != previousFanTwoSpeed) {
    if (fanDebug) {
      Serial.print(F("Fan one speed: ")); Serial.println(device::fanOneSpeed);
      Serial.print(F("Fan two speed: ")); Serial.println(device::fanTwoSpeed);
    }
    setFanSpeeds(device::fanOneSpeed, device::fanTwoSpeed);
    previousFanOneSpeed = device::fanOneSpeed;
    previousFanTwoSpeed = device::fanTwoSpeed;
  }
}

// Control the waters EC and or PH and or PMM
void waterEcPhControl() {
  enum adjustmentModes {UNADJUSTED, ADJUSTED_EC, ADJUSTED_PH};
  static adjustmentModes adjustmentMode = UNADJUSTED;
  //static adjustmentModes lastAdjustmentMode = UNADJUSTED;
  // Every hour increment the dosing hour counter
  if (device::previousDosingHour != rtc.hour() && rtc.minute() >= device::previousDosingMinute) {
    device::dosingTimerHourCounter++;
    device::previousDosingHour = rtc.hour();
    device::previousDosingMinute = rtc.minute();
    if (device::globalDebug) {
      Serial.print(F("Dosing Timer Hour Counter: ")); Serial.println(device::dosingTimerHourCounter);
    }
  }
  // If the dosing hour counter is greather than or equals the dosingInterval then run the dosing logic
  if (device::dosingTimerHourCounter >= user::dosingInterval) {
    if (!device::currentlyDosing && device::sensorsReady) {
      if (adjustmentMode == UNADJUSTED || adjustmentMode == ADJUSTED_PH) {
        if (user::convertToTds)
          adjustWaterTds();
        else
          adjustWaterEc();
        adjustmentMode = ADJUSTED_EC;
      }
      else {
        adjustWaterPh();
        adjustmentMode = ADJUSTED_PH;
      }
      clearPage();
      display::refreshPage = true;
    }
  }
}

void adjustWaterEc() {
  if (device::globalDebug)
    Serial.println(F("Attempting to adjust the EC..."));
  if (sensor::ec < user::targetMinEc) {
    float percentage = percentOutOfRange(user::targetMinEc, sensor::ec);
    if (device::globalDebug) {
      Serial.print(F("EC ")); Serial.print(sensor::ec); Serial.print(F(" is lower than min target ")); Serial.print(user::targetMinEc); Serial.print(F(" by ")); Serial.print(percentage, 2); Serial.println(F("%"));
    }
    // Display a notification to cancel or continue with the starting of the dosing
    display::lastTouchMillis = millis();
    launchDosingNotification(percentage, 1, display::lastTouchMillis);
    // Work out dosing amount
    float dosingAmount = 0;
    if (device::continueDosing != device::CANCEL) {
      float dosingMls[6] {0, 0, 0, 0, 0, 0};
      bool enabledDosers[6] {false, false, false, false, false, false};
      const uint8_t currentDoserModes[] {user::doserOneMode, user::doserTwoMode, user::doserThreeMode, user::doserFourMode, user::doserFiveMode, user::doserSixMode};
      const uint16_t currentDoserMls[] {user::doserOneMills, user::doserTwoMills, user::doserThreeMills, user::doserFourMills, user::doserFiveMills, user::doserSixMills};
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
      //
      if (device::globalDebug)
        Serial.print(F("Enabled dosers = "));
      int16_t numEnabledDosers = 0;
      for (uint8_t i = 0; i < 6; i++) {
        if (enabledDosers[i]) {
          if (device::globalDebug) {
            Serial.print(i+1); Serial.print(F(" "));
          }
          numEnabledDosers++;
        }
      }
      if (device::globalDebug)
        Serial.println();
      //    
      saveLogMessage(9);
      //
      if (user::ecDosingMode == user::PRECISE) {
        if (device::globalDebug) {
          Serial.println(F("EC/TDS dosing mode set to precise"));
          Serial.print(F("waterVolumeLtrs = ")); Serial.println(sensor::waterVolumeLtrs, 2);
        }
        // Work out the exact amout of nutrients to dose
        dosingAmount = (user::targetMinEc - sensor::ec) * sensor::waterVolumeLtrs / sensor::ecSolution;
        float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
        for (uint16_t i = 0; i < 6; i++) {
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
        for (int16_t i = 0; i < 6; i++) {
          if (enabledDosers[i]) {
            dosingAmount += dosingMls[i] = currentDoserMls[i];
          }
        }
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        if (device::globalDebug)
          Serial.println(F("Starting dosing..."));
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 1, display::lastTouchMillis);
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
      Serial.print(F("TDS ")); Serial.print(sensor::tds); Serial.print(F(" is lower than min target ")); Serial.print(user::targetMinTds); Serial.print(F(" by ")); Serial.print(percentage, 2); Serial.println(F("%"));
    }
    // Display a notification to cancel or continue with the starting of the dosing
    display::lastTouchMillis = millis();
    launchDosingNotification(percentage, 1, display::lastTouchMillis);
    // Work out dosing amount
    float dosingAmount = 0;
    if (device::continueDosing != device::CANCEL) {
      float dosingMls[6] {0, 0, 0, 0, 0, 0};
      bool enabledDosers[6] {false, false, false, false, false, false};
      const uint8_t currentDoserModes[] {user::doserOneMode, user::doserTwoMode, user::doserThreeMode, user::doserFourMode, user::doserFiveMode, user::doserSixMode};
      const uint16_t currentDoserMls[] {user::doserOneMills, user::doserTwoMills, user::doserThreeMills, user::doserFourMills, user::doserFiveMills, user::doserSixMills};
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
      //
      if (device::globalDebug) 
        Serial.print(F("Enabled dosers = "));
      int16_t numEnabledDosers = 0;
      for (uint8_t i = 0; i < 6; i++) {
        if (enabledDosers[i]) {
          if (device::globalDebug) {
            Serial.print(i+1); Serial.print(F(" "));
          }
          numEnabledDosers++;
        }
      }
      if (device::globalDebug)
        Serial.println();
      //    
      saveLogMessage(9);
      //
      if (user::ecDosingMode == user::PRECISE) {
        if (device::globalDebug) {
          Serial.println(F("EC/TDS dosing mode set to precise"));
          Serial.print(F("waterVolumeLtrs = ")); Serial.println(sensor::waterVolumeLtrs, 2);
        }
        // Work out the exact amout of nutrients to dose
        dosingAmount = (user::targetMinTds - sensor::tds) * sensor::waterVolumeLtrs / sensor::tdsSolution;
        float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
        for (uint8_t i = 0; i < 6; i++) {
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
        for (uint8_t i = 0; i < 6; i++) {
          if (enabledDosers[i]) {
            dosingAmount += dosingMls[i] = currentDoserMls[i];
          }
        }
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        if (device::globalDebug)
          Serial.println(F("Starting dosing..."));
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 1, display::lastTouchMillis);
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
      Serial.print(F("PH ")); Serial.print(sensor::ph); 
      if (sensor::ph < user::targetMinPh) {
        Serial.print(F(" is lower than min target ")); Serial.print(user::targetMinPh, 2);
      }
      else {
        Serial.print(F(" is higher than max target ")); Serial.print(user::targetMaxPh, 2);
      }      
      Serial.print(F(" by ")); Serial.print(percentage, 2); Serial.println(F("%"));
    }
    // Display a notification to cancel or continue with the starting of the dosing
    display::lastTouchMillis = millis();
    launchDosingNotification(percentage, 0, display::lastTouchMillis);
    // Work out dosing amount
    float dosingAmount = 0;
    if (device::continueDosing != device::CANCEL) {
      bool enabledDosers[6] {false, false, false, false, false, false};
      float dosingMls[6] {0, 0, 0, 0, 0, 0};
      // Adjust PH up
      if (sensor::ph < user::targetMinPh) {
        if (user::doserOneMode == device::DOSER_PH_UP) 
          enabledDosers[0] = true;
        if (user::doserTwoMode == device::DOSER_PH_UP) 
          enabledDosers[1] = true;
        if (user::doserThreeMode == device::DOSER_PH_UP) 
          enabledDosers[2] = true;
        if (user::doserFourMode == device::DOSER_PH_UP) 
          enabledDosers[3] = true;
        if (user::numberOfDosers >= 5 && user::doserFiveMode == device::DOSER_PH_UP) 
          enabledDosers[4] = true;
        if (user::numberOfDosers >= 6 && user::doserSixMode == device::DOSER_PH_UP) 
          enabledDosers[5] = true;
      }
      // Adjust PH down
      else if (sensor::ph > user::targetMinPh) {
        if (user::doserOneMode == device::DOSER_PH_DOWN) 
          enabledDosers[0] = true;
        if (user::doserTwoMode == device::DOSER_PH_DOWN) 
          enabledDosers[1] = true;
        if (user::doserThreeMode == device::DOSER_PH_DOWN) 
          enabledDosers[2] = true;
        if (user::doserFourMode == device::DOSER_PH_DOWN) 
          enabledDosers[3] = true;
        if (user::numberOfDosers >= 5 && user::doserFiveMode == device::DOSER_PH_DOWN) 
          enabledDosers[4] = true;
        if (user::numberOfDosers >= 6 && user::doserSixMode == device::DOSER_PH_DOWN) 
          enabledDosers[5] = true;
      }   
      if (device::globalDebug)
         Serial.print(F("Enabled dosers = "));
      int16_t numEnabledDosers = 0;
      for (uint8_t i = 0; i < 6; i++) {
        if (enabledDosers[i]) {
          if (device::globalDebug) {
            Serial.print(i+1); Serial.print(F(" "));
          }
          numEnabledDosers++;
        }
      }
      if (device::globalDebug)
        Serial.println();
      sensor::ph < user::targetMinPh ? saveLogMessage(8) : saveLogMessage(7);
      if (user::phDosingMode == user::PRECISE) {
        if (device::globalDebug) {
          Serial.println(F("PH dosing mode set to precise"));
          Serial.print(F("waterVolumeLtrs = ")); Serial.println(sensor::waterVolumeLtrs, 2);
        }
        if (sensor::ph < user::targetMinPh) {
          // Work out the exact amout of PH up to dose
          dosingAmount = (user::targetMinPh - sensor::ph) * sensor::waterVolumeLtrs / sensor::phDownSolution;
          float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
          for (uint8_t i = 0; i < 6; i++) {
            if (enabledDosers[i])
              dosingMls[i] = mlsPerDoser;
          }
          if (device::globalDebug) {
            Serial.print(F("Calculated ")); Serial.print(dosingAmount, 2); Serial.print(F(" mls of PH up solution, divided by ")); Serial.print(numEnabledDosers); Serial.print(F(" PH up dosers = ")); Serial.println(mlsPerDoser, 2);
          }
        }
        else if (sensor::ph > user::targetMaxPh) {
          // Work out the exact amout of PH down to dose
          dosingAmount = (sensor::ph - user::targetMaxPh) * sensor::waterVolumeLtrs / sensor::phUpSolution;
          float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
          for (uint8_t i = 0; i < 6; i++) {
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
          dosingAmount += dosingMls[0] = user::doserOneMills;
        if (enabledDosers[1]) 
          dosingAmount += dosingMls[1] = user::doserTwoMills;
        if (enabledDosers[2]) 
          dosingAmount += dosingMls[2] = user::doserThreeMills;
        if (enabledDosers[3]) 
          dosingAmount += dosingMls[3] = user::doserFourMills;
        if (enabledDosers[4]) 
          dosingAmount += dosingMls[4] = user::doserFiveMills;
        if (enabledDosers[5]) 
          dosingAmount += dosingMls[5] = user::doserSixMills; 
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        if (device::globalDebug)
          Serial.println(F("Starting dosing..."));
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 0, display::lastTouchMillis);
      }        
    }
    device::dosingTimerHourCounter = 0;
  }
}

// Return the percent out of range
float percentOutOfRange(const float& a_setPoint, const float& a_val) {
  float outOfRange = abs(a_setPoint - a_val);
  float percent = a_setPoint / 100.0;
  return outOfRange / percent;
}

void runDosers(bool* a_enabledDosers, float* a_dosingMls, const float a_percent, const int a_dosingMode, uint32_t a_lastTouch) {
  uint8_t previousDoserNum = 0;
  device::currentDoserNum = 1;
  device::currentDoserMls = 0;
  bool updateMlsText = false;
  static float prevCurrentDoserMls = -1;
  const char* str[3] = {"PH", "EC", "TDS"};
  uint32_t previousDoserMillis = millis();
  while (device::currentlyDosing) { // timing is critical we must use a while loop
    // show continue dialog
    if (device::currentDoserNum != previousDoserNum) {
      abortMessage(message::cancelDosing, str[a_dosingMode], a_percent, device::currentDoserNum, device::currentDoserMls, 2, updateMlsText);
      previousDoserNum = device::currentDoserNum;
      prevCurrentDoserMls = device::currentDoserMls;
    }
    if (a_enabledDosers[0])
      a_enabledDosers[0] = runDoser(1, pca9685Channel::doserOne, user::doserOneSpeed, a_dosingMls[0], previousDoserMillis);
    else if (a_enabledDosers[1])
      a_enabledDosers[1] = runDoser(2, pca9685Channel::doserTwo, user::doserTwoSpeed, a_dosingMls[1], previousDoserMillis);
    else if (a_enabledDosers[2])
      a_enabledDosers[2] = runDoser(3, pca9685Channel::doserThree, user::doserThreeSpeed, a_dosingMls[2], previousDoserMillis);
    else if (a_enabledDosers[3])
      a_enabledDosers[3] = runDoser(4, pca9685Channel::doserFour, user::doserFourSpeed, a_dosingMls[3], previousDoserMillis);
    else if (a_enabledDosers[4])
      a_enabledDosers[4] = runDoser(5, pca9685Channel::doserFive, user::doserFiveSpeed, a_dosingMls[4], previousDoserMillis);
    else if (a_enabledDosers[5])
      a_enabledDosers[5] = runDoser(6, pca9685Channel::doserSix, user::doserSixSpeed, a_dosingMls[5], previousDoserMillis);
    else if (device::currentlyDosing) {
      if (device::globalDebug)
        Serial.println(F("Dosing complete"));
      device::currentlyDosing = false;
      clearPage();
      display::refreshPage = true;
    }
    // touch event - cancel button
    runDosersTouch(a_enabledDosers, a_lastTouch);
    static uint32_t previousMillis = millis();
    if (millis() - previousMillis >= 1000UL) {
      rtc.refresh();
      relayTimers();
      previousMillis = millis();
    }  
  }
}

// Run a given doser for a_mls * 1000 on a_doserPin at a_doserSpeed
bool runDoser(const uint8_t& a_doserNum, const uint8_t& a_doserChnl, const int& a_doserSpeed, const float& a_mls, uint32_t& a_previousDoserMillis) {
  static uint32_t debugPreviousMillis = 0;
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

void acControl() {
  if (millis() - device::irPreviousMillis >= (user::acPollTime * 1000UL)) {
    if (device::globalDebug) {
      Serial.print(F("----------------------------------------------")); Serial.print(__FUNCTION__); Serial.println(F("----------------------------------------------"));
    }
    float tempPercent = 0, humPercent = 0;
    float maxAirTemp = 0;
    float minAirTemp = 0;
    float maxAirHum = 0;
    float minAirHum = 0;
    float airTemp = user::convertToF ? convertToF(sensor::airTemp) : sensor::airTemp;
    if (device::lightOn) {
      maxAirTemp = user::convertToF ? user::targetDayMaxAirTempF : user::targetDayMaxAirTemp;
      minAirTemp = user::convertToF ? user::targetDayMinAirTempF : user::targetDayMinAirTemp;
      maxAirHum = user::targetDayMinHumidity;
      minAirHum = user::targetDayMaxHumidity;
    }
    else {
      maxAirTemp = user::convertToF ? user::targetNightMaxAirTempF : user::targetNightMaxAirTemp;
      minAirTemp = user::convertToF ? user::targetNightMinAirTempF : user::targetNightMinAirTemp;
      maxAirHum = user::targetNightMinHumidity;
      minAirHum = user::targetNightMaxHumidity;
    }
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
    if (sensor::humidity > maxAirHum)  {
      float humOutOfRange = abs(maxAirHum - sensor::humidity);
      humPercent = humOutOfRange / (maxAirHum / 100.0);
      //Serial.println(F("SPEED_UP humPercent: %d", humPercent);
    }
    else if (sensor::humidity < minAirHum)  {
      float humOutOfRange = abs(minAirHum - sensor::humidity);
      humPercent = humOutOfRange / (minAirHum / 100.0);
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
