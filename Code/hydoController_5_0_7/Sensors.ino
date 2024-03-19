void readSensors() {
  static uint8_t swapSenor;
  if (millis() - device::sensorPreviousMillis >= 2000UL && (display::page == 0 || display::page == 1 || display::page == 2 || display::page == 12 || millis() - display::lastTouchMillis > 10000UL)) {
    // WATER TEMP =========================================================================
    dallasTemperature.requestTemperatures();
    sensor::waterTemp = dallasTemperature.getTempCByIndex(0);
    //Serial.print(F("water temp in c:")); Serial.println(sensor::waterTemp);
    if (sensor::waterTemp < 0)
      sensor::waterTemp = 0;
    // WATER LEVEL ========================================================================
    if (user::heightSensor != user::ETAPE)
      sensor::waterLevel = sensor::emptyWaterTankDepth - getWaterHeight();
    else
      sensor::waterLevel = getWaterHeight();
    if (sensor::waterLevel < 0)
      sensor::waterLevel = 0;
    else if (sensor::waterLevel > 999)
      sensor::waterLevel = 999;
    // calculate the water volume
    sensor::waterVolumeLtrs = (sensor::waterLevel * user::waterTankLength * user::waterTankWidth) / 1000.0; // mls to ltrs
    // AIR TEMP, HUMIDITY AND CO2 ========================================================
    if(SCD4X.getDataReadyStatus()) {
      DFRobot_SCD4X::sSensorMeasurement_t data;
      SCD4X.readMeasurement(&data);
      sensor::airTemp = data.temp;
      sensor::humidity = data.humidity;    
      sensor::co2 = data.CO2ppm;
      //Serial.print(F("Air temp in c:")); Serial.println(sensor::airTemp);
      //Serial.print(F("Humidity:")); Serial.println(sensor::humidity);
      //Serial.print(F("Co2:")); Serial.println(sensor::co2);
      if (sensor::airTemp < 0)
        sensor::airTemp = 0;    
      if (sensor::humidity > 99.9)
        sensor::humidity = 99.9;
      if (sensor::co2 < 0)
        sensor::co2 = 0;
      else if (sensor::co2 > 9999)
        sensor::co2 = 9999;
    }
    // LDR ===============================================================================
     sensor::ldr = analogReadChnl(ads1115Channel::ldr);
    //Serial.print(F("LDR: ")); Serial.println(sensor::ldr);
    if (device::lightOn) {     
      if (sensor::ldr < 200) { // light is off when it should be on
        rtc.refresh();
        static uint8_t prevLightErrorDay = 99;
        if (rtc.day() != prevLightErrorDay){
          if (device::lightDuration >= 2) {
            saveLogMessage(6); // log an error message once per day
            prevLightErrorDay = rtc.day();
          }
        }
      }
    }
    // CO2 ===============================================================================  
    Serial2.flush();
    Serial2.write(sensor::co2Request, 9);
    uint8_t buffer[8] {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t bufferPosition = 0;
    device::prevMillis = millis();
    while (millis() - device::prevMillis <= 50UL && Serial2.available() > 0) {
      char c = Serial2.read();
      if (bufferPosition < 8)
        buffer[bufferPosition++] = c;
    }
    float sensorValue = 0;
    if (buffer[0] == 255 && buffer[5] == 0 && buffer[7] == 0)
      sensorValue = (256 * (int16_t)buffer[2]) + (int16_t)buffer[3];
    if (sensorValue <= 0) {
      if (device::globalDebug) {
        Serial.print(F("Error reading Co2 sensor buffer: "));
        for (const uint8_t& val : buffer) {
          Serial.print(val);
        }
        Serial.println();
      }
      sensor::co2 = 0;
    }
    else if (sensorValue < 300) {
       if (device::globalDebug)
         Serial.println(F("Co2 sensor currently preheating..."));
      sensor::co2 = 0;
    }
    else {
      sensor::co2 = sensorValue;
      //Serial.print(F("co2: ")); Serial.println(sensor::co2);
    }
    if (sensor::co2 < 0)
      sensor::co2 = 0;
    else if (sensor::co2 > 9999)
      sensor::co2 = 9999;
    // EC & PPM / TDS ===================================================================
    if (swapSenor == 0) {
      if (!device::ecWaitTillNextCall) {
        digitalWrite(pin::tdsTransistor, HIGH);
        device::ecWaitTillNextCall = true;
      }
      else {
        gravityTds.setTemperature(sensor::waterTemp);
        float voltage = analogReadChnlToVolts(ads1115Channel::tdsSensor);  
        gravityTds.updateVoltage(voltage);  //sample and calculate    
        sensor::ec = gravityTds.getEcValue() / 1000.0; // Convert uS to mS
        sensor::tds = gravityTds.getTdsValue();
        //Serial.print(F("EC:")); Serial.println(sensor::ec, 2);
        //Serial.print(F("TDS:")); Serial.println(sensor::tds);
        swapSenor = 1;
        device::ecWaitTillNextCall = false;
        digitalWrite(pin::tdsTransistor, LOW);
      }
    }
    // PH ==============================================================================
    else if (swapSenor == 1) {
      if (!device::phWaitTillNextCall) {
        digitalWrite(pin::phTransistor, HIGH);
        device::phWaitTillNextCall = true;
      }
      else {
        float voltage = analogReadChnlToVolts(ads1115Channel::phSensor) * 1000.0; // read the voltage in mV  
        sensor::ph = ph.readPH(voltage, sensor::waterTemp); // ph.calibration(voltageEC, sensor::waterTemp, cmd);
        //Serial.print(F("PH:")); Serial.println(sensor::ph, 2);
        if (sensor::ph > 14)
          sensor::ph = 14.0;
        else if (sensor::ph < 0)
          sensor::ph = 0;
        device::sensorsReady = true;
        swapSenor = 0;
        device::phWaitTillNextCall = false;
        digitalWrite(pin::phTransistor, LOW);
      }
    }
    if (device::sensorsReady) {
      pushBackDataArrays();
      setMaxMins();
    }
    device::sensorPreviousMillis = millis();
  }
}

// calibrate the co2 sensor
void calibrateCo2() {
  if (display::refreshPage)
    continueMessage(message::calibratingCo2, -1, 0, false, false, false);
  const uint8_t calibrateCo2[] {0XFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
  Serial2.write(calibrateCo2, 9);
  display::refreshPage = true;
  clearPage();
}

float getWaterHeight() {
  float waterLevel = 0;
  if (user::heightSensor == user::ETAPE) {
    //if (device::globalDebug)
      //Serial.print(F("ETAPE"));
    float resistance = readResistance(ads1115Channel::etapeSensor, 560);
    waterLevel = resistanceToCM(resistance, sensor::etapeZeroVolumeResistance, sensor::etapeMaxVolumeResistance, sensor::etapeCalibrationCm);
    waterLevel += sensor::etapeOffset;
    if (waterLevel > sensor::etapeCalibrationCm)
      waterLevel = sensor::etapeCalibrationCm;
  }
  else if (user::heightSensor == user::VL53L0X) {
    //if (device::globalDebug)
      //Serial.print(F("VL53L0X"));
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout! 
    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
      waterLevel = measure.RangeMilliMeter / 10.0; // in cm
      //Serial.print("waterLevel (cm): "); Serial.println(waterLevel);
    }   
    else {
      //Serial.println("VL53L0X out of range ");
    }
  }
  if (device::globalDebug) {
    //Serial.print(F(" water level: ")); Serial.println(sensor::waterLevel);
  }
  return waterLevel;
}

// Get the water tanks empty depth in cm
void setTankDepth() {
  float distance = getWaterHeight();
  if (distance > 0 && distance < 999) {
    sensor::emptyWaterTankDepth = distance;
    if (device::globalDebug) {
      Serial.print(F("EmptyWaterTankDepth = ")); Serial.println(sensor::emptyWaterTankDepth, 2);
    }
  }
  else {
    sensor::emptyWaterTankDepth = 0;
    if (device::globalDebug)
      Serial.println(F("Error: tank depth <= 0 or >= 999\n"));
  }
}

// This is a generic Co2 formula tried and tested.
void generateCo2GasTime() {
  float roomHeight = 0.0328084 * user::roomHeightCm; // convert cm to feet
  float roomWidth = 0.0328084 * user::roomWidthCm; // convert cm to feet
  float roomLenght = 0.0328084 * user::roomLengthCm; // convert cm to feet
  float flowrate = user::co2FlowrateLtrs / 28.3168; // convert ltr to feet cubed
  if (user::convertToInches) {
    roomHeight = 0.0833333 * user::roomHeightInches; // convert inches to feet
    roomWidth = 0.0833333 * user::roomWidthInches; // convert inches to feet
    roomLenght = 0.0833333 * user::roomLengthInches; // convert inches to feet
    flowrate = user::co2FlowrateFeet3; // already in feet cubed
  }
  
  float roomsize = roomLenght * roomWidth * roomHeight;
  float co2PpmMargin = user::targetCo2 - sensor::co2;
  co2PpmMargin *= 0.000001;
  //flowrate = flowrate / 60.0; // If in hours
  float gasTime = (roomsize * co2PpmMargin) / flowrate;
  if (device::globalDebug) {
    Serial.print(F("Gas time in minues = ")); Serial.println(gasTime);
  }
  sensor::co2GasTime = gasTime * 60000UL;
  if (device::globalDebug) {
    Serial.print(F("Co2 gas time in millis = ")); Serial.println(sensor::co2GasTime);
  }
}

bool phCallbration() {
  bool returnVal = false;
  if (device::globalDebug)
    Serial.println(F("Starting PH sensor calibration"));
  // Turn the PH sensor off
  digitalWrite(pin::tdsTransistor, LOW);
  // Turn on the TDS sensor
  digitalWrite(pin::phTransistor, HIGH);
  // allow TDS sensor to power up
  delay(3000);
  // Get the water temperature in celsius
  dallasTemperature.requestTemperatures();
  sensor::waterTemp = dallasTemperature.getTempCByIndex(0);
  // Get the average voltage
  float voltage = 0;
  for (int16_t i = 0; i < 10; i++) {
    voltage = analogReadChnlToVolts(ads1115Channel::phSensor) * 1000.0; // read the voltage in mv   
    delay(250);
  } 
  voltage = voltage / 10.0;
  if (device::globalDebug) {
    Serial.print(F("Raw PH voltage in mV = ")); Serial.println(voltage, 2);
  }
  // Attempt the calibration
  if(ph.calibration(voltage, sensor::waterTemp))
    returnVal = true;
  // Turn off the TDS sensor
  digitalWrite(pin::tdsTransistor, LOW);
  return returnVal;
}

bool tdsCalibration(const bool a_lowCal) {
  bool returnVal = false;
  if (device::globalDebug)
    Serial.println(F("Starting TDS sensor calibration"));
  // Turn the PH sensor off
  digitalWrite(pin::phTransistor, LOW);
  // Turn on the TDS sensor
  digitalWrite(pin::tdsTransistor, HIGH);
  // allow TDS sensor to power up
  delay(3000);
  // Get the water temperature in celsius
  dallasTemperature.requestTemperatures();
  sensor::waterTemp = dallasTemperature.getTempCByIndex(0);
  gravityTds.setTemperature(sensor::waterTemp);
  // Attempt the calibration
  float voltage = analogReadChnlToVolts(ads1115Channel::tdsSensor);  
  if (a_lowCal && gravityTds.calibrateLow(voltage))
    returnVal = true;
  else if (!a_lowCal && gravityTds.calibrateHigh(voltage))
    returnVal = true;
  // Turn off the TDS sensor
  digitalWrite(pin::phTransistor, LOW);
  return returnVal;
}

void setEtapeZeroVolumeResistance() {
  sensor::temp_etapeZeroVolumeResistance = readResistance(ads1115Channel::etapeSensor, 560);
  if (device::globalDebug) {
    Serial.print(F("Etape zero volume resistance = ")); Serial.print(sensor::etapeZeroVolumeResistance, 2); Serial.println(F(" Ohms"));
  }
}

void setEtapeMaxVolumeResistance() {
  sensor::etapeZeroVolumeResistance = sensor::temp_etapeZeroVolumeResistance;
  sensor::temp_etapeZeroVolumeResistance = 0;
  sensor::etapeMaxVolumeResistance = readResistance(ads1115Channel::etapeSensor, 560);
  if (device::globalDebug) {
    Serial.print(F("Etape max reading resistance = ")); Serial.print(sensor::etapeMaxVolumeResistance, 2); Serial.println(F(" Ohms"));
  }
}

float readResistance(const int16_t& a_chnl, const int16_t& a_seriesResistance) {
  // Get ADC value.
  float resistance = analogReadChnl(a_chnl);
  // Convert ADC reading to resistance.
  resistance = (4096 / resistance) - 1.0;
  resistance = a_seriesResistance / resistance;
  return resistance;
}

float resistanceToCM(const float& a_resistance, const float& a_zeroResistance, const float& a_calResistance, const uint8_t a_calCM) {
  if (a_resistance > a_zeroResistance || (a_zeroResistance - a_calResistance) == 0.0) {
    // Stop if the value is above the zero threshold, or no max resistance is set (would be divide by zero).
    return 0.0;
  }
  // Compute scale factor by mapping resistance to 0...1.0+ range relative to maxResistance value.
  float scale = (a_zeroResistance - a_resistance) / (a_zeroResistance - a_calResistance);
  // Scale maxVolume based on computed scale factor.
  return a_calCM * scale;
}
