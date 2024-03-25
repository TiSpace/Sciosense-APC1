/*
  ScioSense_APC1.h - Library for the APC1 sensor with I2C interface from ScioSense
  2022 April 27	v1	Zorro Zhu	Created
  based on application note "APC1 Software Integration.pdf" rev 0.01
  2024 March 25 TR clean up 
*/

#include "Sciosense_APC1_I2C.h"
#include "math.h"

// **************************************************************************
// Set APC1 I²C address
// **************************************************************************
ScioSense_APC1::ScioSense_APC1(uint8_t slaveaddr) {
  this->_slaveaddr = slaveaddr;
}

// **************************************************************************
//  Init I2C communication, resets APC1 and checks its PART_ID.
//  Returns false on I2C problems or wrong PART_ID.
// **************************************************************************

  bool ScioSense_APC1::begin() {
  bool result;
  uint8_t i2cbuf[2];

  //init I2C
  _i2c_init();
  delay(APC1_RESET_DELAY);  // Wait to boot after power on

  this->_available = false;
  result = this->reset();

  this->_available = this->checkPartID();

  return this->_available;
}

// **************************************************************************
// Set working mode
// **************************************************************************
bool ScioSense_APC1::setmode(uint8_t mode) {
  bool result = this->APC1_SendCMD(APC1_CMD_MODE, 0x00, mode);

  return result;
}

// **************************************************************************
// Sends a reset to the APC1. Returns false on I2C problems.
// **************************************************************************
bool ScioSense_APC1::reset(void) {
  uint8_t result = this->APC1_SendCMD(APC1_CMD_MODE, 0x00, APC1_MODE_RESET);

  delay(APC1_RESET_DELAY);  // Wait to boot after reset

  return result == 0;
}

// **************************************************************************
//Reads the part ID and confirms valid sensor
// **************************************************************************
bool ScioSense_APC1::checkPartID(void) {
  uint8_t i2cbuf[100], i;
  uint16_t checksum = 0;
  uint8_t result = this->APC1_SendCMD(APC1_CMD_INFO, 0x00, 0x00);


  delay(10);
  result = this->read(_slaveaddr, (CMD_REG_ADDR + 7), i2cbuf, 23);
  if (result) return false;


  if ((i2cbuf[0] != 0x42) || (i2cbuf[1] != 0x4d))  //No header detected
  {
    Serial.println("CheckID no header detected!");
    return false;
  }

  Serial.println("CheckID header detected!");


  if ((i2cbuf[4] != 'A') || (i2cbuf[5] != 'P') || (i2cbuf[6] != 'C') || (i2cbuf[7] != '1') || (i2cbuf[8] != 0x2D) || (i2cbuf[9] != 'I'))  //No correct type detected
  {
    Serial.println("CheckID no correct type detected!");
    return false;
  }

  for (i = 0; i < 21; i++) {
    checksum += i2cbuf[i];
  }


  if ((i2cbuf[21] != (checksum >> 8)) || (i2cbuf[22] != (checksum & 0xFF))) {
    Serial.println("Frame checksum error!");
    return false;
  }

  Serial.print("SN: ");
  for (i = 10; i < 18; i++) {
    Serial.print(i2cbuf[i], HEX);
    
  }
  Serial.print("\n");
   return true;
}

// **************************************************************************
// Read out APC1 data
// **************************************************************************
bool ScioSense_APC1::measure(void) {
  uint8_t i2cbuf[64], i;
  uint8_t status;
  uint8_t result;
  uint16_t checksum = 0;

  result = this->read(_slaveaddr, DATA_REG_ADDR, i2cbuf, 64);

  if ((i2cbuf[0] != 0x42) || (i2cbuf[1] != 0x4d))  //No header detected
  {
    Serial.println("Read data no header detected!");
    return false;
  }

  for (i = 0; i < (APC1_DATA_LENGTH - 2); i++) {
    checksum += i2cbuf[i];
  }


  if ((i2cbuf[APC1_DATA_LENGTH - 2] != (checksum >> 8)) || (i2cbuf[APC1_DATA_LENGTH - 1] != (checksum & 0xFF))) {
    Serial.println("Read data checksum error!");
    return false;
  }

  //TVOC
  _pm1_0 = i2cbuf[4] << 8 | i2cbuf[5];
  _pm2_5 = i2cbuf[6] << 8 | i2cbuf[7];
  _pm10 = i2cbuf[8] << 8 | i2cbuf[9];
  _apm1_0 = i2cbuf[10] << 8 | i2cbuf[11];
  _apm2_5 = i2cbuf[12] << 8 | i2cbuf[13];
  _apm10 = i2cbuf[14] << 8 | i2cbuf[15];
  _count0_3um = i2cbuf[16] << 8 | i2cbuf[17];
  _count0_5um = i2cbuf[18] << 8 | i2cbuf[19];
  _count1_0um = i2cbuf[20] << 8 | i2cbuf[21];
  _count2_5um = i2cbuf[22] << 8 | i2cbuf[23];
  _count5_0um = i2cbuf[24] << 8 | i2cbuf[25];
  _count10um = i2cbuf[26] << 8 | i2cbuf[27];
  //TVOC,RHT
  _data_tvoc = i2cbuf[28] << 8 | i2cbuf[29];
  _data_eco2 = i2cbuf[30] << 8 | i2cbuf[31];
  _temperature = i2cbuf[34] << 8 | i2cbuf[35];
  _humidity = i2cbuf[36] << 8 | i2cbuf[37];
  _temperature = i2cbuf[34] << 8 | i2cbuf[35];
  _humidity = i2cbuf[36] << 8 | i2cbuf[37];
  _raw_temp = i2cbuf[38] << 8 | i2cbuf[39];
  _raw_humi = i2cbuf[40] << 8 | i2cbuf[41];

  _hp0_rs = i2cbuf[42] << 24 | i2cbuf[43] << 16 | i2cbuf[44] << 8 | i2cbuf[45];
  _hp1_rs = i2cbuf[46] << 24 | i2cbuf[47] << 16 | i2cbuf[48] << 8 | i2cbuf[49];
  _hp2_rs = i2cbuf[50] << 24 | i2cbuf[51] << 16 | i2cbuf[52] << 8 | i2cbuf[53];
  _hp3_rs = i2cbuf[54] << 24 | i2cbuf[55] << 16 | i2cbuf[56] << 8 | i2cbuf[57];
  _data_iaq = i2cbuf[58] & 0x0F;

  _data_aqi_index = (i2cbuf[58] >> 4) << 8 | i2cbuf[59];
  _fw_ver = i2cbuf[60];

  _error_code = i2cbuf[61];

  return true;
}



/****************************************************************************/
/*	General functions														*/
/****************************************************************************/

// **************************************************************************
//    I²C Initialization
// **************************************************************************

void ScioSense_APC1::_i2c_init() {
  Wire.begin();
}

// **************************************************************************
//    read 8Bit
// **************************************************************************
uint8_t ScioSense_APC1::read8(uint8_t addr, byte reg) {
  uint8_t ret;
  this->read(addr, reg, &ret, 1);

  return ret;
}


// **************************************************************************
//    read multiple bytes
// **************************************************************************
uint8_t ScioSense_APC1::read(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t num) {
  uint8_t pos = 0;
  uint8_t result = 0;

  //on arduino we need to read in 32 byte chunks
  while (pos < num) {

    uint8_t read_now = min((uint8_t)32, (uint8_t)(num - pos));


    Wire.beginTransmission((uint8_t)addr);
    Wire.write((uint8_t)reg + pos);
    result = Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)addr, read_now);

    for (int i = 0; i < read_now; i++) {
      buf[pos] = Wire.read();
      pos++;
    }
  }
  return result;
}

// **************************************************************************
//    write 8Bit
// **************************************************************************
uint8_t ScioSense_APC1::write8(uint8_t addr, byte reg, byte value) {
  uint8_t result = this->write(addr, reg, &value, 1);
  return result;
}

uint8_t ScioSense_APC1::write(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t num) {

  Wire.beginTransmission((uint8_t)addr);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t *)buf, num);
  uint8_t result = Wire.endTransmission();
  return result;
}

// **************************************************************************
//    send command
// **************************************************************************
uint8_t ScioSense_APC1::APC1_SendCMD(uint8_t CMD, uint8_t DATAH, uint8_t DATAL) {
  uint8_t buf[7] = { 0 };
  uint16_t checksum = 0;
  uint8_t result, i;

  buf[0] = 0x42;
  buf[1] = 0x4D;
  buf[2] = CMD;
  buf[3] = DATAH;
  buf[4] = DATAL;

  checksum = 0x42 + 0x4D + CMD + DATAH + DATAL;


  buf[5] = checksum >> 8;
  buf[6] = checksum & 0xFF;


  result = this->write(APC1_I2CADDR, CMD_REG_ADDR, buf, CMD_LENGTH);

  return result;
}

/**************************************************************************/
