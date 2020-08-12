/**
 * Constants for board pins and servo configurations
 */
 
 //board configuration
#define INTERRUPT 0
#define BUZZER 5
#define GYRO
//#define ULTRA_SOUND
#ifdef ULTRA_SOUND
#define VCC 8
#define TRIGGER 9
#define ECHO 10
#define LONGEST_DISTANCE 200 // 200 cm = 2 meters
float farTime =  LONGEST_DISTANCE * 2 / 0.034;
#endif

//board pins
//L - left, R - right, H - hind, F - front, S - shoulder, P - paw, 
//head, neck, LFS, LFP, RFT, RFP, LHS, LHP, tail
byte pins[] = {7, 0, 8, 15,
               6, 1, 14, 9,
               5, 2, 13, 10,
               4, 3, 12, 11
              };
#define HEAD
#define TAIL
#define X_LEG
#define WALKING_DOF 8

#define DEVICE_ADDRESS 0x50    //I2C Address of AT24C32D eeprom chip
#define WIRE_BUFFER 30 //Arduino wire allows 32 byte buffer, with 2 byte for address.
#define WIRE_LIMIT 16 //That leaves 30 bytes for data. use 16 to balance each writes
#define PAGE_LIMIT 32 //AT24C32D 32-byte Page Write Mode. Partial Page Writes Allowed
#define EEPROM_SIZE (65536/8)
#define SKILL_HEADER 3

//on-board EEPROM addresses
#define MELODY 1023 //melody will be saved at the end of the 1KB EEPROM, and is read reversely. That allows some flexibility on the melody length. 
#define PIN 0                 // 16 byte array
#define CALIB 16              // 16 byte array
#define MID_SHIFT 32          // 16 byte array
#define ROTATION_DIRECTION 48 // 16 byte array
#define SERVO_ANGLE_RANGE 64  // 16 byte array
#define MPUCALIB 80           // 16 byte array
#define FAST 96               // 16 byte array
#define SLOW 112              // 16 byte array
#define LEFT 128              // 16 byte array
#define RIGHT 144             // 16 byte array

#define ADAPT_PARAM 160          // 16 x NUM_ADAPT_PARAM byte array
#define NUM_ADAPT_PARAM  2    // number of parameters for adaption
#define SKILLS 200         // 1 byte for skill name length, followed by the char array for skill name
// then followed by i(nstinct) on progmem, or n(ewbility) on progmem


//servo constants
//Pulse range = maximum pulse width - minimum pulse width
//Pulse width per degree = pulse range / 181
//For a specified angle, the pulse width = minimum pulse width + (angle * pulse width per degree)
//duty cycle = pulse width / period
//http://developer.wildernesslabs.co/Hardware/Reference/Peripherals/Servos/
//for sg90, min pulse width is 1ms (0 deg), max pulse width is 2ms (180 deg)
#define DOF 16
#define SG90_MIN 200
#define SG90_MAX 577
#define SG90_RANGE 377
#define PWM_FREQUENCY 50 //TODO: change if problematic, 50Hz is avg servo value

#define SERVOMIN SG90_MIN
#define SERVOMAX SG90_MAX
#define PWM_RANGE (SERVOMAX - SERVOMIN)

byte servoAngleRanges[] =  {SG90_RANGE, SG90_RANGE, SG90_RANGE, SG90_RANGE,
                            SG90_RANGE, SG90_RANGE, SG90_RANGE, SG90_RANGE,
                            SG90_RANGE, SG90_RANGE, SG90_RANGE, SG90_RANGE,
                            SG90_RANGE, SG90_RANGE, SG90_RANGE, SG90_RANGE
                           };
int8_t calibs[] = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int8_t middleShifts[] = {0, 15, 0, 0,
                         -45, -45, -45, -45,
                         0, 0, 0, 0,
                         0, 0, 0, 0
                        };
int8_t rotationDirections[] = {1, -1, 1, 1,
                               1, -1, 1, -1,
                               1, -1, -1, 1,
                               -1, 1, 1, -1
                              };
float pulsePerDegree[DOF] = {};
int8_t servoCalibs[DOF] = {};
char currentAng[DOF] = {};
int calibratedDuty0[DOF] = {};

// balancing parameters
#define ROLL_LEVEL_TOLERANCE 2//the body is still considered as level, no angle adjustment
#define PITCH_LEVEL_TOLERANCE 1
float levelTolerance[2] = {ROLL_LEVEL_TOLERANCE * radPerDeg, PITCH_LEVEL_TOLERANCE * radPerDeg}; //the body is still considered as level, no angle adjustment
#define LARGE_PITCH 75
//the following coefficients will be divided by 10.0 in the adjust() function. so (float) 0.1 can be saved as (int8_t) 1
//this trick allows using int8_t array insead of float array, saving 96 bytes and allows storage on EEPROM
#define panF 60
#define tiltF 60
#define sRF 50   //shoulder roll factor
#define sPF 12 //shoulder pitch factor
#define uRF 30 //upper leg roll factor
#define uPF 30 //upper leg pitch factor
#define lRF (-1.5*uRF) //lower leg roll factor 
#define lPF (-1.5*uPF)//lower leg pitch factor
#define LEFT_RIGHT_FACTOR 2
#define FRONT_BACK_FACTOR 2
#define POSTURE_WALKING_FACTOR 0.5
#ifdef POSTURE_WALKING_FACTOR
float postureOrWalkingFactor;
#endif

int8_t adaptiveParameterArray[16][NUM_ADAPT_PARAM] = {
  { -panF, 0}, { -panF / 2, -tiltF}, { -2 * panF, 0}, {0, 0},
  {sRF, -sPF}, { -sRF, -sPF}, { -sRF, sPF}, {sRF, sPF},
  {uRF, uPF}, {uRF, uPF}, {uRF, uPF}, {uRF, uPF},
  {lRF, lPF}, {lRF, lPF}, {lRF, lPF}, {lRF, lPF}
};

//numerical constants
#define M_PI 3.1415926535
