#define SOFTWARE_VERSION "N_250224"  //NyBoard + YYMMDD
//board configuration
// -- comment out these blocks to save program space for your own codes --
#define BUZZER 5

//#define SERVO_SLOW_BOOT

#define I2C_EEPROM  //comment this line out if you don't have an I2C EEPROM in your DIY board.
#define SERIAL_TIMEOUT 5
#define SERIAL_TIMEOUT_LONG 200
//Tutorial: https://bittle.petoi.com/11-tutorial-on-creating-new-skills

#define DOF 16

#ifdef NYBBLE
int8_t middleShift[] = { 0, 15, 0, 0,
                         -45, -45, -45, -45,
                         10, 10, -10, -10,
                         -30, -30, 30, 30 };
int angleLimit[][2] = {
  { -120, 120 },
  { -80, 45 },
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
int8_t middleShift[] = { 0, -90, 0, 0,
                         -45, -45, -45, -45,
                         55, 55, -55, -55,
                         -55, -55, -55, -55 };
int angleLimit[][2] = {
  { -120, 120 },
#ifdef ROBOT_ARM
  { -10, 180 },
#else
  { -85, 85 },
#endif
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

// #define INVERSE_SERVO_DIRECTION
int8_t rotationDirection[] = { 1, -1, -1, 1,
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
// #define NEOPIXEL_PIN 10  //the code for NeoPixels have to be shrinked to fit in the board
// #define NUMPIXELS 7

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
// #define NEOPIXEL_PIN 10  //the code for NeoPixels have to be shrinked to fit in the board
// #define NUMPIXELS 1
#endif

#ifdef NYBBLE
#define MODEL "Nybble"
#define HEAD
#define TAIL
#define X_LEG
#define WALKING_DOF 8
#define REGULAR G41
#define KNEE G41
#include "InstinctNybble.h"


#elif defined BITTLE
#ifdef ROBOT_ARM
#define MODEL "Bittle RN"
#else
#define MODEL "Bittle"
#endif
#define HEAD
#define TAIL  // the robot arm's clip is assigned to the tail joint
#define LL_LEG
#define WALKING_DOF 8
#define REGULAR P1S
#define KNEE P1S
#ifdef ROBOT_ARM
#include "InstinctBittle_arm.h"
#else
#include "InstinctBittle.h"
#endif
//#include "InstinctBittleShortExample.h"
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
#define BOOTUP_SOUND_STATE 282        // 1 byte
#define SKILLS 320                    // 1 byte for skill name length, followed by the char array for skill name
// then followed by i(nstinct) on progmem, or n(ewbility) on progmem

//the actual data is stored on the I2C EEPROM.
#define INITIAL_SKILL_DATA_ADDRESS 32
//The first several bytes are reserved for testing
//the above constants from onboard EEPROM to I2C EEPROM

//token list
#define T_ABORT 'a'      //abort the calibration values
#define T_BEEP 'b'       //b note1 duration1 note2 duration2 ... e.g. b12 8 14 8 16 8 17 8 19 4 \
                         //a single 'b' will toggle the melody on/off
#define T_CALIBRATE 'c'  //send the robot to calibration posture for attaching legs and fine-tuning the joint offsets. \
                         //c jointIndex1 offset1 jointIndex2 offset2 ... e.g. c0 7 1 -4 2 3 8 5
#define T_REST 'd'
#define T_GYRO_FINENESS 'g'             //adjust the finess of gyroscope adjustment to accelerate motion
#define T_GYRO_BALANCE 'G'              //toggle on/off the gyro adjustment
#define T_INDEXED_SIMULTANEOUS_ASC 'i'  //i jointIndex1 jointAngle1 jointIndex2 jointAngle2 ... e.g. i0 70 8 -20 9 -20 \
                                        //a single 'i' will free the head joints if it were previously manually controlled.
#define T_JOINTS 'j'                    //A single "j" returns all angles. "j Index" prints the joint's angle. e.g. "j 8" or "j11".
#define T_SKILL 'k'
#define T_SKILL_DATA 'K'
#define T_INDEXED_SEQUENTIAL_ASC 'm'  //m jointIndex1 jointAngle1 jointIndex2 jointAngle2 ... e.g. m0 70 0 -70 8 -20 9 -20
// #define T_MELODY 'o'
#define T_PAUSE 'p'
// #define T_SLOPE 'l'
#define T_SAVE 's'  //save the calibration values
// #define T_TILT 't'
// #define T_MEOW 'u'
#define T_PRINT_GYRO 'v'            //print the Gyro data once
#define T_VERBOSELY_PRINT_GYRO 'V'  //toggle verbosely print Gyro data
// #define T_XLEG        'x'
// #define T_ACCELERATE  '.'
// #define T_DECELERATE  ','
#define T_RANDOM_MIND 'z'  //toggle random behaviors in the RANDOM_MIND mode
#define T_QUERY '?'

#ifdef GROVE_SERIAL_PASS_THROUGH
#define T_READ 'R'        //read pin     R
#define T_WRITE 'W'       //write pin                      W
#define TYPE_ANALOG 'a'   //            Ra(analog read)   Wa(analog write)
#define TYPE_DIGITAL 'd'  //            Rd(digital read)  Wd(digital write)
#endif
#define T_COLOR 'C'                     //change the eye colors of the RGB ultrasonic sensor
#define T_INDEXED_SIMULTANEOUS_BIN 'I'  //I jointIndex1 jointAngle1 jointIndex2 jointAngle2 ... e.g. I0 70 8 -20 9 -20
#define T_INDEXED_SEQUENTIAL_BIN 'M'    //M jointIndex1 jointAngle1 jointIndex2 jointAngle2 ... e.g. M0 70 0 -70 8 -20 9 -20

#define BINARY_COMMAND  //disable the binary commands to save space for the simple random demo

#ifdef BINARY_COMMAND
#define T_BEEP_BIN 'B'    //B note1 duration1 note2 duration2 ... e.g. B12 8 14 8 16 8 17 8 19 4
#define T_LISTED_BIN 'L'  //a list of the DOFx joint angles: angle0 angle1 angle2 ... angle15
// #define T_SERVO_MICROSECOND 'w'  //PWM width modulation
#define T_TEMP 'T'  //call the last 'K' skill data received from the serial port
#endif

#define EXTENSION 'X'
#define EXTENSION_VOICE 'A'
#define EXTENSION_ULTRASONIC 'U'


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
#define IDLE_TIME 3000
long idleTimer = 0;
int randomInterval = 2000;
#define CHECK_BATTERY_PERIOD 10000  //every 10 seconds. 60 mins -> 3600 seconds
int uptime = -1;
int frame = 0;
byte tStep = 1;

char token;
char lowerToken;
char lastToken;
byte newCmdIdx = 0;
int cmdLen = 0;
int8_t periodGlobal = 0;
#define CMD_LEN 10    //the last char will be '\0' so only CMD_LEN-1 elements are allowed
#define BUFF_LEN 467  //452
// char *newCmd = new char[BUFF_LEN + 1];
// char *lastCmd = new char[CMD_LEN + 1];  //the last char must be '\0' for safe so CMD_LEN+1 elements are required
char newCmd[BUFF_LEN + 1];
char lastCmd[CMD_LEN + 1];
int spaceAfterStoringData = BUFF_LEN;
//22*20+7=447, +1 for '\0'.
//The max behavior allowed has 22 frames. The max gait (8 DoF) allowed has (448-4)/8=55.5 frames.
//so 56*8 + 4 = 452 is the most efficient

//468: 23*20+7=467, +1 for '\0'.
//The max behavior allowed has 23 frames. The max gait (8 DoF) allowed has (468-4)/8=58 frames.
//so 468 is the most efficient

int8_t yprTilt[3];
int lastVoltage;
bool servoOff = true;

#ifdef RANDOM_MIND
bool autoSwitch = true;
#endif
bool fineAdjust = true;
bool gyroBalanceQ = true;
bool printGyro = false;
bool walkingQ = false;
// bool serialDominateQ = false;
bool manualHeadQ = false;
bool nonHeadJointQ = false;
bool hardServoQ = true;
bool manualEyeColorQ = false;
#define HEAD_GROUP_LEN 4  //used for controlling head pan, tilt, tail, and other joints independent from walking
// #define DAMPED_MOTION
int targetHead[HEAD_GROUP_LEN];


int exceptions = 0;
byte transformSpeed = 2;
float protectiveShift;  //reduce the wearing of the potentiometer

#include "MemoryFree/MemoryFree.h"  //http://playground.arduino.cc/Code/AvailableMemory
#ifdef DEVELOPER
// #include "MemoryFree/MemoryFree.h"  //http://playground.arduino.cc/Code/AvailableMemory
#endif
#include <EEPROM.h>

#include "tools.h"
#include <avr/wdt.h>  // https://create.arduino.cc/projecthub/rafitc/what-is-watchdog-timer-fffe20
#include "eeprom.h"
#include "sound.h"

#if defined IR_PIN
#include "infrared.h"
#endif

#ifdef NEOPIXEL_PIN
#include <Adafruit_NeoPixel.h>
//Adafruit_NeoPixel pixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

#ifdef RANDOM_MIND
// #define ALL_RANDOM  // add random joint movements between the choice list of preset behaviors \
                    // when it's activated, the gyro will be disabled
#include "randomMind.h"
#undef BINARY_COMMAND
#endif

#if defined NyBoard_V0_1 || defined NyBoard_V0_2
#undef VOLTAGE_DETECTION_PIN
#undef T_SERVO_MICROSECOND
#undef T_SLOPE
#endif

#include "PCA9685servo.h"
#include "motion.h"

#ifndef MAIN_SKETCH
#define GYRO_PIN 0

#else

#define TASK_QUEUE  //allow executing a sequence of tasks, if you enabled the other modules, the task queue will be automatically enabled. \
  // because it takes up memory, it should be disabled if the GYRO is enabled. See "#undef TASK_QUEUE" under ifdef GYRO
#ifdef TASK_QUEUE
#include "taskQueue.h"
#define T_TASK_QUEUE 'q'
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
#elif defined PIR
#include "pir.h"
#elif defined DOUBLE_TOUCH
#include "doubleTouch.h"
#elif defined DOUBLE_LIGHT
#include "doubleLight.h"
#elif defined DOUBLE_INFRARED_DISTANCE
#include "doubleInfraredDistance.h"
#elif defined GROVE_SERIAL_PASS_THROUGH
#include "ultrasonic.h"
#elif defined OTHER_MODULES
#elif defined ALL_RANDOM
#else
#define GYRO_PIN 0
#endif

#endif

#include "io.h"

#ifdef GYRO_PIN
#include "imu.h"
#ifdef IR_PIN
#undef TASK_QUEUE
#endif
#endif

#ifdef GROVE_SERIAL_PASS_THROUGH
#undef IR_PIN
#endif

#include "skill.h"
#include "reaction.h"

void initRobot() {
  //----------------------------------
  beep(20);
#ifdef MAIN_SKETCH  // **
  PTL('k');
  PTLF("\n* Start *");
  PTLF(MODEL);
  PTLF(SOFTWARE_VERSION);
  if (eeprom(BOOTUP_SOUND_STATE))
    playMelody(MELODY_NORMAL);

#ifdef GYRO_PIN
  imuSetup();
#endif
  servoSetup();
  skill.assignSkillAddressToOnboardEeprom();
#ifdef RANDOM_MIND
  for (byte i = 0; i < randomMindListLength; i++) {
    randomBase += choiceWeight[i];
  }
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

#ifdef TASK_QUEUE
  tQueue = new TaskQueue();
#endif

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

#if defined DOUBLE_LIGHT || defined DOUBLE_TOUCH || defined DOUBLE_INFRARED_DISTANCE || defined ULTRASONIC
#ifndef GROVE_SERIAL_PASS_THROUGH
  skill.loadFrame("sit");  //required by double light
  delay(500);              //use your palm to cover the two light sensors for calibration
#endif
#ifdef DOUBLE_INFRARED_DISTANCE
  doubleInfraredDistanceSetup();
#endif
#ifdef DOUBLE_LIGHT
  doubleLightSetup();
#endif
#endif

#ifdef GYRO_PIN
  for (byte r = 0; r < 50; r++) {  //ypr is slow when starting up. leave enough time between IMU initialization and this reading
    read_IMU();
    delay(10);
  }
  token = (fabs(ypr[1]) > 30 || fabs(ypr[2]) > 30) ? T_CALIBRATE : T_REST;  //put the robot's side on the table to enter calibration posture for attaching legs
  newCmdIdx = 2;
  int rndSeed = ypr[2] * 10000;
#else
  int rndSeed = analogRead(A2);
#endif
  randomSeed(rndSeed);  //use the fluctuation of voltage caused by servos as entropy pool

  allCalibratedPWM(currentAng);  //soft boot for servos
  delay(500);
  lastCmd[0] = '\0';
//----------------------------------
#else  // ** save parameters to device's static memory
  configureEEPROM();
  servoSetup();  //servo needs to be after configureEEPROM and before imuSetup
#ifdef GYRO_PIN
  imuSetup();
#endif
#endif  // **
  PTLF("Ready!");
#ifndef MAIN_SKETCH
  PCA9685CalibrationPrompt();
#endif
}
