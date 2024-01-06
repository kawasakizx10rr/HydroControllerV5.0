/*
  Note: This code is designed to run as fast as possible, due to the precise timing
  required to accurately chop up the AC wave form and read i2c commands. The loop
  time must be less than 83-100uS at any given time, currently 18uS is the max loop
  time with all functions running, as a result port manipulation is used throughout.
  --------------------------------------------------------------------------
  SwitchCases:
  Switch     Value range     Modes                                  Example
  Z          0 > 100         FAN_ONE_SPEED_IN_PERCENT               Z50
  X          0 > 100         FAN_TWO_SPEED_IN_PERCENT               X75
  --------------------------------------------------------------------------
  PINS:
  AC interruptor  = PD2 / PIN 2
  fanOne          = PD3 / PIN 3
  fanTwo          = PD4 / PIN 4
*/

#include <Wire.h>

const byte deviceAddress = 9;
byte halfWaveDuration = 0;
volatile unsigned long halfWaveMeasurement = 0;
volatile bool getHalfWaveDuration = true;
volatile unsigned long fanOneTiming = 0, fanTwoTiming = 0;
volatile unsigned long fanOneCutOffPoint = 0, fanTwoCutOffPoint = 0;
volatile bool finishFanOneCutOff = false, finishFanTwoCutOff = false;

void setup() {
  //Serial.begin(115200); // Just for debugging, DO NOT use outside testing it will slow the code down!
  DDRD = B00011000; // Sets pins 3 to 4 as outputs
  Wire.begin(deviceAddress); // Start the I2C Bus as Slave on address 9 (0 - 100) are available connections
  Wire.onReceive(i2cReceiveEvent); // Attach a function to trigger when i2c data is received.
  attachInterrupt(digitalPinToInterrupt(2), zeroCrosssEvent, RISING);
}

void loop() {
  //unsigned long loopTime = micros();
  cutOff();
  //Serial.println(micros() - loopTime); // loop time in uS
}

void i2cReceiveEvent(const int cnt) {
  static char switchCase = 0;
  static byte value = 0;
  if (cnt == 2) {
    switchCase = Wire.read();
    value = Wire.read();
  }
  else if (switchCase == 'Z') {
    if (value == 0) { // user fan 1 speed is 0% so turn off fan 1 constantly
      PORTD &= ~_BV(PD3); // pin 3 low
      fanOneTiming = 0;
      finishFanOneCutOff = false;
      //Serial.println(F("f1 off"));
    }
    else if (value == 100) { // user fan 1 speed is 100% so turn on fan 1 constantly
      PORTD |= _BV(PD3); // pin 3 high
      fanOneTiming = 0;
      //Serial.println(F("f1 on"));
    }
    else { // else fan 1 speed is 1% to 99% so set the fan 1 speed timing
      fanOneTiming = (100 - value) * halfWaveDuration; // map speed to max half sine wave duration in uS
      //Serial.print(F("f1 uS:")); Serial.println(fanOneTiming);
    }
  }
  else if (switchCase == 'X') {
    if (value == 0) { // user fan 2 speed is 0% so turn off fan 2 constantly
      PORTD &= ~_BV(PD4); // pin 4 low
      fanTwoTiming = 0;
      finishFanTwoCutOff = false;
      //Serial.println(F("f2 off"));
    }
    else if (value == 100) { // user fan 2 speed is 100% so turn on fan 2 constantly
      PORTD |= _BV(PD4); // pin 4 high
      fanTwoTiming = 0;
      //Serial.println(F("f2 on"));
    }
    else { // else fan 2 speed is 1% to 99% so set the fan 2 speed timing
      fanTwoTiming = (100 - value) * halfWaveDuration;
      //Serial.print(F("f2 uS:")); Serial.println(fanTwoTiming);
    }
  }
}

void zeroCrosssEvent() {
  // On boot of device, work out the AC half wave duration to see if it is 50Hz or 60Hz.
  if (getHalfWaveDuration) {
    if (halfWaveMeasurement == 0)
      halfWaveMeasurement = micros();
    else {
      halfWaveMeasurement = micros() - halfWaveMeasurement;
      if (halfWaveMeasurement >= 8100 && halfWaveMeasurement <= 8500) {
        halfWaveDuration = 83;
        getHalfWaveDuration = false;
      }
      else if (halfWaveMeasurement >= 9800 && halfWaveMeasurement <= 10200) {
        halfWaveDuration = 100;
        getHalfWaveDuration = false;
      }
      else {
        halfWaveMeasurement = 0;
      }
      // just for debugging
      if (halfWaveDuration != 0) {
        Serial.print(F("HWD:")); Serial.println(halfWaveDuration);
      }
    }
  }
  // Else set the AC cutoff peroid for fan one and fan two.
  else {
    if (fanOneTiming != 0) {
      fanOneCutOffPoint = micros() + fanOneTiming; // set cut off period
      PORTD &= ~_BV(PD3); // pin 3 low
      finishFanOneCutOff = true;
    }
    if (fanTwoTiming != 0) {
      fanTwoCutOffPoint = micros() + fanTwoTiming;
      PORTD &= ~_BV(PD4); // pin 4 low
      finishFanTwoCutOff = true;
    }
  }
}

// turn AC back on for fan x after fanXCutOffPoint period
void cutOff() {
  if (finishFanOneCutOff && micros() >= fanOneCutOffPoint) {
    PORTD |= _BV(PD3); // pin 3 high
    finishFanOneCutOff = false;
  }
  if (finishFanTwoCutOff && micros() >= fanTwoCutOffPoint) {
    PORTD |= _BV(PD4); // pin 4 high
    finishFanTwoCutOff = false;
  }
}
