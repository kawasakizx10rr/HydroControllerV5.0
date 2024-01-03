/*!
 * @file DFRobot_PH.h
 * @brief Arduino library for Gravity: Analog pH Sensor / Meter Kit V2, SKU: SEN0161-V2
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Jiawei Zhang](jiawei.zhang@dfrobot.com)
 * @version  V1.0
 * @date  2018-11-06
 * @url https://github.com/DFRobot/DFRobot_PH
 */

#ifndef _DFROBOT_PH_H_
#define _DFROBOT_PH_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class DFRobot_PH
{
public:
  DFRobot_PH();
  ~DFRobot_PH();
  /**
   * @fn calibration
   * @brief Calibrate the calibration data
   *
   * @param a_voltage     : Voltage value
   * @param a_temperature : Ambient temperature
   */
  bool calibration(const float a_voltage, const float a_temperature);
  /**
   * @fn readPH
   * @brief Convert voltage to PH with temperature compensation
   * @note voltage to pH value, with temperature compensation
   *
   * @param a_voltage     : Voltage value
   * @param a_temperature : Ambient temperature
   * @return The PH value
   */
  float readPH(const float a_voltage, const float a_temperature); 
  /**
   * @fn begin
   * @brief Initialization The Analog pH Sensor
   */
  void begin(const float a_acidVoltage = 2032.44, const float a_neutralVoltage = 1500.0);
  
  float getAcidicVoltage();
  float getNeutralVoltage();

private:
    float  m_acidVoltage;
    float  m_neutralVoltage;
};

#endif
