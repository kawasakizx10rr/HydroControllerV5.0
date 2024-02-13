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


#include "GravityTDS.h"

GravityTDS::GravityTDS() {
    temperature = 25.0;
    aref = 5.0;
    adcRange = 1024.0;
    kValueLow = 1.0;
	kValueHigh = 1.0;
	tdsFactor = 0.5;
	rawEcLowSolution = 1413;
	rawEcLowSolution = 2760;
}

GravityTDS::~GravityTDS() {}

void GravityTDS::setTdsFactor(const float a_tdsFactor) {
	tdsFactor = a_tdsFactor;
}

void GravityTDS::setTemperature(const float a_temp) {
	temperature = a_temp;
}

void GravityTDS::setAref(const float a_value) {
	aref = a_value;
}

void GravityTDS::setAdcRange(const float a_range) {
    adcRange = a_range;
}

void GravityTDS::setRawEcLowSolution(const float a_rawEcSolution) {
    rawEcLowSolution = a_rawEcSolution;
}

void GravityTDS::setRawEcHighSolution(const float a_rawEcSolution) {
    rawEcHighSolution = a_rawEcSolution;
}

void GravityTDS::setKvalueLow(const float a_kValueLow) {
	kValueLow = a_kValueLow;
}

void GravityTDS::setKvalueHigh(const float a_kValueHigh) {
	kValueHigh = a_kValueHigh;
}

float GravityTDS::getKvalueLow() {
	return kValueLow;
}

float GravityTDS::getKvalueHigh() {
	return kValueHigh;
}

float GravityTDS::getTdsValue() {
	return tdsValue;
}

float GravityTDS::getEcValue() {
    return ecValue25;
}

float GravityTDS::getTdsFactor() {
    return tdsFactor;	
}

float GravityTDS::getRawEcLowSolution() {
    return rawEcLowSolution;	
}

float GravityTDS::getRawEcHighSolution() {
    return rawEcHighSolution;	
}

void GravityTDS::update(const int a_analogValue, const int a_averageCnt, const unsigned long a_averageDelay) {
	float voltage = 0;
	for (uint8_t i = 0; i < a_averageCnt; i++) {
		voltage += a_analogValue/adcRange*aref;
		delay(a_averageDelay);
	}
	voltage = voltage / a_averageCnt;
	Serial.print(F("Voltage: ")); Serial.println(voltage, 2);
	float kValue = voltage < 2.0 ? kValueLow : kValueHigh;
	float ecValue=(133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage)*kValue;
	ecValue25 = ecValue / (1.0+0.02*(temperature-25.0));  //temperature compensation
	tdsValue = ecValue25 * tdsFactor;
}

bool GravityTDS::calibrateLow(const int a_analogValue) {
	return calibrate(a_analogValue, rawEcLowSolution, kValueLow);		
}

bool GravityTDS::calibrateHigh(const int a_analogValue) {
	return calibrate(a_analogValue, rawEcHighSolution, kValueHigh);	
}

bool GravityTDS::calibrate(const int a_analogValue, const float a_rawEcSolution, float& a_kValue) {
	Serial.print(F("\nCalibrating using EC ")); Serial.print(a_rawEcSolution); Serial.println(F(" buffer solution"));
	const float rawEcSolution25 = a_rawEcSolution*(1.0+0.02*(temperature-25.0));
	Serial.print(F("Temperature adjusted EC ")); Serial.println(rawEcSolution25);
	
	float voltage = 0;
	for (uint8_t i = 0; i < 10; i++) {
		voltage += a_analogValue/adcRange*aref;
		delay(100);
	}
	voltage = voltage / 10.0;	
	
	float kValueTemp = rawEcSolution25/(133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage);  //calibrate in the  buffer solution, such as 707ppm(1413us/cm)@25^c	
	if(rawEcSolution25 > 0 && rawEcSolution25 < 4000 && kValueTemp > 0.25 && kValueTemp < 4.0) {	
		a_kValue =  kValueTemp;
		return true;
	}
	return false;	
}