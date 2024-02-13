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

#ifndef GRAVITY_TDS_H
#define GRAVITY_TDS_H

#include "Arduino.h"


class GravityTDS
{
public:
    GravityTDS();
    ~GravityTDS();
    void update(const int a_analogValue, const int a_averageCnt = 1, const unsigned long a_averageDelay = 100UL); //read and calculate
    void setTemperature(const float temp);  //set the temperature and execute temperature compensation
    void setAref(const float value);  //reference voltage on ADC, default 5.0V on Arduino UNO
    void setAdcRange(const float range);  //1024 for 10bit ADC;4096 for 12bit ADC
	void setRawEcLowSolution(const float a_rawEcSolution);
	void setRawEcHighSolution(const float a_rawEcSolution);
	void setTdsFactor(const float a_tdsFactor);
	void setKvalueLow(const float a_kValueLow);
	void setKvalueHigh(const float a_kValueHigh);
    float getKvalueLow(); 
	float getKvalueHigh(); 
    float getTdsValue();
    float getEcValue();
	float getRawEcLowSolution();
	float getRawEcHighSolution();
	float getTdsFactor();
	bool calibrateLow(const int a_analogValue);
	bool calibrateHigh(const int a_analogValue);
	
private:
	bool calibrate(const int a_analogValue, const float a_rawEcSolution, float& a_kValue);
    float aref;  // default 5.0V on Arduino UNO
    float adcRange;
    float temperature;
	float tdsFactor;
    float kValueLow;      // k value of the probe,you can calibrate in buffer solution ,such as 706.5ppm(1413us/cm)@25^C 
	float kValueHigh;      // k value of the probe,you can calibrate in buffer solution ,such as 706.5ppm(1413us/cm)@25^C 
    float ecValue25; //after temperature compensation
    float tdsValue;
	float rawEcLowSolution;
	float rawEcHighSolution;
};  

#endif
