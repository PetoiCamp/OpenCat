/*
   The driver for OpenCat, runs on ATmega328P-based NyBoard (as Arduino Uno).
   Compatible with Petoi Nybble, Bittle and many other 8 or 12 DoF quadruped robots.
   Drives up to 16 PWM servos.

   Rongzhong Li
   Mar 15rd, 2022
   Copyright (c) 2022 Petoi LLC.

   This sketch may also include others' codes under MIT or other open-source licenses.
   Check those licenses in corresponding module test folders.
   Feel free to contact us if you find any missing references.

  The MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/*
  To configure the board:
  1. Select your robot and board version.
  2. Comment out #define MAIN_SKETCH and upload. Upload and follow the serial prompts to proceed.
  3. Uncomment #define MAIN_SKETCH to make it active. Then upload the program for main functions.
*/

#define MAIN_SKETCH
//#define AUTO_INIT //automatically select 'Y' for the prompts
//#define DEVELOPER //to print out some verbose debugging data

#define BITTLE    //Petoi 9 DOF robot dog: 1x on head + 8x on leg
//#define NYBBLE  //Petoi 11 DOF robot cat: 2x on head + 1x on tail + 8x on leg

//#define NyBoard_V0_1
//#define NyBoard_V0_2
#define NyBoard_V1_0
//#define NyBoard_V1_1

//choose only one of the following modes (they will diable the gyro to save programming space)
//#define RANDOM_MIND       //let the robot do random stuffs. use token 'z' to activate/deactivate
//#define ULTRASONIC      //for Nybble's ultrasonic sensor
//#define VOICE           //for LD3320 module


#include "src/OpenCat.h"

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(5);
  PTLF(">Flush>");
  delay(2000);
  // join I2C bus (I2Cdev library doesn't do this automatically)
  //#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
  //#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  //  Fastwire::setup(400, true);
  //#endif

  //----------------------------------
#ifdef MAIN_SKETCH  // **
#ifdef GYRO_PIN
  imuSetup();
#endif
#ifdef IR_PIN
  irrecv.enableIRIn(); // Start the receiver
  gait.reserve(4);
#endif
  servoSetup();
  randomSeed(analogRead(0));//use the fluctuation of voltage caused by servos as entropy pool

  skill.assignSkillAddressToOnboardEeprom();

#ifdef VOICE
  voiceSetup();
#endif

  playMelody(MELODY_NORMAL);
#ifdef GYRO_PIN
  read_IMU();  //ypr is slow when starting up. leave enough time between IMU initialization and this reading
  token = (fabs(ypr[1]) > 30 || fabs(ypr[2]) > 30) ? T_CALIBRATE : T_REST; //put the robot's side on the table to enter calibration posture for attaching legs
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
  //----------------------------------
#else               // ** save parameters to device's static memory
  configureEEPROM();

  servoSetup(); //servo needs to be after configureEEPROM and before imuSetup
#ifdef GYRO_PIN
  imuSetup();
#endif
#endif              // **

  PTL("Ready!");
}

void loop() {
#ifdef MAIN_SKETCH
#ifdef VOLTAGE_DETECTION_PIN
  while (lowBattery()); //block the loop if battery is low
#endif
#ifdef RANDOM_MIND
  if (autoSwitch)
    randomMind();//make the robot do random demos
#endif
  readSignal();
#ifdef GYRO_PIN
  readEnvironment();
  if (token != T_CALIBRATE && exceptions) //the gyro reaction switch can be toggled on/off by the 'g' token
    dealWithExceptions(); //fall over, lifted, etc.
#endif
  reaction();
#endif
}
