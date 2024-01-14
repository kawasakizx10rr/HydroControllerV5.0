void envriomentalControl() {
  lightingControl();
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
    rtc.refresh();
    if (!user::disableDrainAndRefill && rtc.day() != previousDate) {
      for (uint8_t i = 0; i < 31; i++) {
        if (user::autoFillDays[i] && rtc.day() == i + 1 && rtc.hour() == user::autoFillHour && rtc.minute() >= user::autoFillMinute) {
          printf("Auto drain time matched, about to start the auto drain process\n");
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
      printf("About to show the drain abort dialog, and start draining\n");
      const float waterTarget = user::convertToInches ? user::targetMinWaterHeight / 2.5 : user::targetMinWaterHeight;
      abortMessage(message::cancelDraining, user::convertToInches ? "\"" : "cm", waterTarget, -1, 1);
    }

    // Start draining the water from the tank
    int previousWaterLevel = sensor::waterLevel;
    previousMillis = millis();
    while (startDraining && continueDraining != device::CANCEL) {
      if (sensor::waterLevel > 0) { // drain tank till empty
        // Turn drain pump relay on
        analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 0 : 4096));
        // timer checking water level is still decresing else bail after 1 minute * drainTimeout
        if (millis() - previousMillis >= 60000UL * user::drainTimeout) { // put timer value in settings GUI !
          analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
          startDraining = false;
          startRefilling = true;
          printf("Failed to pump any further water out of the tank, quiting drain process\n");
        }
        int waterHeight = getWaterHeight();
        if (waterHeight >= 0)
          sensor::waterLevel = waterHeight;

        if (sensor::waterLevel < previousWaterLevel) {
          previousWaterLevel = sensor::waterLevel;
          previousMillis = millis();
        }
      }
      else {
        printf("Draining complete\n");
        analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
        startDraining = false;
        startRefilling = true;
      }

      // cancel button touch event for abort draining dialog
      const uint16_t startX = 166, startY = 166;
      if (tft.touched()) {
        tft.touchReadPixel(&display::touch_x, &display::touch_y);
        if (millis() >= lastTouch) {
          if (display::touch_x >= startX + 200 && display::touch_x <= startX + 400 && display::touch_y >= startY + 200 && display::touch_y <= startY + 250) { // Cancel
            printf("Water drain aborted\n");
            analogWriteChnl(pca9685Channel::outletPump, (device::relayOffState == HIGH ? 4096 : 0));
            beep();
            startDraining = false;
            clearPage();
            display::refreshPage = true;
          }
        }
      }
    }

    // refill tank when water is low at any date or time
    if (!user::disableDrainAndRefill && !startRefilling) {
      static unsigned long previousDelayMillis = millis(); // give the user 5 minutes to disable "refill When Low" else if the water level is below min this will keep showing a dialog over and over..
      if (millis() - previousDelayMillis >= 300000UL) {
        if ((sensor::waterLevel < user::targetMinWaterHeight && !user::convertToInches) || (sensor::waterLevelInches < user::targetMinWaterHeightInches && user::convertToInches)) {
          startRefilling = true;
          printf("The water level is below the min target, starting refill process\n");
        }
        previousDelayMillis = millis();
      }
    }

    // Show a dialog asking the user if they want to cancel the refill process before it starts
    launchRefillNotification(startRefilling, continueRefilling);

    // Show a dialog asking the user if they want to abort the refill process while its refilling
    if (startRefilling && continueRefilling != device::CANCEL) {
      printf("About to show refill abort dialog, and refill tank\n");
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

  if (waterLevel < waterTarget && startRefilling) {
    // turn on the inlet water pump
    analogWriteChnl(pca9685Channel::inletPump, (device::relayOffState == HIGH ? 0 : 4096));
    // timer checking water level is still decresing else bail after 1 minute * drainTimeout
    if (millis() - a_previousMillis >= 60000UL * user::drainTimeout) {
      printf("Failed to pump in any further water, quiting refill process and starting dosing\n");
      analogWriteChnl(pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
      startRefilling = false;
    }
    else if (waterLevel >= waterTarget) {
      printf("Finished pumping in water, quiting refill process and starting dosing\n");
      analogWriteChnl(pca9685Channel::inletPump, (device::relayOffState == HIGH ? 4096 : 0));
      startRefilling = false;
    }
  }

  // get current water height
  sensor::waterLevel = getWaterHeight();
  if (sensor::waterLevel < a_previousWaterLevel) {
    a_previousWaterLevel = sensor::waterLevel;
    a_previousMillis = millis();
  }
  else if (!startRefilling && !lockPump) {
    printf("Refilling complete, starting dosing\n");
    for (uint8_t i = 0; i < user::numberOfDosers; i++)
      enabledDosers[i] = true;
    lockPump = true;
    previousDoserMillis = millis();
  }

  // run dosers
  if (lockPump) {
    if (enabledDosers[0] && user::doserOneMode != device::DOSER_OFF)
      enabledDosers[0] = runDoser(1, pca9685Channel::doserOne, user::doserOneSpeed, user::refillDoserOneMills, previousDoserMillis);
    else if (enabledDosers[1] && user::doserTwoMode != device::DOSER_OFF)
      enabledDosers[1] = runDoser(2, pca9685Channel::doserTwo, user::doserTwoSpeed, user::refillDoserTwoMills, previousDoserMillis);
    else if (enabledDosers[2] && user::doserThreeMode != device::DOSER_OFF)
      enabledDosers[2] = runDoser(3, pca9685Channel::doserThree, user::doserThreeSpeed, user::refillDoserThreeMills, previousDoserMillis);
    else if (enabledDosers[3] && user::doserFourMode != device::DOSER_OFF)
      enabledDosers[3] = runDoser(4, pca9685Channel::doserFour, user::doserFourSpeed, user::refillDoserFourMills, previousDoserMillis);
    else if (enabledDosers[4] && user::doserFiveMode != device::DOSER_OFF)
      enabledDosers[4] = runDoser(5, pca9685Channel::doserFive, user::doserFiveSpeed, user::refillDoserFiveMills, previousDoserMillis);
    else if (enabledDosers[5] && user::doserSixMode != device::DOSER_OFF)
      enabledDosers[5] = runDoser(6, pca9685Channel::doserSix, user::doserSixSpeed, user::refillDoserSixMills, previousDoserMillis);
    else {
      printf("Dosing is complete\n");
      lockPump = false;
      device::dosingTimerHourCounter = 0;
      return false;
    }
  }

  // touch event - cancel button
  const uint16_t startX = 166, startY = 166;
  if (tft.touched()) {
    tft.touchReadPixel(&display::touch_x, &display::touch_y);
    if (millis() > a_lastTouch) {
      if (display::touch_x >= startX + 200 && display::touch_x <= startX + 400 && display::touch_y >= startY + 200 && display::touch_y <= startY + 250) { // Cancel
        // digitalWrite(pin::inletPump, device::relayOffState);
        // printf("Water refill and or dosing aborted\n");
        analogWriteChnl(pca9685Channel::doserOne, 0);
        analogWriteChnl(pca9685Channel::doserTwo, 0);
        analogWriteChnl(pca9685Channel::doserThree, 0);
        analogWriteChnl(pca9685Channel::doserFour, 0);
        analogWriteChnl(pca9685Channel::doserFive, 0);
        analogWriteChnl(pca9685Channel::doserSix, 0);
        beep();
        return false;
      }
    }
  }

  return true;
}

// Control the water temperature
void waterTemperatureControl() {
  // Control the water heater
  float waterTemp = sensor::waterTemp;
  float minTemp = user::targetMinWaterTemp;
  static uint8_t prevWaterHeaterMin = 69;
  rtc.refresh();
  if (rtc.minute() != prevWaterHeaterMin) {
    if (user::convertToF) {
      waterTemp = convertToF(sensor::waterTemp);
      minTemp = user::targetMinWaterTempF;
    } 
    if (waterTemp <= minTemp && !device::waterHeaterIsOn) {
      device::waterHeaterIsOn = true;
      analogWriteChnl(pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 0 : 4096));
      saveLogMessage(13); // save log message, water heater on
    }
    else if (waterTemp > minTemp && device::waterHeaterIsOn) {
      device::waterHeaterIsOn = false;
      analogWriteChnl(pca9685Channel::waterHeater, (device::relayOffState == HIGH ? 4096 : 0));
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
    rtc.refresh();
    if (lightOnTimeHour == rtc.hour() && lightOnTimeMin == rtc.minute() && !device::lightOn) {
      analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
      printf("Light on\n");
      device::lightOn = true;
      device::lightSwitchedOnHour = rtc.hour();
      device::lightSwitchedOnMin = rtc.minute();
      saveLogMessage(4);
    }
    else if (lightOffTimeHour == rtc.hour() && lightOffTimeMin == rtc.minute() && device::lightOn) {
      analogWriteChnl(pca9685Channel::light, 0);
      printf("Light off\n");
      device::lightOn = false;
      saveLogMessage(5);
    }
  }
  else if (lightMode == 1 && lightMode != prviousLightMode) { // constant on
    analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 0 : 4096));
    printf("Light const on\n");
    device::lightOn = true;
    device::lightSwitchedOnHour = rtc.hour();
    device::lightSwitchedOnMin = rtc.minute();
  }
  else if (lightMode == 2 && lightMode != prviousLightMode) { // constant off
    printf("Light const off\n");
    analogWriteChnl(pca9685Channel::light, (device::relayOffState == HIGH ? 4096 : 0));
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

// Check if the current time is in between the users start and end time
bool restartLightingTimer() {
  using namespace user;
  int startMinutesSinceMidnight = lightOnTimeMin + 60 * lightOnTimeHour;
  int endMinutesSinceMidnight = lightOnTimeHour + 60 * lightOffTimeHour;
  rtc.refresh();
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
    // Get current date time
    rtc.refresh();
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
          // turn on the Co2 solnoide
          analogWriteChnl(pca9685Channel::co2Solenoid, (device::relayOffState == HIGH ? 0 : 4096));
          printf("About to show Co2 abort dialog, and start pumping Co2\n");
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
  static uint8_t previousFanOneSpeed = 200;
  static uint8_t previousFanTwoSpeed = 200;
  float tempPercent = 0, humPercent = 0;
  float tempOutOfRange = 0, humOutOfRange = 0;

  if (millis() - previousMillis >= 2000UL) {
    rtc.refresh();
    // If Co2 has disabled the fans for x durations, check to see if we can turn the fans back on else do nothing
    if (device::co2DisabledFans && device::co2TurnFansBackOnHour == rtc.hour() && device::co2TurnFansBackOnMinute >= rtc.minute())
      device::co2DisabledFans = false;
    else if (device::co2DisabledFans)
      return;

    // Check to see if the fan speed is less than or greater than the min or max speed and if so adjust the current fans speed
    if (device::fanOneSpeed < user::targetMinFanOneSpeed) {
      device::fanOneSpeed = user::targetMinFanOneSpeed;
    }
    else if (device::fanOneSpeed > user::targetMaxFanOneSpeed) {
      device::fanOneSpeed = user::targetMaxFanOneSpeed;
    }
    if (device::fanTwoSpeed < user::targetMinFanTwoSpeed) {
      device::fanTwoSpeed = user::targetMinFanTwoSpeed;
    }
    else if (device::fanTwoSpeed > user::targetMaxFanTwoSpeed) {
      device::fanTwoSpeed = user::targetMaxFanTwoSpeed;
    }

    // Check to see if the temperature and or humidity is out of range of the user targets
    device::controlOptions tempOption = device::SPEED_IDLE;
    device::controlOptions humOption = device::SPEED_IDLE;
    device::controlOptions fanMode = device::SPEED_IDLE;
    const float maxAirTemp = user::convertToF ? user::targetMaxAirTempF : user::targetMaxAirTemp;
    const float minAirTemp = user::convertToF ? user::targetMinAirTempF : user::targetMinAirTemp;
    const float airTemp = user::convertToF ? convertToF(sensor::airTemp) : sensor::airTemp;

    // Check to see if the temperature is out of range
    if (airTemp > maxAirTemp && user::fansControlTemperature) {
      tempOutOfRange = abs(maxAirTemp - airTemp);     
      tempOption = device::SPEED_UP;
      tempPercent = tempOutOfRange / (maxAirTemp / 100.0);
      //printf("SPEED_UP tempPercent: %d\n", tempPercent);
    }
    else if (airTemp < minAirTemp && user::fansControlTemperature) {
      tempOutOfRange = abs(minAirTemp - airTemp);
      tempOption = device::SPEED_DOWN;
      tempPercent = tempOutOfRange / (minAirTemp / 100.0);
      //printf("SPEED_DOWN tempPercent: %d\n", tempPercent);
    }

    // Check to see if the humidity is out of range
    if (sensor::humidity > user::targetMaxHumidity && user::fansControlHumidity)  {
      humOutOfRange = abs(user::targetMaxHumidity - sensor::humidity);
      humOption = device::SPEED_UP;
      humPercent = humOutOfRange / (user::targetMaxHumidity / 100.0);
      //printf("SPEED_UP humPercent: %d\n", humPercent);
    }
    else if (sensor::humidity < user::targetMinHumidity && user::fansControlHumidity)  {
      humOutOfRange = abs(user::targetMinHumidity - sensor::humidity);
      humOption = device::SPEED_DOWN;
      humPercent = humOutOfRange / (user::targetMinHumidity / 100.0);
      //printf("SPEED_DOWN humPercent: %d\n", humPercent);
    }

    // Control the air heater
    static uint8_t prevAirHeaterMin = 69;
    if (rtc.minute() != prevAirHeaterMin) {
      if (airTemp <= minAirTemp && !device::airHeaterIsOn) {
        device::airHeaterIsOn = true;
        analogWriteChnl(pca9685Channel::airHeater, (device::relayOffState == HIGH ? 0 : 4096));
        saveLogMessage(11); // save log message, air heater on
      }
      else if (airTemp > minAirTemp &&device::airHeaterIsOn) {
        device::airHeaterIsOn = false;
        analogWriteChnl(pca9685Channel::airHeater, (device::relayOffState == HIGH ? 4096 : 0));
        saveLogMessage(12); // save log message, air heater off
      }
      prevAirHeaterMin = rtc.minute();
    }
    //printf("Temp out by: %.2f%%\n", tempOption == device::SPEED_UP ? tempPercent : -tempPercent);
    //printf("Hum out by %.2f%%\n", humOption == device::SPEED_UP ? humPercent : -humPercent);

    // Temperature and humidity are ok, so send 'just fan' IR command
    if (tempPercent <= 0.01 && humPercent <= 0.01) {
      // Fans should stay at current speed ?
      //printf("Temp and Hum ok, fans speed remains the same..\n");
    }
    // Temperature or humidity is out of range so work out which command needs to be sent
    else {
      // Check if the tempPercent > humPercent, else control the humidity
      if (tempPercent > humPercent)
        fanMode = tempOption;
      else
        fanMode = humOption;
      // adjust the fan speeds based on the fanMode and max min targets
      if (fanMode == device::SPEED_UP && device::fanOneSpeed < user::targetMaxFanOneSpeed)
        device::fanOneSpeed++;
      else if (fanMode == device::SPEED_DOWN && device::fanOneSpeed > user::targetMinFanOneSpeed)
        device::fanOneSpeed--;
      if (fanMode == device::SPEED_UP && device::fanTwoSpeed < user::targetMaxFanTwoSpeed)
        device::fanTwoSpeed++;
      else if (fanMode == device::SPEED_DOWN && device::fanTwoSpeed > user::targetMinFanTwoSpeed)
        device::fanTwoSpeed--;
    }
    // Send the new fan speeds to the Atmel328P
    if (device::fanOneSpeed != previousFanOneSpeed || device::fanTwoSpeed != previousFanTwoSpeed) {
      setFanSpeeds(device::fanOneSpeed, device::fanTwoSpeed);
      previousFanOneSpeed = device::fanOneSpeed;
      previousFanTwoSpeed = device::fanTwoSpeed;
    }
    previousMillis = millis();
  }
}

// Control the waters EC and or PH and or PMM
void waterEcPhControl() {
  enum adjustmentModes {UNADJUSTED, ADJUSTED_EC, ADJUSTED_PH};
  static adjustmentModes adjustmentMode = UNADJUSTED;
  static adjustmentModes lastAdjustmentMode = UNADJUSTED;
  // Every hour increment the dosing hour counter
  rtc.refresh();
  if (device::previousDosingHour != rtc.hour() && rtc.minute() >= device::previousDosingMinute) {
    device::dosingTimerHourCounter++;
    device::previousDosingHour = rtc.hour();
    printf("Dosing Timer Hour Counter: %d\n", device::dosingTimerHourCounter);
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
  printf("Attempting to adjust the EC...\n");
  if (sensor::ec < user::targetMinEc) {
    float percentage = percentOutOfRange(user::targetMinEc, sensor::ec);
    printf("EC is lower than min target by: %.2f%%\n", percentage);
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
        printf("EC/TDS dosing mode set to precise\n");
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
        printf("Add %d milliliters of nutrients, divided by %d EC dosers = %.2f\n", dosingAmount, numEnabledDosers, mlsPerDoser);
      }
      //
      else {
        printf("EC/TDS dosing mode set to incremental\n");
        for (int i = 0; i < 6; i++) {
          if (enabledDosers[i])
            dosingMls[i] = currentDoserMls[i];
        }
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        printf("Starting dosing...\n");
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 1, lastTouch);
      }     
    }
    device::dosingTimerHourCounter = 0;
  }
}

void adjustWaterTds() {
  printf("Attempting to adjust the TDS...\n");
  if (sensor::tds < user::targetMinTds) {
    float percentage = percentOutOfRange(user::targetMinTds, sensor::tds);
    printf("TDS is lower than min target by: %.2f%%\n", percentage);
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
        printf("EC/TDS dosing mode set to precise\n");
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
        printf("Add %d milliliters of nutrients, divided by %d EC dosers = %.2f\n", dosingAmount, numEnabledDosers, mlsPerDoser);
      }
      //
      else {
        printf("EC/TDS dosing mode set to incremental\n");
        for (int i = 0; i < 6; i++) {
          if (enabledDosers[i])
            dosingMls[i] = currentDoserMls[i];
        }
      }
      // Run the dosing pumps   
      if (dosingAmount > 0) {
        printf("Starting dosing...\n");
        device::currentlyDosing = true;
        runDosers(enabledDosers, dosingMls, percentage, 1, lastTouch);
      }     
    }
    device::dosingTimerHourCounter = 0;
  }
}

void adjustWaterPh() {
  printf("Attempting to adjust the PH...\n");
  if (sensor::ph < user::targetMinPh || sensor::ph > user::targetMaxPh) {
    float percentage = percentOutOfRange(sensor::ph < user::targetMinPh ? user::targetMinPh : user::targetMaxPh, sensor::ph);
    printf("PH is %s than min target by %.2f%%", (sensor::ph < user::targetMinPh ? "lower" : "higher"), percentage);
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
        printf("PH dosing mode set to precise\n");
        if (sensor::ph < user::targetMinPh) {
          // Work out the exact amout of PH up to dose
          dosingAmount = (user::targetMinPh - sensor::ph) * waterVolumeLtrs / sensor::phDownSolution;
          float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
          for (int i = 0; i < 6; i++) {
            if (enabledDosers[i])
              dosingMls[i] = mlsPerDoser;
          }
          printf("Add %d milliliters of PH up solution, divided by %d PH up dosers = %.2f\n", dosingAmount, numEnabledDosers, mlsPerDoser);
        }
        else if (sensor::ph > user::targetMaxPh) {
          // Work out the exact amout of PH down to dose
          dosingAmount = (sensor::ph - user::targetMaxPh) * waterVolumeLtrs / sensor::phUpSolution;
          float mlsPerDoser = (float)dosingAmount / numEnabledDosers;
          for (int i = 0; i < 6; i++) {
            if (enabledDosers[i])
              dosingMls[i] = mlsPerDoser;
          }
          printf("Add %d milliliters of PH down solution, divided by %d PH down dosers = %.2f\n", dosingAmount, numEnabledDosers, mlsPerDoser);
        } 
      }
      else {
        printf("PH dosing mode set to incremental\n");
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
        printf("Starting dosing...\n");
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
      printf("Dosing complete\n");
      device::currentlyDosing = false;
      clearPage();
      display::refreshPage = true;
    }
    // touch event - cancel button
    const uint16_t startX = 166, startY = 166;
    if (tft.touched()) {
      tft.touchReadPixel(&display::touch_x, &display::touch_y);
      if (millis() >= a_lastTouch) {
        if (display::touch_x >= startX + 200 && display::touch_x <= startX + 400 && display::touch_y >= startY + 200 && display::touch_y <= startY + 250) { // Cancel
          printf("Dosing aborted\n");
          beep();
          device::currentlyDosing = false;
          for (uint8_t i = 0; i < 6; i++)
            a_enabledDosers[i] = false;
          clearPage();
          display::refreshPage = true;
        }
      }
    }
  }
}

// Run a given doser for a_mls * 1000 on a_doserPin at a_doserSpeed
bool runDoser(const uint8_t& a_doserNum, const uint8_t& a_doserChnl, const int& a_doserSpeed, const float& a_mls, unsigned long& a_previousDoserMillis) {
  static unsigned long debugPreviousMillis = 0; // JUST FOR TESTING !
  if (millis() - a_previousDoserMillis <= 1000UL * a_mls) {
    if (a_previousDoserMillis != debugPreviousMillis) { // JUST FOR TESTING !  ----->
      printf("Starting doser: %d, pumping %d\n", a_doserNum, a_mls);
      debugPreviousMillis = a_previousDoserMillis;
    } // <---
    analogWriteChnl(a_doserChnl, a_doserSpeed);
    device::currentDoserNum = a_doserNum;
  }
  else {
    printf("Stopping doser: %d\n", a_doserNum); // JUST FOR TESTING !
    printf("Doser run time: %lu\n", millis() - a_previousDoserMillis); // JUST FOR TESTING !
    analogWriteChnl(a_doserChnl, 0);
    a_previousDoserMillis = millis();
    return false;
  }
  return true;
}
