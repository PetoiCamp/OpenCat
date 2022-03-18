//#define VOICE
char token;
char *newCmd = new char[40];
byte newCmdIdx = 0;

#ifdef VOICE
#include "voice.h"
#else

#define RANDOM_MIND //allow the robot to behave randomly
//#define GYRO_PIN  0

#endif

//board configuration
// -- comment out these blocks to save program space for your own codes --

#define IR_PIN    4   // Signal Pin of IR receiver to Arduino Digital Pin 4
// -----------------------------------------------------------------------
#define BUZZER    5
#define ULTRA_SOUND

#define SOUND A2
#define LIGHT A3


#define I2C_EEPROM //comment this line out if you don't have an I2C EEPROM in your DIY board. 

//Tutorial: https://bittle.petoi.com/11-tutorial-on-creating-new-skills
#ifdef NYBBLE
#include "InstinctNybble.h"
#elif defined BITTLE
#include "InstinctBittle.h"
//#include "InstinctBittleShortExample.h"
#endif

#define DOF 16

#ifdef BITTLE
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        55, 55, -55, -55,
                        -75, -75, -75, -75
                       };
#else
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        0, 0, 0, 0,
                        0, 0, 0, 0
                       };
#endif
int8_t rotationDirection[] = {1, -1, 1, 1,
                              1, -1, 1, -1,
                              1, -1, -1, 1,
                              -1, 1, 1, -1
                             };
int angleLimit[][2] = {
  { -120, 120}, { -30, 80}, { -120, 120}, { -120, 120},
  { -90, 60}, { -90, 60}, { -90, 90}, { -90, 90},
  { -200, 80}, { -200, 80}, { -70, 250}, { -70, 250},
  { -80, 100}, { -80, 100}, { -78, 100}, { -78, 100},
};

#ifdef NyBoard_V0_1
byte pwm_pin[] = {7, 0, 15, 8,
                  6, 1, 14, 9,
                  5, 2, 13, 10,
                  4, 3, 12, 11
                 };
#define VOLTAGE_DETECTION_PIN A0 //voltage detector
#define LOW_VOLTAGE 440
#define DEVICE_ADDRESS 0x50     //I2C Address of eeprom chip         
#define BAUD_RATE 57600

#elif defined NyBoard_V0_2
byte pwm_pin[] = {4, 3, 12, 11,
                  5, 2, 13, 10,
                  6, 1, 14, 9,
                  7, 0, 15, 8
                 };
#define VOLTAGE_DETECTION_PIN A0
#define LOW_VOLTAGE 440
#define DEVICE_ADDRESS 0x50
#define BAUD_RATE 57600

#elif defined NyBoard_V1_0
byte pwm_pin[] = {12, 11, 4, 3,
                  13, 10, 5, 2,
                  14, 9, 6, 1,
                  15, 8, 7, 0
                 };
#define VOLTAGE_DETECTION_PIN A7
#define LOW_VOLTAGE 650
#define DEVICE_ADDRESS 0x54
#define BAUD_RATE 115200
//#define NEOPIXEL_PIN 10

#elif defined NyBoard_V1_1
byte pwm_pin[] = {12, 11, 4, 3,
                  13, 10, 5, 2,
                  14, 9, 6, 1,
                  15, 8, 7, 0
                 };
#define VOLTAGE_DETECTION_PIN A7
#define LOW_VOLTAGE 650
#define DEVICE_ADDRESS 0x54
#define BAUD_RATE 115200
#define LED_PIN 10
#endif

#ifdef NEOPIXEL_PIN
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 7
#define LIT_ON 30
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

#ifdef NYBBLE
#define HEAD
#define TAIL
#define X_LEG
#define WALKING_DOF 8
#define REGULAR G41
#define KNEE G41

#elif defined BITTLE
#define HEAD
#define LL_LEG
#define WALKING_DOF 8
#define REGULAR P1S
#define KNEE P1S

#endif

//on-board EEPROM addresses
#define MELODY_NORMAL 1023 //melody will be saved at the end of the 1KB EEPROM, and is read reversely. That allows some flexibility on the melody length. 
#define MELODY_INIT 1002
#define MELODY_LOW_BATTERY 975
#define MELODY_1 964
#define PWM_PIN       0                 // 16 byte array
#define CALIB     16              // 16 byte array
#define MPUCALIB  32           // 9 int byte array 9x2 =18
#define MID_SHIFT 50         // 16 byte array
#define ROTATION_DIRECTION  66 // 16 byte array
#define ADAPT_PARAM         82          // 16 x 2 byte array
#define ANGLE_LIMIT         114        // 16*2*2 = 64   82+64 = 178
#define ZERO_POSITIONS      178        // 16*2 = 32   178+32 = 210
#define CALIBRATED_ZERO_POSITIONS 210  // 16*2 = 32   210+32 = 242
#define ANGLE2PULSE_FACTOR 242          // 16*2 = 32  
#define B_OFFSET           274
#define NUM_SKILLS         275
#define SKILLS             276        // 1 byte for skill name length, followed by the char array for skill name
// then followed by i(nstinct) on progmem, or n(ewbility) on progmem

//the actual data is stored on the I2C EEPROM.
#define INITIAL_SKILL_DATA_ADDRESS 32
//The first several bytes are reserved for testing
//the above constants from onboard EEPROM to I2C EEPROM

//token list
#define T_ABORT       'a'
#define T_BEEP        'b'
#define T_CALIBRATE   'c'
#define T_REST        'd'
#define T_GYRO        'g'
#define T_HELP        'h'
#define T_INDEXED_SIMULTANEOUS_ASC 'i'
#define T_INDEXED_SIMULTANEOUS_BIN 'I'
#define T_JOINTS      'j'
#define T_SKILL       'k'
#define T_SKILL_DATA  'K'
#define T_LISTED_BIN  'L'
#define T_MOVE        'm'
#define T_MELODY      'o'
#define T_PAUSE       'p'
#define T_RAMP        'r'
#define T_SAVE        's'
#define T_MEOW        'u'
#define T_PRINT_GYRO            'v' //print Gyro data
#define T_VERBOSELY_PRINT_GYRO  'V' //verbosely print Gyro data
#define T_WORD        'w'
#define T_XLEG        'x'
#define T_ACCELERATE  '.'
#define T_DECELERATE  ','
#define T_RANDOM_MIND 'z'

float degPerRad = 180.0 / M_PI;
float radPerDeg = M_PI / 180.0;

int8_t servoCalib[DOF] = {0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0
                         };

enum ServoModel_t {
  G41 = 0,
  P1S
};

#define SERVO_FREQ 320

ServoModel_t servoModelList[] = {
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  KNEE, KNEE, KNEE, KNEE
};

int zeroPosition[DOF] = {};
int calibratedZeroPosition[DOF] = {};

#ifdef X_LEG
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  -75,  -75,
                        -55, -55, 55, 55
                      };
#else
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  75,  75,
                        -55, -55, -55, -55
                      };
#endif
float currentAdjust[DOF] = {};

//control related variables
char lastToken;
char *lastCmd = new char[5];
int8_t* dataBuffer = new int8_t[450];
byte cmdLen;
bool checkGyro = true;
bool printGyro = false;
bool autoSwitch = false;//true;
bool walkingQ = false;
byte exceptions = 0;

int frame;
byte tStep = 0;

#ifdef DEVELOPER
#include "MemoryFree/MemoryFree.h" //http://playground.arduino.cc/Code/AvailableMemory
#endif
#include <EEPROM.h>
#include "sound.h"

#if defined IR_PIN && defined MAIN_SKETCH
#include "infrared.h"
#endif

#include "io.h"

#ifdef RANDOM_MIND
#include "randomMind.h"
#endif

#ifdef GYRO_PIN
#include "imu.h"
#endif

#include "PCA9685servo.h"
#include "motion.h"
#include "skill.h"
#include "reaction.h"
