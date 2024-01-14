/*
  Version V5.0.2 Updated on the 8th Jan 2024
*/

// Library imports
#include <SPI.h>
#include <RA8875.h>
#include <Wire.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <uRTCLib.h>
#include <Adafruit_HDC1000.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_ADS1X15.h>
#include <SparkFun_External_EEPROM.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"
#include <WebServer.h>
#include "index.h" //Web page HTML file
// Include font files
#include "Fonts/Aerial_22.h"
#include "Fonts/Akashi_36.h"
#include "Fonts/Hallfetica_42_Nums.h"
#include "Fonts/MyriadPro_32.h"
#include "Icons.h"
// ESP32 Watchdog
#include "soc/rtc_wdt.h"

// i2c addresses used
// ADS1115  = 72 (options are: 72, 73, 74, 75) 4-Channel ADC
// PCA9685  = 65 (options are: 64 to 127) 16-Channel 12-bit PWM/Servo Driver
// AD5245   = 44 (options are: 44 or 45) AD5245BRJZ10-RL7 10K Omhs
// HDC10180 = 64 
// DS3231   = 104 and 87 (Two addresses are used as the rtc eeprom is addressable too)
// 24LC256  = 80

// pins used
namespace pin {
// I2C 
// SCL = 22
// SDA = 21
const uint8_t rxd2 = 16;
const uint8_t txd2 = 17;
// display
const uint8_t ra8875_rst = 33;
const uint8_t ra8875_cs = 32; // was 5
const uint8_t ra8875_sdo = 19;
const uint8_t ra8875_sdi = 23;
const uint8_t ra8875_sck = 18;
// temp / humidity sensors
const uint8_t oneWireBus = 26;
// Transistors
const uint8_t phTransistor = 4;
const uint8_t tdsTransistor = 27;
// Ultrasonic sensor
const uint8_t hcsrEcho = 5;
const uint8_t hcsrTrigger = 15;
}

enum fanModes {
  FAN_ON,
  FAN_OFF,
  FAN_AUTO
};

enum ads1115Channel {
  ldr,          // ADS1115 channel 1
  tdsSensor,    // ADS1115 channel 2
  phSensor,     // ADS1115 channel 3
  etapeSensor,  // ADS1115 channel 4
};

enum pca9685Channel {
  doserOne,     // PCA9685 channel 1
  doserTwo,     // PCA9685 channel 2
  doserThree,   // PCA9685 channel 3
  doserFour,    // PCA9685 channel 4
  doserFive,    // PCA9685 channel 5
  doserSix,     // PCA9685 channel 6
  airHeater,    // PCA9685 channel 7
  co2Solenoid,  // PCA9685 channel 8
  inletPump,    // PCA9685 channel 9
  outletPump,   // PCA9685 channel 10
  light,        // PCA9685 channel 11
  waterHeater,  // PCA9685 channel 12
  ledRed,       // PCA9685 channel 13
  ledBlue,      // PCA9685 channel 14
  ledGreen,     // PCA9685 channel 15
  speaker       // PCA9685 channel 16
};

namespace display {
uint16_t touch_x, touch_y = 0;
uint16_t lastTouchX = 0, lastTouchY = 0;
uint8_t dosingDialogTimer = 10;
uint8_t page = 0, previousPage = 0;
uint8_t homePage = 0;
bool showTdsGraph = true;
bool showPhGraph = true;
bool showEcGraph = true;
bool showCo2Graph = true;
bool showWaterTempGraph = true;
bool showWaterLvlGraph = true;
bool showFanOneGraph = true;
bool showFanTwoGraph = true;
bool showAirTempGraph = true;
bool showHumidityGraph = true;
bool showCalErrorMessage = false;
bool showingDialog = false;
bool showWifiSsid = false; // TO DO
bool showWifiPassword = false; // TO DO
bool showSystemLogs = false;
bool showAfkTime = false;
bool showGraphInterval = false;
bool showNumberOfDosers = false;
bool showDosingInterval = false;
bool showEcDosingMode = false;
bool showPhDosingMode = false;
bool showEcTdsValue = false;
bool showPhDownUpValue = false;
bool showSr04HeightCalibration = false;
uint8_t showEtapeCalibration = 0;
bool showDoserCalibration = false;
bool showTdsCalibration = false;
bool showCo2Calibration = false;
bool showPhCalibration = false;
bool showRTCtime = false;
bool showKeyboard = false;
bool showSaveDialog = false;
bool showInfoDialog = false; /////////
uint8_t co2PageScrollPos = 0;
uint8_t waterPageScrollPos = 0;
uint8_t doserPageScrollPos = 0;
uint8_t calDoserPageScrollPos = 0;
uint8_t calPhPageScrollPos = 0;
uint8_t calTdsPageScrollPos = 0;
uint8_t refillDoserPageScrollPos = 0;
uint8_t settingsPage = 0;
uint8_t settingsPageZeroScrollPos = 0;
uint8_t settingsPageOneScrollPos = 0;
uint8_t settingsPageTwoScrollPos = 0;
uint8_t settingsPageThreeScrollPos= 0;
uint8_t systemLogScrollPos = 0;
uint8_t setRtcTimePos = 0;
uint8_t maxMinsPage = 0;
uint8_t fansPage = 0;
uint8_t warningsPage = 0;
bool displayIsOff = false;
bool refreshPage = true;
int cursourPosition = 0;
bool refreshCalander;
unsigned long debounceTime = 200; // in millis
unsigned long lastTouchMillis = 0;
unsigned long touchStartMillis = 0; // time button has been held down
unsigned long infoDialogDisplayTime = 0;
const unsigned int RA8875_SEABLUE      = 0x1C1F;
const unsigned int RA8875_DARKGREY     = 0x8C51;
const unsigned int RA8875_LIGHTGREY    = 0xE71C;
const unsigned int RA8875_MIDGREY      = 0x8410;
const unsigned int RA8875_ORANGE       = 0xFD06;
//const unsigned int RA8875_SMOKE_WHITE  = 0xF79E;
const unsigned int RA8875_LIGHT_BLUE   = 0xA67E;
const unsigned int RA8875_LIGHT_GREEN  = 0x87F0;
const unsigned int RA8875_LIGHT_YELLOW = 0xFFF0;
const unsigned int RA8875_LIGHT_PINK   = 0xFC1F;
const unsigned int RA8875_PURPLE       = 0x981F;
}

namespace message {
// dialogs
const char* saveProfile                = "Are you sure you want to\nsave profile $? This will\noverwrite all the data\ncurrently saved to profile $";
const char* resetDevice                = "Are you sure you want to\nreset the device to its default\nstate? This will delete all\nsave profiles and system\nconfiguration settings.";
const char* setWaterHeight             = "Please make sure the tank\nis completely empty and the\nheight sensor is correctly\nmounted before continuing!";
const char* setEtapeMinReading         = "Please remove the Etape\nfrom the water and ensure\nthe Etape is not bent before\ncontinuing the calibration!";
const char* setEtapeMaxReading         = "Please place the Etape\nin water up to 32cm and\nthe Etape is not bent before\ncontinuing the calibration!";
const char* calibrateError             = "Error: the calibrate has\nfailed, please try again\nor cancel the calibration.";
const char* calibrateTds               = "Please place the TDS probe\nin $ mS/cm calibration\nsolution, and do not disturb\nthe sensor then press\ncontinue.";
const char* calibratePh                = "Please place the PH probe\nin PH $ calibration\nsolution, and do not disturb\nthe sensor then press\ncontinue.";
const char* drainingAlert              = "Alert the system is about\nto start draining and filling\nthe water reservoir.\nStarting in $ seconds\nDo you want to continue?";
const char* cancelDraining             = "The system is currently\ndraining the water reservoir\nto the minimum target $*\nPress cancel to quit draining\nat any time.";
const char* refillingAlert             = "Alert the system is about\nto start refilling the water\nreservoir.\nStarting in $ seconds\nDo you want to continue?";
const char* cancelRefilling            = "The system is currently\nfilling the water reservoir to\nthe maximum target $*\nPress cancel to quit refilling\nat any time.";
const char* cancelDosing               = "Dosing currently in progress\n* out of range by $%\nCurrently running doser #\nPress cancel to quit dosing\nat any time.";
const char* calibrateCo2               = "Please make sure the Co2\nhas been a constant 400ppm\nfor at least an hour before\nstarting the calibration\nDo you want to continue?";
const char* calibratingCo2             = "Please wait 10 seconds\nwhile the Co2 sensor is\ncalibrating. And do not\ninterfere with the Co2 while\nthe sensor is calibrating.";
const char* co2Alert                   = "Alert the system is about\nto start pumping Co2. Please\nmake sure that no one is\ninside! Starting in $ seconds\nDo you want to continue?";
const char* cancelCo2                  = "Pumping Co2 for $ minutes.\nPlease make sure that no\none is inside! Press cancel to\nquit Co2 pumping at any time.";
// pages info's
const char* ecPageInfo                 = "Set the minimum and maximum EC target. The EC will be adjusted when the EC falls below the minimum target.";
const char* tdsPageInfo                = "Set the minimum and maximum TDS target. The TDS will be adjusted when the TDS falls below the minimum target.";
const char* phPageInfo                 = "Set the minimum and maximum PH target. The PH will be adjusted when the PH goes outside the minimum or maximum targets.";
const char* co2Page1Info               = "Set the Co2 target and offset. The Co2 will be adjusted when the Co2 falls below the target minus the offset.";
const char* co2Page2Info               = "Set the dimensions of the room to control the Co2. The system will use these measurements to assist in calculating the gas duration.";
const char* co2Page3Info               = "Set the Co2 flow rate, the time to adjust the rooms Co2, and the duration in minutes to disable the extraction fans.";
const char* co2Page4Info               = "Enable or disable manual Co2 gas duration, when enabled the Co2 duration will be static, and manual Co2 gas duration can be adjusted. The Co2 adjustment can also be disabled";
const char* waterPage1Info             = "Set the minimum and maximum target water height. The water will be refilled with the water falls below the minimum target.";
const char* waterPage2Info             = "Set the minimum and maximum target water temperature. The water will be heated when the temperature falls below the minimum target.";
const char* waterPage3Info             = "Set the dimensions of the water tank. The system will use these measurements to assist in calculating the precise EC, TDS, PH dosing amount.";
const char* waterPage4Info             = "Set the days and time to refresh the water tank. The system will drain and refill the water and run the doser using the fixed refill tank mls. The drain and refill can also be disabled.";
const char* waterPage5Info             = "Set the dosing millilitres for each dosing pump. The system will run the dosers using the fixed refill tank mls after the drain and refill.";
const char* dosingPageInfo             = "Set the dosing millilitres when in incremental mode and dosing mode for each pump. EC# mode is for nutrients that does not effect the EC. You can also prime each pump.";
const char* lightPageInfo              = "Set the light on and off time, and the timer mode. The modes available are constant on, constant off, and auto. A warning will be raised if the LDR fails to detect the light.";
const char* fanPage1Info               = "Set the minimum and maximum fans speed targets for the main fan. Note, you should ensure that at the minimum fan speed, the fan still rotates sufficiently.";
const char* fanPage2Info               = "Set the minimum and maximum fans speed targets for the secondary fan. Note, you should ensure that at the minimum fan speed, the fan still rotates sufficiently.";
const char* fanPage3Info               = "Set the minimum and maximum air temperature targets. The system will use these values to control the rooms air temperature via the extraction fans."; 
const char* fanPage4Info               = "Set the minimum and maximum humidity targets. The system will use these values to control the rooms humidity via the extraction fans.";
const char* fanPage5Info               = "Set the fan modes. The air temperature control, and humidity control can be disabled. And each fan can be set to a fixed speed mode.";
const char* warningPageInfo            = "Set the error margin for each environmental reading. When the given reading is outside the target +/- the error margin, a critical error is raised.";
const char* maxminsPageInfo            = "View the environmental readings history. Note the maximum and minimum readings are shared between save profiles.";
const char* profilesPageInfo           = "Select the user profile to load. You can save and load the current settings to any of the profiles., and rename each of the profile.";
// settings info
const char* calDoserPageInfo           = "Set the PWM for each of the dosing pumps, the aim is to achive 1 milliliter per second. You can also adjust the pumps voltage on the PCB if required."; 
const char* rtcTimePageInfo            = "Set the date and time for the clock. Note the time is a 24 hour format and the date is a dd-mm-yyyy format.";
const char* displayTimeoutPageInfo     = "Set the display timeout interval in minutes, the display will be turned off when it has not been touched for the given interval.";
const char* graphIntervalPageInfo      = "Set the graph update interval in seconds or minutes. The valid range is 2 to 59 seconds, or 1 to 999 minutes.";
const char* dosingIntervalPageInfo     = "Set the dosing interval in hours. The system will adjust the waters EC, TDS or PH every dosing interval.";
const char* numDosersPageInfo          = "Set the number of dosing pumps connected to the hydro controller. The minimum number of dosing pumps is 4.";
const char* systemLosPageInfo          = "View the log history, this includes a brief description and the date and time of each environmental adjustment.";
const char* ecDosingModePageInfo       = "Set the EC/TDS dosing mode, when set to precise the system will calculate the dosing mls required to meet the target, and when set to incremental the system will dose the user defined mls.";
const char* phDosingModePageInfo       = "Set the PH dosing mode, when set to precise the system will calculate the dosing mls required to meet the target, and when set to incremental the system will dose the user defined mls.";
const char* ecTdsValuePageInfo         = "Set the value of the EC and TDS nutrients used to adjust the waters EC/TDS when EC mode is set to precise. Note this is the value of all the nutrients mixed together.";
const char* phValuePageInfo            = "Set the value of the PH down and PH up buffer solutions, these values are used to adjust the waters PH when PH mode is set to precise.";
const char* wifiSsidPageInfo           = "Set the SSID for the WiFi hotspot, note the minimum SSID length is 8 characters and 16 characters maximum.";
const char* wifiPassPageInfo           = "Set the password for the WiFi hotspot, note the minimum password length is 8 characters and 16 characters maximum.";
const char* infoMessageArray[] = {
  ecPageInfo,                    // 0
  tdsPageInfo,                   // 1
  phPageInfo,                    // 2
  co2Page1Info,                  // 3
  co2Page2Info,                  // 4
  co2Page3Info,                  // 5
  co2Page4Info,                  // 6
  waterPage1Info,                // 7
  waterPage2Info,                // 8
  waterPage3Info,                // 9
  waterPage4Info,                // 10
  waterPage5Info,                // 11
  dosingPageInfo,                // 12
  lightPageInfo,                 // 13
  fanPage1Info,                  // 14
  fanPage2Info,                  // 15
  fanPage3Info,                  // 16
  fanPage4Info,                  // 17
  fanPage5Info,                  // 18
  warningPageInfo,               // 19
  maxminsPageInfo,               // 20 
  profilesPageInfo,              // 21
  calDoserPageInfo,              // 22
  rtcTimePageInfo,               // 23
  displayTimeoutPageInfo,        // 24
  graphIntervalPageInfo,         // 25
  dosingIntervalPageInfo,        // 26
  numDosersPageInfo,             // 27
  systemLosPageInfo,             // 28
  ecDosingModePageInfo,          // 29
  phDosingModePageInfo,          // 30
  ecTdsValuePageInfo,            // 31
  phValuePageInfo,               // 32
  wifiSsidPageInfo,              // 33
  wifiPassPageInfo               // 34
};
// log messages
const char* bootNotification           = "System started";     // 0
const char* drainNotification          = "Drained water";      // 1
const char* refillNotification         = "Refilled water";     // 2
const char* co2Notification            = "Adjusted Co2";       // 3
const char* lightOnNotification        = "Light on";           // 4
const char* lightOffNotification       = "Light off";          // 5
const char* lightErrorNotification     = "Error light off";    // 6
const char* phUpNotification           = "Adjusted PH up";     // 7
const char* phDownNotification         = "Adjusted PH down";   // 8
const char* ecNotification             = "Adjusted EC";        // 9
const char* tdsNotification            = "Adjusted TDS";       // 10
const char* heaterOnNotification       = "Air heater on";      // 11
const char* heaterOffNotification      = "Air heater off";     // 12
const char* waterOnNotification        = "Water heater on";    // 13
const char* waterOffNotification       = "Water heater off";   // 14
const char* const notificationsArray[] = {
  bootNotification,
  drainNotification,
  refillNotification,
  co2Notification,
  lightOnNotification,
  lightOffNotification,
  lightErrorNotification,
  phUpNotification,
  phDownNotification,
  ecNotification,
  tdsNotification,
  heaterOnNotification,
  heaterOffNotification,
  waterOnNotification,
  waterOffNotification
};
const uint8_t maxLogs = 10;
const uint8_t maxCharsPerLog = 16;
uint8_t logTypeArray[maxLogs];
char timeStrArray[maxLogs][maxCharsPerLog];
uint8_t systemLogPos = 0;
uint8_t infoPos = 0;
}

namespace device {
const char* versionNumber = "5.0.2"; // do not adjust !
bool relayOffState = HIGH;
const uint8_t slaveAddress = 9;
int remotlyLoadUserProfile = -1;
int remotlySaveUserProfile = -1;
bool sensorsReady = false;
bool currentlyDosing = false;
bool airHeaterIsOn = false;
bool waterHeaterIsOn = false;
uint8_t currentDoserNum = 0;
// bool currentlyDraining;
uint8_t dosingTimerHourCounter = 0;
unsigned long primeTouchTime = 0;
bool doserIsPriming[6] {0,0,0,0,0,0};
uint8_t previousDosingMinute = 0, previousDosingHour = 0;
unsigned long slideShowpreviousMillis = millis();
unsigned long sensorPreviousMillis = 0;
uint8_t continueDosing = 0;
int scrollTouchCount = 0;
uint8_t phCalStage = 0;
bool newGraphData = false;
uint8_t graphArrayPos = 0;
bool ecWaitTillNextCall = false;
bool phWaitTillNextCall = false;
const uint8_t maxGraphArrayValues = 120;
float phArray[maxGraphArrayValues];
float co2Array[maxGraphArrayValues];
float ecArray[maxGraphArrayValues];
float tdsArray[maxGraphArrayValues];
float waterTemperatureArray[maxGraphArrayValues];
float waterTemperatureArrayF[maxGraphArrayValues];
float waterLevelArray[maxGraphArrayValues];
float waterLevelArrayInInches[maxGraphArrayValues];
float airTemperatureArray[maxGraphArrayValues];
float airTemperatureArrayF[maxGraphArrayValues];
float humidityArray[maxGraphArrayValues];
float fanOneSpeedArray[maxGraphArrayValues];
float fanTwoSpeedArray[maxGraphArrayValues];
uint8_t lightSwitchedOnHour = 0;
uint8_t lightSwitchedOnMin = 0;
bool lightOn = false;
int intputPosition = 0;
enum charTypes {LOWER, UPPER, SPECIAL};
charTypes charType = LOWER;
const char specialSymbols[11]{ '!', '@', '#', '&', '$', '%', '?', '/', '-', '~', '*' };
bool refreshKeys = false;
uint8_t profileInputNumber = 0;
uint8_t userProfile = 0;
int lastIntputPosition = 0;
bool lockSaveButtons = false, updateKeyboardInput = false;
unsigned long keyBoardClosedTime = millis();
float minPh = 0;
float maxPh = 0;
float minCo2 = 0;
float maxCo2 = 0;
float minEc = 0;
float maxEc = 0;
float minWaterTemp = 0;
float maxWaterTemp = 0;
float minWaterLevel = 0;
float maxWaterLevel = 0;
int minTds = 0;
int maxTds = 0;
float minAirTemp = 0;
float maxAirTemp = 0;
float minHumidity = 0;
float maxHumidity = 0;
uint8_t minFanOneSpeed = 0;
uint8_t maxFanOneSpeed = 0;
uint8_t minFanTwoSpeed = 0;
uint8_t maxFanTwoSpeed = 0;
uint8_t fanOneSpeed = 0;
uint8_t fanTwoSpeed = 0;
uint8_t ppmErrorState = 0;
uint8_t ecErrorState = 0;
uint8_t phErrorState = 0;
uint8_t co2ErrorState = 0;
uint8_t waterTempErrorState = 0;
uint8_t waterLevelErrorState = 0;
uint8_t airTempErrorState = 0;
uint8_t humidityErrorState = 0;
uint8_t globalErrorState = 0;
uint8_t co2TurnFansBackOnMinute = 0;
uint8_t co2TurnFansBackOnHour = 0;
bool co2DisabledFans = false;
bool usernameAdjusted = false;
bool settingsAdjusted = false;
const float aref = 5.0;
enum systemStates {
  NO_ADJUSTMENT,
  ADJUSTING_TDS,
  ADJUSTING_EC,
  ADJUSTING_PH,
  ADJUSTING_WATER_HEIGHT,
  ADJUSTING_CO2, 
};
uint8_t systemStatus = NO_ADJUSTMENT; // TO DO?
enum dosingDialogOption {
  NOT_SET,
  CONTINUE,
  CANCEL
};
enum etapeDialogOption {
  UNCHANGED,
  PAGE_ONE,
  PAGE_TWO
};
enum warningStates {
  NO_WARNING,
  MINOR_WARNING,
  MAJOR_WARNING
};
enum doserModes {
  DOSER_OFF,
  DOSER_EC,
  DOSER_EC_OP,
  DOSER_PH_DOWN,
  DOSER_PH_UP
};
enum conversionTypes { // to do
  EU,
  US,
  AU
}; // EC 0.64, US 0.50, AU 0.7
uint8_t conversionType = EU;
enum controlOptions {
  SPEED_IDLE,
  SPEED_DOWN,
  SPEED_UP
};
}

namespace user {
char profileOneName[16]   = "Profile_1";
char profileTwoName[16]   = "Profile_2";
char profileThreeName[16] = "Profile_3";
char profileFourName[16]  = "Profile_4";
char profileFiveName[16]  = "Profile_5";
uint8_t lightOnTimeHour = 23;
uint8_t lightOnTimeMin = 0;
uint8_t lightOffTimeHour = 22;
uint8_t lightOffTimeMin = 30;
uint8_t lightMode = 1;
uint8_t targetMinFanOneSpeed = 25;
uint8_t targetMaxFanOneSpeed = 100;
uint8_t targetMinFanTwoSpeed = 50;
uint8_t targetMaxFanTwoSpeed = 100;
float targetMinAirTemp = 20;
float targetMaxAirTemp = 27.5;
float targetMinAirTempF = 68.0; // NEW IN F
float targetMaxAirTempF = 81.5; // NEW IN F
float targetMinHumidity = 40.1;
float targetMaxHumidity = 60.2;
bool fansControlTemperature = true;
bool fansControlHumidity = false;
bool fanOneFixedSpeed = false;
bool fanTwoFixedSpeed = false;
uint8_t numberOfDosers = 6;
uint8_t doserOneMode = device::DOSER_EC;
uint8_t doserTwoMode = device::DOSER_EC;
uint8_t doserThreeMode = device::DOSER_PH_DOWN;
uint8_t doserFourMode = device::DOSER_PH_UP;
uint8_t doserFiveMode = device::DOSER_EC_OP;
uint8_t doserSixMode = device::DOSER_EC_OP;
int doserOneMills = 10;
int doserTwoMills = 10;
int doserThreeMills = 10;
int doserFourMills = 10;
int doserFiveMills = 10;
int doserSixMills = 10;
int targetMinTds = 400;
int targetMaxTds = 600;
int targetCo2 = 400;
int co2Offset = 99;
int roomWidthCm = 120;
int roomLengthCm = 120;
int roomHeightCm = 200;
float co2FlowrateLtrs = 6;
int roomWidthInches = 120;
int roomLengthInches = 120;
int roomHeightInches = 200;
float co2FlowrateFeet3 = 6;
uint8_t co2CheckTimeMinute = 0;
uint8_t co2CheckTimeHour = 22;
int disableFansTimer = 30;
bool enableManualCo2Duration = false;
int manualCo2GasDuration = 30;
bool disableCo2Control = true;
float targetMinPh = 6.0;
float targetMaxPh = 6.5;
float targetMinEc = 1.60;
float targetMaxEc = 1.70;
float targetMinWaterTemp = 20.0;
float targetMaxWaterTemp = 27.0;
float targetMinWaterTempF = 73.4;
float targetMaxWaterTempF = 77.0;
float targetMinWaterHeight = 10.1;
float targetMaxWaterHeight = 32.2;
float targetMinWaterHeightInches = 10.1;
float targetMaxWaterHeightInches = 32.2;
float waterTankLength = 50; 
float waterTankWidth = 50; 
float waterTankLengthInches = 20;
float waterTankWidthInches = 20;
int doserOneSpeed = 4096;
int doserTwoSpeed = 4096;
int doserThreeSpeed = 4096;
int doserFourSpeed = 4096;
int doserFiveSpeed = 4096;
int doserSixSpeed = 4096;
uint8_t dosingInterval = 6;
int refillDoserOneMills = 10;
int refillDoserTwoMills = 10;
int refillDoserThreeMills = 10;
int refillDoserFourMills = 10;
int refillDoserFiveMills = 10;
int refillDoserSixMills = 10;
bool autoFillDays[31];
uint8_t autoFillHour = 12;
uint8_t autoFillMinute = 0;
uint8_t drainTimeout = 1;
unsigned int backgroundColor = display::RA8875_LIGHT_BLUE;
unsigned long graphInterval = 2;
unsigned long afkTime = 1;
bool convertToF  = false;
bool convertToInches = false;
bool convertToTds = false;
int tdsErrorMargin = 50;
float ecErrorMargin = 0.5;
float phErrorMargin = 0.5;
float co2ErrorMargin = 100;
float waterTempErrorMargin = 1.0;
float waterTempErrorMarginF = 1.0;
float waterHeightErrorMargin = 5;
float waterHeightErrorMarginInches = 1;
float airTempErrorMargin = 3.0;
float airTempErrorMarginF = 2.0;
float humidityErrorMargin = 5;
bool disablePpmWarnings = true;
bool disableEcWarnings = true;
bool disableCo2Warnings = true;
bool disablePhWarnings = true;
bool disableWaterTempWarnings = true;
bool disableWaterHeightWarnings = true;
bool disableAirTempWarnings = true;
bool disableHumidityWarnings = true;
//bool disableAllWarnings = false;
bool disableLED = false;
bool disableBeeper = false;
bool disableDrainAndRefill = true;
bool disableMachineLearning = true;
bool clearWifiDetails = false;
bool resetSettings = false;
uint8_t rtcDayOfWeek = 0;
uint8_t rtcSecond = 0;
uint8_t rtcMinute = 0;
uint8_t rtcHour = 0;
uint8_t rtcDay = 0;
uint8_t rtcMonth = 0;
int rtcYear = 0;
bool useEtapeSensor = true;
enum dosingModes {
  PRECISE,
  INCREMENTAL
};
dosingModes ecDosingMode = PRECISE;
dosingModes phDosingMode = PRECISE;
}

namespace sensor {
float ec = 0;
float ph = 0;
int tds = 0;
int co2 = 0;
int ldr = 0;
float waterTemp = 0;
float airTemp = 0;
float humidity = 0;
float waterLevel = 0;
float waterLevelInches = 0;
float waterVolumeLtrs = 0;
float waterVolumeGallons = 0;
float emptyWaterTankDepth = 0;
long hcsrDuration = 0;
float ecCalibration = 1;
float phDownSolution = 4.0;
float phUpSolution = 11.0;
float ecSolution = 1.0;
float tdsSolution = 640.0;
const float phCalSolutionPart1 = 7.0;
const float phCalSolutionPart2 = 4.0;
const float ecCalSolutionPart1 = 1.413;
const float ecCalSolutionPart2 = 12.88;
float ecKvalueLow = 1.0;
float ecKvalueHigh = 1.0;
float phNeutralVoltage = 1500.0;
float phAcidicVoltage =  2032.44;
unsigned long co2GasTime = 0;
float temp_etapeZeroVolumeResistance = 0;
float etapeZeroVolumeResistance = 1877.79; // 2104.56  resistance value (in ohms) when no liquid is present.
float etapeMaxVolumeResistance = 271.4; // 288.71 resistance value (in ohms) when liquid is at max line.
const float etapeCalibrationCm = 31.00; // maximum mesurement in centimeters on etap 12" version
const float etapeOffset = 2.5; // Lowest value read on etape in Centimeters, datasheet says 2.5 but i get 2.5
const uint8_t co2Request[] {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
}

namespace wifi {
bool wifiEnabled = false;
unsigned long connectionTime = 0;
unsigned long wifiPreviousMillis = 0;
char ssid[16] = "HydroController";
char password[16] = "Password1!";
bool hiddenNetwork = false;
char webData[1024] {};
}

// Class instances
RA8875 tft = RA8875(pin::ra8875_cs, pin::ra8875_rst);
OneWire oneWire(pin::oneWireBus);
DallasTemperature dallasTemperature(&oneWire);
uRTCLib rtc(0x68); // The RTC uses hardware I2C (SCL/SDA) //DS3231 rtc(SDA, SCL);
Adafruit_HDC1000 hdc = Adafruit_HDC1000();
Adafruit_ADS1115 ads;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41); // 0x41 = 65
DFRobot_PH ph;
DFRobot_EC ec;
ExternalEEPROM externalEEPROM;
WebServer server(80);


void setup() {
  Serial.begin(115200); // debugging
  Serial2.begin(9600, SERIAL_8N1, pin::rxd2, pin::txd2); // co2 sensor
  pinMode(pin::hcsrEcho, INPUT);
  pinMode(pin::hcsrTrigger, OUTPUT);
  pinMode(pin::tdsTransistor, OUTPUT);
  pinMode(pin::phTransistor, OUTPUT);
  Wire.begin();
  initializeDevice();
}

void loop() {
  touchEvent();
  readSensors();
  //adjustScreenValuesTest();
  drawPages();
  screenSaver();
  envriomentalControl();
  if (wifi::wifiEnabled)
    server.handleClient();
  displayWarnings();
}