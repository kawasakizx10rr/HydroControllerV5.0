/***************************************************
 DFRobot Gravity: Analog TDS Sensor/Meter
 <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_TDS_Sensor_/_Meter_For_Arduino_SKU:_SEN0244>
 
 ***************************************************
 This sample code shows how to read the tds value and calibrate it with the standard buffer solution.
 707ppm(1413us/cm)@25^c standard buffer solution is recommended.
 
 Created 2018-1-3
 By Jason <jason.ling@dfrobot.com@dfrobot.com>
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/
 
 /***********Notice and Trouble shooting***************
 1. This code is tested on Arduino Uno with Arduino IDE 1.0.5 r2 and 1.8.2.
 2. Calibration CMD:
     enter -> enter the calibration mode
     cal:tds value -> calibrate with the known tds value(25^c). e.g.cal:707
     exit -> save the parameters and exit the calibration mode
 ****************************************************/
//https://www.snowate.com/knowledge-calculator/calculator/tds-ec-conversion.html

#include "GravityTDS.h"

#include <OneWire.h>
#include <DallasTemperature.h>

const uint8_t tdsSensorPin = A1; // pin::tdsSensor
const uint8_t tdsTransistor = 37;
const uint8_t oneWireBus = 38;

bool usingDs18b20 = true;

GravityTDS gravityTds;
// for reading the temperature
OneWire oneWire(oneWireBus);
DallasTemperature dallasTemperature(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1000);
   if (!dallasTemperature.begin()) {
    Serial.println(F("Error: Failed to load DS18B20, using a fixed temperature of 25c"));
    usingDs18b20 = false;
  }
  pinMode(tdsSensorPin, INPUT);
  pinMode(tdsTransistor, OUTPUT);
  digitalWrite(tdsTransistor, HIGH);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.setTdsFactor(0.5);  // 0.64 for EU, 0.70 for AU and 0.5 for US
  gravityTds.setRawEcLowSolution(1413); // 707ppm(1413)@25^c	is recommended, but you can use any solution > 0 && < 2000
  gravityTds.setRawEcHighSolution(2760); // 707ppm(1413us/cm)@25^c	is recommended, but you can use any solution > 0 && < 2000
  //gravityTds.setKvalue(1.41); // Set the know kValue after a successfull calibration
  delay(2000); // Allow the TDS module to power up fully after turning on the transistor
}

void loop() {
  static bool isCalibratedLow = false, isCalibratedHigh = false;
  float temperature = 25.0;

  if (usingDs18b20) {
    dallasTemperature.requestTemperatures();
    temperature = dallasTemperature.getTempCByIndex(0);
    Serial.print(F("Temperature=")); Serial.println(temperature, 2);
  }

  if (!isCalibratedLow) {
    Serial.print(F("Please put the TDS probe in ")); Serial.print(gravityTds.getRawEcLowSolution()); Serial.println(F(" us/cm buffer solutuion and press enter in the serial montor."));
    while(Serial.available() == 0) {};
    Serial.read();
    gravityTds.setTemperature(temperature); // Add your real temperature here...
    if (gravityTds.calibrateLow(analogRead(tdsSensorPin))) {
      Serial.print(F("Calibration low successful, K=")); Serial.println(gravityTds.getKvalueLow());
      isCalibratedLow = true;
    }
    else {
      Serial.println(F("Calibration low failed, try again..."));
    }
  }
  else if (!isCalibratedHigh) {
    Serial.print(F("Please put the TDS probe in ")); Serial.print(gravityTds.getRawEcHighSolution()); Serial.println(F(" us/cm buffer solutuion and press enter in the serial montor."));
    while(Serial.available() == 0) {};
    Serial.read();
    gravityTds.setTemperature(temperature); // Add your real temperature here...
    if (gravityTds.calibrateHigh(analogRead(tdsSensorPin))) {
      Serial.print(F("Calibration high successful, K=")); Serial.println(gravityTds.getKvalueHigh());
      isCalibratedHigh = true;
    }
    else {
      Serial.println(F("Calibration high failed, try again..."));
    }
  }
  else {
    gravityTds.setTemperature(temperature); // Add your real temperature here...
    gravityTds.update(analogRead(tdsSensorPin), 5, 10);  //sample and calculate 
    int tdsValue = gravityTds.getTdsValue();  // then get the value
    float ecValue = gravityTds.getEcValue();  // then get the value
    Serial.print(F("TDS: ")); Serial.print(tdsValue); Serial.println(F("ppm"));
    Serial.print(F("EC: ")); Serial.println(ecValue, 2);   
  }

  delay(2000);
}
