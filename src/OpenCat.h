//board configuration
// -- comment out these blocks to save program space for your own codes --

#define IR_PIN 4  // Signal Pin of IR receiver to Arduino Digital Pin 4
// -----------------------------------------------------------------------
#define BUZZER 5

#define SOUND A2
#define LIGHT A3

//#define SERVO_SLOW_BOOT

#define I2C_EEPROM  //comment this line out if you don't have an I2C EEPROM in your DIY board.

//Tutorial: https://bittle.petoi.com/11-tutorial-on-creating-new-skills
#ifdef NYBBLE
#include "InstinctNybble.h"
#elif defined BITTLE
#include "InstinctBittle.h"
//#include "InstinctBittleShortExample.h"
#endif

#define DOF 16

#ifdef NYBBLE
int8_t middleShift[] = { 0, 15, 0, 0,
                         -45, -45, -45, -45,
                         10, 10, -10, -10,
                         -30, -30, 30, 30 };
int angleLimit[][2] = {
  { -120, 120 },
  { -80, 90 },
  { -120, 120 },
  { -120, 120 },
  { -90, 60 },
  { -90, 60 },
  { -90, 90 },
  { -90, 90 },
  { -100, 80 },
  { -100, 80 },
  { -80, 100 },
  { -80, 100 },
  { -60, 120 },
  { -60, 120 },
  { -120, 60 },
  { -120, 60 },
};
#elif defined BITTLE
int8_t middleShift[] = { 0, 15, 0, 0,
                         -45, -45, -45, -45,
                         55, 55, -55, -55,
                         -55, -55, -55, -55 };
int angleLimit[][2] = {
  { -120, 120 },
  { -30, 80 },
  { -120, 120 },
  { -120, 120 },
  { -90, 60 },
  { -90, 60 },
  { -90, 90 },
  { -90, 90 },
  { -200, 80 },
  { -200, 80 },
  { -80, 200 },
  { -80, 200 },
  { -80, 200 },
  { -80, 200 },
  { -80, 200 },
  { -80, 200 },
};
#else
int8_t middleShift[] = { 0, 15, 0, 0,
                         -45, -45, -45, -45,
                         55, 55, -55, -55,
                         0, 0, 0, 0 };
int angleLimit[][2] = {
  { -90, 90 },
  { -80, 65 },
  { -90, 90 },
  { -30, 30 },
  { -90, 60 },
  { -90, 60 },
  { -90, 90 },
  { -90, 90 },
  { -200, 90 },
  { -200, 90 },
  { -90, 210 },
  { -90, 210 },
  { -65, 100 },
  { -65, 100 },
  { -70, 65 },
  { -70, 65 },
};
#endif
int8_t rotationDirection[] = { 1, -1, 1, 1,
                               1, -1, 1, -1,
                               1, -1, -1, 1,
                               -1, 1, 1, -1 };


#ifdef NyBoard_V0_1
byte pwm_pin[] = { 7, 0, 15, 8,
                   6, 1, 14, 9,
                   5, 2, 13, 10,
                   4, 3, 12, 11 };
#define VOLTAGE_DETECTION_PIN A0  //voltage detector
#define LOW_VOLTAGE 440
#define DEVICE_ADDRESS 0x50  //I2C Address of eeprom chip
#define BAUD_RATE 57600

#elif defined NyBoard_V0_2
byte pwm_pin[] = { 4, 3, 12, 11,
                   5, 2, 13, 10,
                   6, 1, 14, 9,
                   7, 0, 15, 8 };
#define VOLTAGE_DETECTION_PIN A0
#define LOW_VOLTAGE 440
#define DEVICE_ADDRESS 0x50
#define BAUD_RATE 57600

#elif defined NyBoard_V1_0
byte pwm_pin[] = { 12, 11, 4, 3,
                   13, 10, 5, 2,
                   14, 9, 6, 1,
                   15, 8, 7, 0 };
#define VOLTAGE_DETECTION_PIN A7
#define LOW_VOLTAGE 650
#define DEVICE_ADDRESS 0x54
#define BAUD_RATE 115200
#define NEOPIXEL_PIN 10  //the code for NeoPixels have to be shrinked to fit in the board
#define NUMPIXELS 7

#elif defined NyBoard_V1_1
byte pwm_pin[] = { 12, 11, 4, 3,
                   13, 10, 5, 2,
                   14, 9, 6, 1,
                   15, 8, 7, 0 };
#define VOLTAGE_DETECTION_PIN A7
#define LOW_VOLTAGE 650
#define DEVICE_ADDRESS 0x54
#define BAUD_RATE 115200
#define LED_PIN 10

#elif defined NyBoard_V1_2
byte pwm_pin[] = { 12, 11, 4, 3,
                   13, 10, 5, 2,
                   14, 9, 6, 1,
                   15, 8, 7, 0 };
#define VOLTAGE_DETECTION_PIN A7
#define LOW_VOLTAGE 650
#define DEVICE_ADDRESS 0x54
#define BAUD_RATE 115200
#define LED_PIN 3
#define NEOPIXEL_PIN 10  //the code for NeoPixels have to be shrinked to fit in the board
#define NUMPIXELS 1
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
#define MELODY_NORMAL 1023  //melody will be saved at the end of the 1KB EEPROM, and is read reversely. That allows some flexibility on the melody length.
#define MELODY_INIT 1002
#define MELODY_LOW_BATTERY 977
#define MELODY_1 966
#define PWM_PIN 0                     // 16 byte array
#define CALIB 16                      // 16 byte array
#define MID_SHIFT 32                  // 16 byte array
#define ROTATION_DIRECTION 48         // 16 byte array
#define ZERO_POSITIONS 64             // 16*2 = 32   64+32 = 96
#define CALIBRATED_ZERO_POSITIONS 96  // 16*2 = 32   96+32 = 128
#define ADAPT_PARAM 128               // 16 x 2 byte array 128+32 = 160
#define ANGLE2PULSE_FACTOR 160        // 16*2 = 32  160+32 = 192
#define ANGLE_LIMIT 192               // 16*2*2 = 64   192+64 = 256
#define MPUCALIB 256                  // 9 int byte array 9x2 =18  256+18=274
#define B_OFFSET 274                  // 1 bytes
#define PCA9685_FREQ 275              // 2 bytes
#define NUM_SKILLS 277                // 1 bytes
#define SERIAL_BUFF 278               // 2 bytes
#define SERIAL_BUFF_RAND 280          // 2 bytes
#define SKILLS 320                    // 1 byte for skill name length, followed by the char array for skill name
// then followed by i(nstinct) on progmem, or n(ewbility) on progmem

//the actual data is stored on the I2C EEPROM.
#define INITIAL_SKILL_DATA_ADDRESS 32
//The first several bytes are reserved for testing
//the above constants from onboard EEPROM to I2C EEPROM

//token list
#define T_ABORT 'a'
#define T_BEEP 'b'
#define T_CALIBRATE 'c'
#define T_REST 'd'
#define T_GYRO 'g'
#define T_HELP 'h'
#define T_INDEXED_SIMULTANEOUS_ASC 'i'
#define T_JOINTS 'j'
#define T_SKILL 'k'
#define T_INDEXED_SEQUENTIAL_ASC 'm'
#define T_MELODY 'o'
#define T_PAUSE 'p'
#define T_TASK_QUEUE 'q'
#define T_RAMP 'r'
#define T_SAVE 's'
#define T_SOUND 'S'
#define T_TILT 't'
#define T_MEOW 'u'
#define T_PRINT_GYRO 'v'  //print Gyro data
//#define T_VERBOSELY_PRINT_GYRO  'V' //verbosely print Gyro data
//#define T_WORD        'w'
//#define T_XLEG        'x'
//#define T_ACCELERATE  '.'
//#define T_DECELERATE  ','
#define T_RANDOM_MIND 'z'

#define T_COLOR 'C'
#define T_INDEXED_SIMULTANEOUS_BIN 'I'
#define T_INDEXED_SEQUENTIAL_BIN 'M'

#define BINARY_COMMAND  //disable the binary commands to save space for the simple random demo

#ifdef BINARY_COMMAND
#define T_BEEP_BIN 'B'
#define T_SKILL_DATA 'K'
#define T_LISTED_BIN 'L'
#define T_SERVO_MICROSECOND 'W'  //PWM width modulation
#define T_TEMP 'T'               //call the last skill data received from the serial port
#endif

float degPerRad = 180.0 / M_PI;
float radPerDeg = M_PI / 180.0;

int8_t servoCalib[DOF] = { 0, 0, 0, 0,
                           0, 0, 0, 0,
                           0, 0, 0, 0,
                           0, 0, 0, 0 };
#ifndef MAIN_SKETCH
enum ServoModel_t {
  G41 = 0,
  P1S
};

ServoModel_t servoModelList[] = {
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  KNEE, KNEE, KNEE, KNEE
};
#endif

int zeroPosition[DOF] = {};
int calibratedZeroPosition[DOF] = {};

#ifdef X_LEG
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75, 75, -75, -75,
                        -55, -55, 55, 55 };
#else
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75, 75, 75, 75,
                        -55, -55, -55, -55 };
#endif
float currentAdjust[DOF] = {};

//control related variables
#define IDLE_TIME 5000
long idleTimer;
int randomInterval = 2000;
#define CHECK_BATTERY_PERIOD 10000  //every 10 seconds. 60 mins -> 3600 seconds
int uptime = -1;
int frame = 0;
byte tStep = 0;

char token;
char lastToken;
#define CMD_LEN 10  //the last char will be '\0' so only CMD_LEN-1 elements are allowed
char *newCmd = new char[CMD_LEN];
char *lastCmd = new char[2];
byte newCmdIdx = 0;
int cmdLen;
int8_t *dataBuffer = new int8_t[450];
int8_t yprTilt[3];
int lastVoltage;
bool servoOff = true;

#ifdef RANDOM_MIND
bool autoSwitch = true;
#endif
bool checkGyro = true;
bool printGyro = false;
bool walkingQ = false;
bool serialConnectedQ = false;

byte exceptions = 0;
byte transformSpeed = 2;
float protectiveShift;  //reduce the wearing of the potentiometer

#define PT(s) Serial.print(s)  // abbreviate print commands
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))  // trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))

#ifdef DEVELOPER
#include "MemoryFree/MemoryFree.h"  //http://playground.arduino.cc/Code/AvailableMemory
#endif
#include <EEPROM.h>

//short tools
template<typename T> int8_t sign(T val) {
  return (T(0) < val) - (val < T(0));
}


void printRange(int r0 = 0, int r1 = 0) {
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      PT(i);
      PT('\t');
    }
  else
    for (byte i = r0; i < r1; i++) {
      PT(i);
      PT('\t');
    }
  PTL();
}
template<typename T> void printList(T *arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  PTL(temp);
}
template<typename T> void printTable(T *list) {
  printRange(0, DOF);
  printList(list, DOF);
}

template<typename T> void arrayNCPY(T *destination, const T *source, int len) {  //deep copy regardless of '\0'
  for (int i = 0; i < len; i++)
    destination[i] = source[i];
}

#include "sound.h"

#if defined IR_PIN
#include "infrared.h"
#endif

#ifdef NEOPIXEL_PIN
#include <Adafruit_NeoPixel.h>
//Adafruit_NeoPixel pixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif
#ifdef RANDOM_MIND
#define ALL_RANDOM  // add random joint movements between the choice list of preset behaviors
#include "randomMind.h"
#undef BINARY_COMMAND
#endif

#ifdef VOICE
#include "voice.h"
#elif defined VOICE_LD3320
#include "voiceLD3320.h"
#elif defined CAMERA
#include "camera.h"
#elif defined ULTRASONIC
#include "ultrasonic.h"
#elif defined GESTURE
#include "gesture.h"
#elif defined OTHER_MODULES
#elif defined TASK_QUEUE
#else
#define GYRO_PIN 0
#endif

#ifndef MAIN_SKETCH
#define GYRO_PIN 0
#endif

#ifndef GYRO_PIN
#define TASK_QUEUE
#endif

#ifdef TASK_QUEUE
#include "taskQueue.h"
#endif

#if defined NyBoard_V0_1 || defined NyBoard_V0_2
#undef VOLTAGE_DETECTION_PIN
#undef T_SERVO_MICROSECOND
#undef T_RAMP
#endif

#include <avr/wdt.h>  // https://create.arduino.cc/projecthub/rafitc/what-is-watchdog-timer-fffe20
#include "eeprom.h"
#include "io.h"

#ifdef GYRO_PIN
#include "imu.h"
#endif

#include "PCA9685servo.h"
#include "motion.h"
#include "skill.h"
#include "reaction.h"

void initRobot() {
  //----------------------------------
#ifdef MAIN_SKETCH  // **
  PTL('k');
#ifdef GYRO_PIN
  imuSetup();
#endif
#ifdef IR_PIN
  irrecv.enableIRIn();  // Start the receiver
  gait.reserve(4);
#endif

#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif
#ifdef NEOPIXEL_PIN
  //  pixel.begin();           // INITIALIZE NeoPixel pixel object (REQUIRED)
  //  pixel.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
#endif

  servoSetup();
  skill.assignSkillAddressToOnboardEeprom();
#ifdef RANDOM_MIND
  for (byte i = 0; i < randomMindListLength; i++) {
    randomBase += choiceWeight[i];
  }
#endif
  randomSeed(analogRead(A2));  //use the fluctuation of voltage caused by servos as entropy pool

#ifdef VOICE
  voiceSetup();
#endif
#ifdef VOICE_LD3320
  voiceLD3320Setup();
#endif
#ifdef CAMERA
  cameraSetup();
#endif
#ifdef GESTURE
  gestureSetup();
#endif

  playMelody(MELODY_NORMAL);

  delay(2000);  //change the delay if the app doesn't recognize the Petoi device.
#ifdef GYRO_PIN
  for (byte r = 0; r < 3; r++)
    read_IMU();                                                             //ypr is slow when starting up. leave enough time between IMU initialization and this reading
  token = (fabs(ypr[1]) > 30 || fabs(ypr[2]) > 30) ? T_CALIBRATE : T_REST;  //put the robot's side on the table to enter calibration posture for attaching legs
  newCmdIdx = 2;
#endif
  PTLF("\n* Start *");
#ifdef BITTLE
  PTLF("Bittle");
#elif defined NYBBLE
  PTLF("Nybble");
#elif defined CUB
  PTLF("Cub");
#endif
#ifdef TASK_QUEUE
  tQueue = new TaskQueue();
#endif
  //----------------------------------
#else  // ** save parameters to device's static memory
  configureEEPROM();
  servoSetup();  //servo needs to be after configureEEPROM and before imuSetup
#ifdef GYRO_PIN
  imuSetup();
#endif
#endif  // **

  allCalibratedPWM(currentAng);  //soft boot for servos
  delay(500);
  PTLF("Ready!");
#ifndef MAIN_SKETCH
  PCA9685CalibrationPrompt();
#endif
}