/*######################################################################################################################
*
*   Sciosense ACP1 via I²C
*
*   Note: APC1 requires VCC=5V while bus high voltage level must not increase 3.3V
*
*   V1.0:  TR 2024-03-25  basic example 
*   this version allows the usage of sleep mode; therefore definition of APC1_NOSLEEP needs to be set accordingly
*
* ######################################################################################################################
*/


#include <Arduino.h>
#include <Wire.h>
#include "Sciosense_APC1_I2C.h"

// declarations
#define APC1_NOSLEEP         // comment or uncomment in case you want to use/not to use APC1 sleepmode
#define sensorAPC1_sleep 100  // sleep time [sec]
#define sensorAPC1_active 10  //active time between sleep [sec]

ScioSense_APC1 apc1(APC1_I2CADDR);


/* ================================================================================
            Setup
   ================================================================================
*/
void setup() {

  Serial.begin(115200);
  while (!Serial) {};  //wait until serial monitor is ready
  Serial.print("\n\n");

 // Wire.pins(0, 2);  //make sure the I²C pinout fits to your board  -> for ESP-01 special connection


  apc1.begin();
  apc1.setmode(APC1_MODE_WORK);

  delay(2000);  // allow the PM sensor >1sec to provide valid data
  apc1.measure();

  if (apc1.available()) {
    Serial.println("APC1 available");
  } else {
    Serial.println("Module not found");
  }

// report FW
  Serial.print("Firmware version ");
  Serial.println(apc1.getFW());



#ifndef APC1_NOSLEEP
  apc1.setmode(APC1_MODE_SLEEP);  // go to idle mode (no measurements, fann off, 9mA)
  Serial.println("Sleepmode activated");
#endif

  print_header();  // print the header for all used readings
}


/* ================================================================================
            Loop
   ================================================================================
*/
void loop() {

  static long timeLastWakeUp;
  static uint8_t lastWorkStatus;

#ifndef APC1_NOSLEEP    // process with sleep mode
  if ((timeLastWakeUp + (sensorAPC1_sleep * 1000U)) < millis()) {
    if (lastWorkStatus != APC1_MODE_WORK) {
      apc1.setmode(APC1_MODE_WORK);
      lastWorkStatus = APC1_MODE_WORK;
      Serial.println("APC1 working");
      delay(1000);  //requires one second to read
    }
    getAPC1measurments();

    //run for a given time
    if ((timeLastWakeUp + (sensorAPC1_sleep * 1000U) + (sensorAPC1_active * 1000)) < millis()) {
      timeLastWakeUp = millis();
      apc1.setmode(APC1_MODE_SLEEP);  // go to idle mode (no measurements, fann off, 9mA)
      Serial.println("APC1 went to SLEEP");
      lastWorkStatus = APC1_MODE_SLEEP;
    }
  }
#else
  getAPC1measurments();   // measure without sleep periode
#endif


  delay(2000);
}

/* ================================================================================
            print measurements to serial monitor
   ================================================================================
*/
void getAPC1measurments() {

  apc1.measure();

  Serial.print(apc1.getPM1_0());
  Serial.print('\t');
  Serial.print(apc1.getPM2_5());
  Serial.print('\t');
  Serial.print(apc1.getPM10());
  Serial.print('\t');
  Serial.print(apc1.getRawTemp() / 10.0);
  Serial.print('\t');
  Serial.print(apc1.getRawHumi() / 10.0);
  Serial.print('\t');
  Serial.print(computeAbsHumidity(apc1.getRawTemp() / 10, apc1.getRawHumi() / 10));
  Serial.print('\t');
  Serial.print(apc1.geteCO2());
  Serial.print('\t');
  Serial.print(apc1.getHP0());
  Serial.print('\t');
  Serial.print(apc1.getHP3());
  Serial.print('\t');
  Serial.print(apc1.getTVOC());
  Serial.print('\t');
  Serial.print(apc1.getIAQ());
  Serial.print('\t');
  Serial.print(apc1.getAQI_INDEX());
  Serial.print('\t');

  Serial.print(apc1.getCOUNT0_3());
  Serial.print('\t');
  Serial.print(apc1.getCOUNT0_5());
  Serial.print('\t');
  Serial.print(apc1.getCOUNT1_0());
  Serial.print('\t');
  Serial.print(apc1.getCOUNT2_5());

  Serial.print('\t');
  Serial.print(apc1.getCOUNT10());
  Serial.print('\n');
}

/* ================================================================================
            print measurements header to serial monitor
   ================================================================================
*/
void print_header() {
  Serial.print("PM1.0");
  Serial.print('\t');
  Serial.print("PM2.5");
  Serial.print('\t');
  Serial.print("PM10");
  Serial.print('\t');
  Serial.print("T_raw");
  Serial.print('\t');
  Serial.print("rH_raw");
  Serial.print('\t');
  Serial.print("abs H");
  Serial.print('\t');

  Serial.print("eCO2");
  Serial.print('\t');
  Serial.print("R0");
  Serial.print('\t');
  Serial.print("R3");
  Serial.print('\t');

  Serial.print("TVOC");
  Serial.print('\t');

  Serial.print("IAQ");
  Serial.print('\t');
  Serial.print("AQI");
  Serial.print('\t');

  Serial.print("#PM0.3");
  Serial.print('\t');
  Serial.print("#PM0.5");
  Serial.print('\t');
  Serial.print("#PM1");
  Serial.print('\t');
  Serial.print("#PM2.5");
  Serial.print('\t');
  Serial.print("#PM10");
  Serial.print('\t');




  Serial.println();
}




/***********************************************************************
FUNCTION CALCULATE ABS. HUMIDITY
***********************************************************************/
//reference https://www.wetterochs.de/wetter/feuchte.html
double computeAbsHumidity(double celsius, double humidity) {

  double abs_humidity = 6.112 * exp((17.67 * celsius) / (celsius + 243.5)) * 2.1674 * humidity / (273.15 + celsius);
  return abs_humidity;
}
