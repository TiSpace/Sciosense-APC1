/*
  ScioSense_APC1.h - Library for the APC1 sensor with I2C interface from ScioSense
  2020 Apr 06	v2	Christoph Friese	Changed nomenclature to ScioSense as product shifted from ams
  2020 Feb 15	v2	Giuseppe Pasetti	Corrected firmware flash option
  2019 May 05	v1	Christoph Friese	Created
  based on application note "APC1 Software Integration.pdf" rev 0.01
*/

#ifndef __SCIOSENSE_APC1_H_
#define __SCIOSENSE_APC1_H_

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>


// Chip constants
#define APC1_PARTID 0x6001

// 7-bit I2C slave address of the APC1
#define APC1_I2CADDR 0x12  //0x12

#define APC1_RESET_DELAY 2000  //Delay after software reset
#define CMD_REG_ADDR 0x40      //Default command address
#define CMD_LENGTH 7           //Default command length

#define DATA_REG_ADDR 0x00     //Default start address for data
#define APC1_DATA_LENGTH 0x40  //Frame size

#define APC1_CMD_MODE 0xE4       //Change mode command
#define APC1_MODE_RESET 0x0F     //Reset command
#define APC1_MODE_SLEEP 0x00     //Sleep mode
#define APC1_MODE_WORK 0x01      //Work mode               ********** is this correct, shoulnd't this be 0x00??
#define APC1_MODE_LOWPOWER 0x02  //Low power mode

#define APC1_CMD_INFO 0xE9  //Read out module info including type, ID, Version, etc.

class ScioSense_APC1 {

public:
  ScioSense_APC1(uint8_t slaveaddr = APC1_I2CADDR);  // Constructor using slave address

  bool begin();
  bool setmode(uint8_t mode);
  bool available() {
    return this->_available;
  }

  bool measure(void);  // perfrom measurement and stores result in internal variables

  uint8_t getMajorRev() {
    return this->_fw_ver_major;
  }
  uint8_t getMinorRev() {
    return this->_fw_ver_minor;
  }
  uint8_t getFW() {
    return this->_fw_ver;
  }
  //PM Sensor related
  uint16_t getPM1_0() {
    return this->_pm1_0;
  }  // PM1.0 concentration unit μ g/m3
  uint16_t getPM2_5() {
    return this->_pm2_5;
  }  // PM2.5 concentration unit μ g/m3
  uint16_t getPM10() {
    return this->_pm10;
  }  // PM10 concentration unit  μ g/m3
  uint16_t getAPM1_0() {
    return this->_apm1_0;
  }  // PM1.0 concentration unit μ g/m3 under atmospheric environment
  uint16_t getAPM2_5() {
    return this->_apm2_5;
  }  // PM2.5 concentration unit μ g/m3 under atmospheric environment
  uint16_t getAPM10() {
    return this->_apm10;
  }  // PM10 concentration unit μ g/m3 under atmospheric environment
  uint16_t getCOUNT0_3() {
    return this->_count0_3um;
  }  // Number of particles with diameter beyond  0.3 um in  0.1 L of air
  uint16_t getCOUNT0_5() {
    return this->_count0_5um;
  }  // Number of particles with diameter beyond  0.5 um in  0.1 L of air
  uint16_t getCOUNT1_0() {
    return this->_count1_0um;
  }  // Number of particles with diameter beyond  1.0 um in  0.1 L of air
  uint16_t getCOUNT2_5() {
    return this->_count2_5um;
  }  // Number of particles with diameter beyond  2.5 um in  0.1 L of air
  uint16_t getCOUNT5_0() {
    return this->_count5_0um;
  }  // Number of particles with diameter beyond  5.0 um in  0.1 L of air
  uint16_t getCOUNT10() {
    return this->_count10um;
  }  // Number of particles with diameter beyond  10 um in  0.1 L of air

  //RH+T sensor related
  int16_t getTemp() {
    return this->_temperature;
  }  // Compensated Temperature
  uint16_t getHumi() {
    return this->_humidity;
  }  // Compensated Humidity
  int16_t getRawTemp() {
    return this->_raw_temp;
  }  // Raw Temperature
  uint16_t getRawHumi() {
    return this->_raw_humi;
  }  // Raw Humidity

  //VOC related
  uint8_t getIAQ() {
    return this->_data_iaq;
  }  //UBA AQI
  uint16_t getTVOC() {
    return this->_data_tvoc;
  }
  uint16_t geteCO2() {
    return this->_data_eco2;
  }
  uint16_t getAQI_INDEX() {
    return this->_data_aqi_index;
  }  //AQI500 Index
  uint32_t getHP0() {
    return this->_hp0_rs;
  }
  uint32_t getHP1() {
    return this->_hp1_rs;
  }
  uint32_t getHP2() {
    return this->_hp2_rs;
  }
  uint32_t getHP3() {
    return this->_hp3_rs;
  }
  uint32_t getHP0BL() {
    return this->_hp0_bl;
  }
  uint32_t getHP1BL() {
    return this->_hp1_bl;
  }
  uint32_t getHP2BL() {
    return this->_hp2_bl;
  }
  uint32_t getHP3BL() {
    return this->_hp3_bl;
  }
  uint8_t getMISR() {
    return this->_misr;
  }
  bool checkPartID();
  //		bool				getFirmware();
  bool flash(const uint8_t *app_img, int size);  // Flashes the firmware of the CCS811 with size bytes from image - image _must_ be in PROGMEM
  bool reset(void);                              // Sends a reset to the APC1. Returns false on I2C problems.
  //bool EraseBaseline(void);

  uint8_t _fw_ver;

  uint8_t _fw_ver_major;
  uint8_t _fw_ver_minor;
  uint8_t _error_code;

private:
  bool debugAPC1 = false;

  bool _available = false;  // APC1 available

  uint16_t _pm1_0;
  uint16_t _pm2_5;
  uint16_t _pm10;
  uint16_t _apm1_0;
  uint16_t _apm2_5;
  uint16_t _apm10;
  uint16_t _count0_3um;
  uint16_t _count0_5um;
  uint16_t _count1_0um;
  uint16_t _count2_5um;
  uint16_t _count5_0um;
  uint16_t _count10um;

  int16_t _temperature;
  uint16_t _humidity;

  int16_t _raw_temp;
  uint16_t _raw_humi;

  uint8_t _data_iaq;
  uint16_t _data_tvoc;
  uint16_t _data_eco2;
  uint16_t _data_aqi_index;
  uint32_t _hp0_rs;
  uint32_t _hp0_bl;
  uint32_t _hp1_rs;
  uint32_t _hp1_bl;
  uint32_t _hp2_rs;
  uint32_t _hp2_bl;
  uint32_t _hp3_rs;
  uint32_t _hp3_bl;
  uint16_t _temp;
  int _slaveaddr;  // Slave address of the APC1
  uint8_t _misr;

  /****************************************************************************/
  /*	General functions														*/
  /****************************************************************************/
  void _i2c_init();

  uint8_t read8(uint8_t addr, byte reg);
  uint8_t read(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t num);

  uint8_t write8(uint8_t addr, byte reg, byte value);
  uint8_t write(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t num);

  uint8_t APC1_SendCMD(uint8_t CMD, uint8_t DATAH, uint8_t DATAL);
};


#endif
