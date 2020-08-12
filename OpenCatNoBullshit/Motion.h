/**
 * Motion class
 * loading and lookup functionality for movement
 */
#include "Constants.h"
#include "Util.h"
#include <Wire.h>


class Motion {
  public:
    byte pins[DOF];
    uint8_t period;
    float expectedRollPitch[2];
    char* dutyAngles;
    Motion() {
      period = 0;
      expectedRollPitch[0] = 0;
      expectedRollPitch[1] = 0;
      dutyAngles = NULL;
    }

    int lookupAddressByName(char* skillName) {
      int skillAddressShift = 0;
      for (byte s = 0; s < NUM_SKILLS; s++) {//save skill info to on-board EEPROM, load skills to SkillList
        byte nameLen = EEPROM.read(SKILLS + skillAddressShift++);
        char* readName = new char[nameLen + 1];
        for (byte l = 0; l < nameLen; l++) {
          readName[l] = EEPROM.read(SKILLS + skillAddressShift++);
        }
        readName[nameLen] = '\0';
        if (!strcmp(readName, skillName)) {
          delete[]readName;
          return SKILLS + skillAddressShift;
        }
        delete[]readName;
        skillAddressShift += 3;//1 byte type, 1 int address
      }
      PTLF("wrong key!");
      return -1;
    }
    void loadDataFromProgmem(unsigned int pgmAddress) {
      period = pgm_read_byte(pgmAddress);//automatically cast to char*
      for (int i = 0; i < 2; i++)
        expectedRollPitch[i] = radPerDeg * (int8_t)pgm_read_byte(pgmAddress + 1 + i);
      byte frameSize = period > 1 ? WALKING_DOF : 16;
      int len = period * frameSize;
      //delete []dutyAngles; //check here
      dutyAngles = new char[len];
      for (int k = 0; k < len; k++) {
        dutyAngles[k] = pgm_read_byte(pgmAddress + SKILL_HEADER + k);
      }
    }
    void loadDataFromI2cEeprom(unsigned int &eeAddress) {
      Wire.beginTransmission(DEVICE_ADDRESS);
      Wire.write((int)((eeAddress) >> 8));   // MSB
      Wire.write((int)((eeAddress) & 0xFF)); // LSB
      Wire.endTransmission();
      Wire.requestFrom(DEVICE_ADDRESS, 3);
      period = Wire.read();
      //PTL("read " + String(period) + " frames");
      for (int i = 0; i < 2; i++)
        expectedRollPitch[i] = radPerDeg * (int8_t)Wire.read();
      byte frameSize = period > 1 ? WALKING_DOF : 16;
      int len = period * frameSize;
      //delete []dutyAngles;//check here
      dutyAngles = new char[len];

      int readFromEE = 0;
      int readToWire = 0;
      while (len > 0) {
        //PTL("request " + String(min(WIRE_BUFFER, len)));
        Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
        readToWire = 0;
        do {
          if (Wire.available()) dutyAngles[readFromEE++] = Wire.read();
          /*PT( (int8_t)dutyAngles[readFromEE - 1]);
            PT('\t')*/
        } while (--len > 0 && ++readToWire < WIRE_BUFFER);
        //PTL();
      }
      //PTLF("finish reading");
    }

    void loadDataByOnboardEepromAddress(int onBoardEepromAddress) {
      char skillType = EEPROM.read(onBoardEepromAddress);
      unsigned int dataArrayAddress = EEPROMReadInt(onBoardEepromAddress + 1);
      delete[] dutyAngles;

      PTF("free memory: ");
      PTL(freeMemory());
      
#ifdef I2C_EEPROM
      if (skillType == 'I') { //copy instinct data array from external i2c eeprom
        loadDataFromI2cEeprom(dataArrayAddress);
      }
      else                    //copy newbility data array from progmem
#endif
      {
        loadDataFromProgmem(dataArrayAddress) ;
      }
#ifdef DEVELOPER
      PTF("free memory: ");
      PTL(freeMemory());
#endif
    }

    void loadBySkillName(char* skillName) {//get lookup information from on-board EEPROM and read the data array from storage
      int onBoardEepromAddress = lookupAddressByName(skillName);
      if (onBoardEepromAddress == -1)
        return;
      loadDataByOnboardEepromAddress(onBoardEepromAddress);
    }

    void info() {
      PTL("period: " + String(period) + ",\tdelayBetweenFrames: " + ",\texpected (pitch,roll): (" + expectedRollPitch[0]*degPerRad + "," + expectedRollPitch[1]*degPerRad + ")");
      for (int k = 0; k < period * (period > 1 ? WALKING_DOF : 16); k++) {
        PT(String((int8_t)dutyAngles[k]) + ", ");
      }
      PTL();
    }
};
