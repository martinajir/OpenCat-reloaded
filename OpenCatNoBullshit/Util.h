/**
 * Help functions
 * abbreviations, conversions, EEPROM functionality
 */
#include "Constants.h"
#include <Wire.h>

//abbreviations
#define PT(s) Serial.print(s)  //makes life easier
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))//trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))

//conversions
 float convertRadToDeg(float rad) {
    return rad * 180 / M_PI;
  }

  float convertDegToRad(float deg) {
    return deg * M_PI / 180;
  }

//EEPROM functions
bool EEPROMOverflow = false;
  //This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.update(p_address, lowByte);
  EEPROM.update(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

//This function returns true if trying to access overflowing address
bool isEEPROMOverflowing(unsigned int &eeAddress) {
  if (eeAddress == EEPROM_SIZE) {
    PTL();
    PTL("I2C EEPROM overflow! You must reduce the size of your instincts file!\n");
    EEPROMOverflow = true;
    return true;
    }
    return false;
  }

void copyDataFromPgmToEeprom(unsigned int &eeAddress, unsigned int pgmAddress) {
  uint8_t period = pgm_read_byte(pgmAddress);//automatically cast to char*
  byte frameSize = period > 1 ? WALKING_DOF : 16;
  int len = period * frameSize + SKILL_HEADER;
  int writtenToEE = 0;
  while (len > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((eeAddress) >> 8));   // MSB
    Wire.write((int)((eeAddress) & 0xFF)); // LSB
   
    byte writtenToWire = 0;
    while ((--len > 0 ) && (eeAddress  % PAGE_LIMIT ) && (writtenToWire < WIRE_LIMIT)) { //be careful with the chained conditions 
   
      if (isEEPROMOverflowing(eeAddress)) {
        return;
      }
      
      Wire.write((byte)pgm_read_byte(pgmAddress + writtenToEE++));
      writtenToWire++;
      eeAddress++;
  
    } 
    //self-increment may not work as expected
    Wire.endTransmission();
    delay(6);  // needs 5ms for page write
    //PTL("\nwrote " + String(writtenToWire) + " bytes.");
  }
  //PTLF("finish copying to I2C EEPROM");
}
