/*!
 * @file DFRobot_PH.cpp
 * @brief Arduino library for Gravity: Analog pH Sensor / Meter Kit V2, SKU: SEN0161-V2
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Jiawei Zhang](jiawei.zhang@dfrobot.com)
 * @version  V1.0
 * @date  2018-11-06
 * @url https://github.com/DFRobot/DFRobot_PH
 */


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DFRobot_PH.h"


DFRobot_PH::DFRobot_PH() {
    this->m_acidVoltage    = 2032.44;    //buffer solution 4.0 at 25C
    this->m_neutralVoltage = 1500.0;     //buffer solution 7.0 at 25C
}

DFRobot_PH::~DFRobot_PH()
{
 // Nothing going on here yet...
}

void DFRobot_PH::begin(const float a_acidVoltage, const float a_neutralVoltage) {
	this->m_acidVoltage = a_acidVoltage;
	this->m_neutralVoltage = a_neutralVoltage;
}

// TO DO: PH temperature compensation
float DFRobot_PH::readPH(const float a_voltage, const float a_temperature) {
    float slope = (7.0-4.0)/((this->m_neutralVoltage-1500.0)/3.0 - (this->m_acidVoltage-1500.0)/3.0);  // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    float intercept =  7.0 - slope*(this->m_neutralVoltage-1500.0)/3.0;
    //Serial.print("slope:"); Serial.println(slope);
    //Serial.print("intercept:"); Serial.println(intercept);
    float phValue = slope*(a_voltage-1500.0)/3.0+intercept;  //y = k*x + b
	//Serial.print("phValue:"); Serial.println(phValue);
    return phValue;
}

// TO DO: PH temperature compensation
bool DFRobot_PH::calibration(const float a_voltage, const float a_temperature) {
	Serial.println(F(">>>Enter PH Calibration Mode<<<"));
	Serial.println(F(">>>Please put the probe into the 4.0 or 7.0 standard buffer solution<<<"));
	if (a_voltage > 1322 && a_voltage < 1678){        // buffer solution:7.0{
		Serial.print(F("Detected buffer Solution: 7.0"));
		this->m_neutralVoltage =  a_voltage;
		Serial.println(F("PH calibration successful"));
		Serial.print(F("Neutral voltage = ")); Serial.println(this->m_neutralVoltage); 
		return true;
	}
	else if (a_voltage > 1854 && a_voltage < 2210) {  //buffer solution:4.0
		Serial.print(F("Detected buffer solution: 4.0"));
		this->m_acidVoltage =  a_voltage;
		Serial.println(F("PH calibration successful"));
		Serial.print(F("Acidic voltage = ")); Serial.println(this->m_acidVoltage); 
		return true;
	}
	else {
		Serial.print(F("PH calibration error try again"));  // not buffer solution or faulty operation
		return false;
	}
}

float DFRobot_PH::getAcidicVoltage() {
	return this->m_acidVoltage;
}

float DFRobot_PH::getNeutralVoltage() {
	return this->m_neutralVoltage;
}