/*
   Instincts trained by Rongzhong
   Save calibration data and constants to EEPROM

   This sketch may also includes others' codes under MIT or other open source liscence.
   Check those liscences in corresponding module test folders.
   Feel free to contact us if you find any missing references.

   Rongzhong Li
   Jan 3rd, 2021
   Copyright (c) 2021 Petoi LLC.

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
#include "OpenCat.h"
#define INSTINCT_SKETCH
//#define AUTORUN

void saveMPUcalib(int * var) {
  for (byte i = 0; i < 6; i++)
    EEPROM.update(MPUCALIB + i, var[i]);
}

void writeConst() {
  EEPROM.update(MELODY, sizeof(melody));
  for (byte i = 0; i < sizeof(melody); i++)
    EEPROM.update(MELODY - 1 - i, melody[i]);
  PTLF("Reset joint calibration? (Y/n)");
#ifndef AUTORUN
  while (!Serial.available());
  char resetJointCalibrationQ = Serial.read();
#endif
  for (byte i = 0; i < DOF; i++) {
#ifndef AUTORUN
    if (resetJointCalibrationQ == 'Y' || resetJointCalibrationQ == 'y')
      EEPROM.update(CALIB + i, calibs[i]);
#endif
    EEPROM.update(PIN + i, pins[i]);
    EEPROM.update(MID_SHIFT + i, middleShifts[i]);
    EEPROM.update(ROTATION_DIRECTION + i, rotationDirections[i]);
    EEPROM.update(SERVO_ANGLE_RANGE + i, servoAngleRanges[i]);
    for (byte para = 0; para < NUM_ADAPT_PARAM; para++) {
      EEPROM.update(ADAPT_PARAM + i * NUM_ADAPT_PARAM + para, round(adaptiveParameterArray[i][para]));
    }
    /*PT(servoCalib(i));
      PT(',');*/
  }
  //PTL();
}
void saveSkillInfoFromProgmemToOnboardEeprom() {
  int skillAddressShift = 0;
  unsigned int i2cEepromAddress = 0; //won't hurt if unused
#ifdef I2C_EEPROM
  PTLF("\n* Update Instincts? (Y/n)");
#ifndef AUTORUN
  while (!Serial.available());
  char choice = Serial.read();
  PT((choice == 'Y' || choice == 'y') ? "Will" : "Won't");
#endif
  PTL(" overwrite Instincts on external I2C EEPROM!");
#endif
  PTLF("Saving skill info...");
  for (byte s = 0; s < NUM_SKILLS; s++) {//save skill info to on-board EEPROM
    byte len = strlen(skillNameWithType[s]);
    EEPROM.update(SKILLS + skillAddressShift++, len - 1); //the last char in name denotes skill type, I(nstinct) on external eeprom, N(ewbility) on progmem
    PT(skillNameWithType[s][len - 1] == 'I' ? "I nstinct:\t" : "N ewbility:\t");
    for (byte l = 0; l < len; l++) {
      PT(skillNameWithType[s][l]);
      EEPROM.update(SKILLS + skillAddressShift++, skillNameWithType[s][l]);
    }
    PTL();
    //PTL("Current EEPROM address is " + String(SKILLS + skillAddressShift));
#ifdef I2C_EEPROM
    if (!EEPROMOverflow)
      if (skillNameWithType[s][len - 1] == 'I'
#ifndef AUTORUN
          && (choice == 'Y' || choice == 'y')
#endif
         ) { //  if there's instinct and there's i2c eeprom, and user decide to update.
        // save the data array to i2c eeprom. its address will be saved to onboard eeprom
        EEPROMWriteInt(SKILLS + skillAddressShift, i2cEepromAddress);
        copyDataFromPgmToI2cEeprom(i2cEepromAddress,  (unsigned int) progmemPointer[s]);
      }
#endif
    skillAddressShift += 2; // one int (2 bytes) for address
  }
  PTLF("  ***");
  PTLF("    On-chip EEPROM has 1024 bytes.");
  PTF("\tInstinctive dictionary used ");
  PT(SKILLS + skillAddressShift);
  PT(" bytes (");
  PT(float(100) * (SKILLS + skillAddressShift) / 1024);
  PTLF(" %)!");
#ifdef I2C_EEPROM
#ifndef AUTORUN
  if (choice == 'Y' || choice == 'y')
#endif
  {
    PTF("    Maximal storage of external I2C EEPROM is ");
    PT(EEPROM_SIZE);
    PTLF(" bytes.");
    PT("\tInstinctive data used ");
    PT(i2cEepromAddress);
    PT(" bytes (");
    PT(float(100)*i2cEepromAddress / EEPROM_SIZE);
    PTLF(" %)!");
  }
#endif
  PTLF("  ***");
  PTLF("Finished!");
}

// the following MPU calibration codes are modified based on http://wired.chillibasket.com/2015/01/calibrating-mpu6050/
// Rongzhong Li
// Sep 16, 2018

// Arduino sketch that returns calibration offsets for MPU6050
//   Version 1.1  (31th January 2014)
// Done by Luis Ródenas <luisrodenaslorda@gmail.com>
// Based on the I2Cdev library and previous work by Jeff Rowberg <jeff@rowberg.net>
// Updates (of the library) should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib

// These offsets were meant to calibrate MPU6050's internal DMP, but can be also useful for reading sensors.
// The effect of temperature has not been taken into account so I can't promise that it will work if you
// calibrate indoors and then use it outdoors. Best is to calibrate and use at the same room temperature.

/* ==========  LICENSE  ==================================
  I2Cdev device library code is placed under the MIT license
  Copyright (c) 2011 Jeff Rowberg

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  =========================================================
*/

// I2Cdev and MPU6050 must be installed as libraries
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>

///////////////////////////////////   CONFIGURATION   /////////////////////////////
//Change this 3 variables if you want to fine tune the skecth to your needs.
int discard = 100;
int buffersize = 1000;   //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone = 8;   //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone = 1;   //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

// default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
//MPU6050 mpu;
MPU6050 mpu(0x68); // <-- use for AD0 high


byte stage = 0;
char choice;
int ag[6];      //int16_t ax, ay, az, gx, gy, gz;
int agMean[6];  //mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;
int agOffset[6];  //ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;
int mpuOffset[6];
uint8_t timer = 0;
#define SKIP 3
#ifdef SKIP
byte updateFrame = 0;
#endif
byte firstValidJoint;
char token;
#define CMD_LEN 10
char lastCmd[CMD_LEN] = {};
byte jointIdx = 0;

#define CMD_LEN 10
bool printMPU = false;


void setup() {
#ifdef PIXEL_PIN
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  pixels.setPixelColor(NUMPIXELS - 1, pixels.Color(LIT_ON, 0, 0));

  pixels.show();   // Send the updated pixel colors to the hardware.
#endif

  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(BAUD_RATE);
  Serial.setTimeout(5);
  delay(1);
  while (!Serial);//check here
  /*PTLF("MPU calibration data");
    for (byte i = 0; i < 6; i++){
    PTL(EEPROMReadInt(MPUCALIB + i * 2));
    PT("\t");
    }
    PTL();*/
  beep(100, 50);
  // initialize device
  mpu.initialize();

  // wait for ready
  while (Serial.available() && Serial.read()); // empty buffer
  PTLF("\n* Change \"#define NyBoard_V*_*\" in OpenCat.h according to your NyBoard version!");
  PTLF("\n* OpenCat Writing Constants to EEPROM...");
  writeConst(); // only run for the first time when writing to the board.
  beep(30);
  saveSkillInfoFromProgmemToOnboardEeprom();
  assignSkillAddressToOnboardEeprom();

  // servo
  { pwm.begin();
    pwm.setPWMFreq(60 * PWM_FACTOR); // Analog servos run at ~60 Hz updates
    delay(100);
    strcpy(lastCmd, "rest");
    motion.loadBySkillName("rest");
    for (byte i = 0; i < DOF; i++) {
      pulsePerDegree[i] = float(PWM_RANGE) / servoAngleRange(i);
      servoCalibs[i] = servoCalib(i);
      calibratedDuty0[i] =  SERVOMIN + PWM_RANGE / 2 + float(middleShift(i) + servoCalibs[i]) * pulsePerDegree[i]  * rotationDirection(i) ;
      calibratedPWM(i, motion.dutyAngles[i]);
    }
    shutServos();
    token = T_REST;
  }
  beep(15, 50, 50, 5);
  // start message
  PTLF("\nCalibrate MPU? (Y/n)");
#ifndef AUTORUN
  while (!Serial.available());
  choice = Serial.read();
  PTLF("Gotcha!");
  if (choice == 'Y' || choice == 'y')
#endif
  {
    PTLF("\n* MPU6050 Calibration Routine");
#ifdef PIXEL_PIN
    pixels.setPixelColor(NUMPIXELS - 1, pixels.Color(0, 0, LIT_ON));
    pixels.show();   // Send the updated pixel colors to the hardware.
#endif
    delay(100);
    // verify connection
    PTL(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed (sometimes it shows \"failed\" but is ok to bypass."));
    delay(100);
    // reset offsets
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
  }
}

void loop() {
#ifndef AUTORUN
  if (choice == 'Y' || choice == 'y')
#endif
  {
    if (stage == 0) {
      PTLF("\nReading sensors for first time...");
      meansensors();
      stage++;
      delay(100);
    }

    if (stage == 1) {
      PTLF("\nYour MPU6050 should be placed in horizontal position, with package letters facing up.");
      PTLF("Don't touch it until all six numbers appear. You should hear a long beep followed by a Meooow!");
//      for (byte i = 0; i < 6; i++) {
//        EEPROMWriteInt(MPUCALIB + i * 2, 0);
//      }
      calibration();
      stage++;
      delay(100);
    }

    if (stage == 2) {
      meansensors();
      PTLF("FINISHED!");
      PTLF("\nData is printed as:\t\tacelX\tacelY\tacelZ\tgiroX\tgiroY\tgiroZ");
      PTLF("Readings should be close to:\t0\t0\t16384\t0\t0\t0");

      PTF("Sensor readings with offsets:\t");
      printList(agMean, 6);

      PTF("Your calibration offsets:\t");
      printList(agOffset, 6);

      PTLF("The offsets are saved and automatically sent to mpu.setXAccelOffset(yourOffset)\n");
      for (byte i = 0; i < 6; i++) {
        mpuOffset[i] = agOffset[i];
        EEPROMWriteInt(MPUCALIB + i * 2, mpuOffset[i]);
      }

      mpu.setXAccelOffset(agOffset[0]);
      mpu.setYAccelOffset(agOffset[1]);
      mpu.setZAccelOffset(agOffset[2]);
      mpu.setXGyroOffset(agOffset[3]);
      mpu.setYGyroOffset(agOffset[4]);
      mpu.setZGyroOffset(agOffset[5]);
      //while (1);
      stage = 3;
      meow();
    }
  }

  char cmd[CMD_LEN] = {};
  byte newCmd = 0;
  if (Serial.available() > 0) {
    token = Serial.read();
    newCmd = 3;
  }
  if (newCmd) {
    beep(newCmd * 10);
    // this block handles argumentless tokens

    switch (token) {
      case T_GYRO: {
          printMPU = !printMPU;
          break;
        }
      // if (token == T_HELP)
      //   PTLF("** Help Information **");// print the help document
      case T_REST : {
          skillByName("rest");
          break;
        }
      case T_JOINTS: { //show the list of current joint angles
          printRange(DOF);
          printList(currentAng);
          break;
        }
      case T_SAVE: {
          PTLF("save calibration");
          saveCalib(servoCalibs);
          break;
        }
      case T_ABORT: {
          PTLF("abort calibration");
          for (byte i = 0; i < DOF; i++) {
            servoCalibs[i] = servoCalib( i);
          }
          break;
        }
      // this block handles array like arguments
      case T_CALIBRATE:
      case T_MOVE: {
          int target[2] = {};
          String inBuffer = Serial.readStringUntil('\n');
          byte inLen = 0;
          strcpy(cmd, inBuffer.c_str());
          char *pch;
          pch = strtok (cmd, " ,\t");
          for (int c = 0; pch != NULL; c++)
          {
            target[c] = atoi(pch);
            pch = strtok (NULL, " ,");
            inLen++;
          }

          if (token == T_CALIBRATE) {
            //PTLF("calibrating [ targetIdx, angle ]: ");
            if (strcmp(lastCmd, "c")) { //first time entering the calibration function
              motion.loadBySkillName("calib");
              transform(motion.dutyAngles, 1, 1, 0.5);
            }
            if (inLen == 2)
              servoCalibs[target[0]] = target[1];
            PTL();
            printRange(DOF);
            printList(servoCalibs);
            yield();

          }
          else if (token == T_MOVE) {
            PTLF("moving [ targetIdx, angle ]: ");
            currentAng[target[0]] = motion.dutyAngles[target[0]] = target[1];
          }
          PT(token);
          printList(target, 2);

          int duty = SERVOMIN + PWM_RANGE / 2 + float(middleShift(target[0])  + servoCalibs[target[0]] +  motion.dutyAngles[target[0]]) * pulsePerDegree[target[0]] * rotationDirection(target[0]) ;
          pwm.setPWM(pin(target[0]), 0,  duty);
          //      PT("shift ");
          //      PT(middleShift(target[0]));
          //      PT("\tcalib ");
          //      PT(servoCalibs[target[0]]);
          //      PT("\ttarget ");
          //      PT(target[1]);
          //      PT("\tPWM_range ");
          //      PT(PWM_RANGE);
          //      PT("\tangle_range ");
          //      PT(servoAngleRange(target[0]));
          //      PT("\tPpD ");
          //      PT(pulsePerDegree[target[0]]);
          //      PT("\tduty ");
          //      PTL(duty);
          break;
        }
      default: if (Serial.available() > 0) {
          String inBuffer = Serial.readStringUntil('\n');
          strcpy(cmd, inBuffer.c_str());
        }
    }
    while (Serial.available() && Serial.read()); //flush the remaining serial buffer in case the commands are parsed incorrectly

    if (strcmp(cmd, "") && strcmp(lastCmd, cmd) ) {
      //      PT("compare lastCmd ");
      //      PT(lastCmd);
      //      PT(" with newCmd ");
      //      PT(token);
      //      PT(cmd);
      //      PT("\n");
      if (token == T_UNDEFINED) {}; //some words for undefined behaviors

      if (token == T_SKILL) { //validating key
        motion.loadBySkillName(cmd);
        //motion.info();
#ifdef DEVELOPER
        PTF("free memory: ");
        PTL(freeMemory());
#endif
        timer = 0;
        if (strcmp(cmd, "balance") && strcmp(cmd, "lifted") && strcmp(cmd, "dropped") )
          strcpy(lastCmd, cmd);
        // if posture, start jointIdx from 0
        // if gait, walking DOF = 8, start jointIdx from 8
        //          walking DOF = 12, start jointIdx from 4
        firstValidJoint = (motion.period == 1) ? 0 : DOF - WALKING_DOF;
        jointIdx = firstValidJoint;
        transform( motion.dutyAngles, 1, 1, firstValidJoint);
        if (!strcmp(cmd, "rest")) {
          shutServos();
          token = T_REST;
        }
      }
      else {
        lastCmd[0] = token;
        memset(lastCmd + 1, '\0', CMD_LEN - 1);
      }
    }
  }

  //motion block
  {
    if (printMPU) {
      mpu.getMotion6(ag, ag + 1, ag + 2, ag + 3, ag + 4, ag + 5);
      //PTLF("ax\tay\taz\tgx\tgy\tgz: ");
      printList(ag, 6);
    }

    if (token == T_SKILL) {
#ifndef HEAD  //skip head
      if (jointIdx == 0)
        jointIdx = 2;
#endif
#ifndef TAIL  //skip tail
      if (jointIdx == 2)
        jointIdx = 4;
#endif
      if (motion.period != 1) {//skip non-walking DOF
        if (jointIdx < 4)
          jointIdx = 4;

      }
#if WALKING_DOF==8 //skip shoulder roll 
      if (jointIdx == 4)
        jointIdx = 8;
#endif
      int dutyIdx = timer * WALKING_DOF + jointIdx - firstValidJoint;
      calibratedPWM(jointIdx, motion.dutyAngles[dutyIdx] );
      jointIdx++;

      if (jointIdx == DOF) {
        jointIdx = 0;
#ifdef SKIP
        if (updateFrame++ == SKIP) {
          updateFrame = 0;
#endif
          timer = (timer + 1) % motion.period;
#ifdef SKIP
        }
#endif
      }
    }
  }
}

void meansensors() {
  long i = 0;
  long * agBuff = new long[6] {0, 0, 0, 0, 0, 0}; //buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;

  while (i < (buffersize + discard + 1)) {
    // read raw accel/gyro measurements from device
    mpu.getMotion6(ag, ag + 1, ag + 2, ag + 3, ag + 4, ag + 5);

    if (i > discard && i <= (buffersize + discard)) { //First 100 measures are discarded
      for (byte i = 0; i < 6; i++)
        agBuff[i] += ag[i];
      /*
        //replacing the following codes
        buff_ax = buff_ax + ax;
        buff_ay = buff_ay + ay;
        buff_az = buff_az + az;
        buff_gx = buff_gx + gx;
        buff_gy = buff_gy + gy;
        buff_gz = buff_gz + gz;*/
    }
    if (i == (buffersize + discard)) {
      for (byte i = 0; i < 6; i++)
        agMean[i] = agBuff[i] / buffersize;
      /*mean_ax = buff_ax / buffersize;
        mean_ay = buff_ay / buffersize;
        mean_az = buff_az / buffersize;
        mean_gx = buff_gx / buffersize;
        mean_gy = buff_gy / buffersize;
        mean_gz = buff_gz / buffersize;*/
    }
    i++;
    delay(2); //Needed so we don't get repeated measures
  }
  delete [] agBuff;
}

void calibration() {
  for (int i = 0; i < 6; i++) {
    agOffset[i] = ((i == 2 ? 16384 : 0) - agMean[i]) / 8; //agOffset[2] is az_offset

    /*
       //replacing the following codes
       ax_offset = -mean_ax / 8;
      ay_offset = -mean_ay / 8;
      az_offset = (16384 - mean_az) / 8;

      gx_offset = -mean_gx / 4;
      gy_offset = -mean_gy / 4;
      gz_offset = -mean_gz / 4;*/
  }
  while (1) {
    int ready = 0;
    mpu.setXAccelOffset(agOffset[0]);
    mpu.setYAccelOffset(agOffset[1]);
    mpu.setZAccelOffset(agOffset[2]);
    mpu.setXGyroOffset(agOffset[3]);
    mpu.setYGyroOffset(agOffset[4]);
    mpu.setZGyroOffset(agOffset[5]);
    /*
       //replacing the following codes
      mpu.setXAccelOffset(ax_offset);
      mpu.setYAccelOffset(ay_offset);
      mpu.setZAccelOffset(az_offset);
      mpu.setXGyroOffset(gx_offset);
      mpu.setYGyroOffset(gy_offset);
      mpu.setZGyroOffset(gz_offset);*/

    meansensors();

    for (int i = 0; i < 6; i++) {
      int tolerance = (i < 3) ? acel_deadzone : giro_deadzone;
      if (abs((i == 2 ? 16384 : 0) - agMean[i]) <= tolerance) {
        PT(i + 1);
        beep(i * 2 + (i == 3 ? 0 : 1), 100, 10); // note F to G takes half tone
#ifdef PIXEL_PIN
        pixels.setPixelColor(i, pixels.Color(0, LIT_ON, 0));
#endif
        ready++;
      }
      else {
        PT('.');
#ifdef PIXEL_PIN
        pixels.setPixelColor(i, pixels.Color(LIT_ON, 0, 0));
#endif

        agOffset[i] -= (agMean[i] - (i == 2 ? 16384 : 0)) / (tolerance == 1 ? 3 : tolerance);
      }
    }
#ifdef PIXEL_PIN
    pixels.show();   // Send the updated pixel colors to the hardware.
#endif
    PTL();
    for (int i = 0; i < 6; i++) {
      PT(agOffset[i]);
      PT("\t");
    }
    PTL();
    /*  //replacing the following codes

          if (abs(mean_ax) <= acel_deadzone) {
            PT (1);
            beep(1, 100, 10);
            ready++;
          }
          else {
            PT('.');
            ax_offset = ax_offset - mean_ax / acel_deadzone;
          }

          if (abs(mean_ay) <= acel_deadzone)  {
            PT (2);
            beep(3, 100, 10);
            ready++;
          }
          else {
            PT('.');
            ay_offset = ay_offset - mean_ay / acel_deadzone;
          }

          if (abs(16384 - mean_az) <= acel_deadzone)  {
            PT (3);
            beep(5, 100, 10);
            ready++;
          }
          else {
            PT('.');
            az_offset = az_offset + (16384 - mean_az) / acel_deadzone;
          }

          if (abs(mean_gx) <= giro_deadzone) {
            PT (4);
            beep(6, 100, 10);
            ready++;
          }
          else {
            PT('.');
            gx_offset = gx_offset - mean_gx / (giro_deadzone + 2);
          }

          if (abs(mean_gy) <= giro_deadzone) {
            PT (5);
            beep(8, 100, 10);
            ready++;
          }
          else {
            PT('.');
            gy_offset = gy_offset - mean_gy / (giro_deadzone + 2);
          }

          if (abs(mean_gz) <= giro_deadzone)  {
            PT (6);
            beep(10, 100, 10);
            ready++;
          }
          else {
            PT('.');
            gz_offset = gz_offset - mean_gz / (giro_deadzone + 2);
          }
    */

    if (ready == 6) {
      beep(100, 1000);
#ifdef PIXEL_PIN
      for (int i = 0; i < NUMPIXELS - 1; i++) { // For each pixel...
        pixels.setPixelColor(i, pixels.Color(0, 0, LIT_ON));
      }
      pixels.setPixelColor(NUMPIXELS - 1, pixels.Color(0, LIT_ON, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
#endif
      break;
    }
  }
}
