// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v6.12)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2019-07-10 - Uses the new version of the DMP Firmware V6.12
//                 - Note: I believe the Teapot demo is broken with this versin as
//                 - the fifo buffer structure has changed
//      2016-04-18 - Eliminated a potential infinite loop
//      2013-05-08 - added seamless Fastwire support
//                 - added note about gyro calibration
//      2012-06-21 - added note about Arduino 1.0.1 + Leonardo compatibility error
//      2012-06-20 - improved FIFO overflow handling and simplified read process
//      2012-06-19 - completely rearranged DMP initialization code and simplification
//      2012-06-13 - pull gyro and accel data from FIFO packet instead of reading directly
//      2012-06-09 - fix broken FIFO read sequence and change interrupt detection to RISING
//      2012-06-05 - add gravity-compensated initial reference frame acceleration output
//                 - add 3D math helper file to DMP6 example sketch
//                 - add Euler output and Yaw/Pitch/Roll output formats
//      2012-06-04 - remove accel offset clearing for better results (thanks Sungon Lee)
//      2012-06-01 - fixed gyro sensitivity to be 2000 deg/sec instead of 250
//      2012-05-30 - basic DMP initialization working

/* ============================================
  I2Cdev device library code is placed under the MIT license
  Copyright (c) 2012 Jeff Rowberg

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
  ===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "mpu6050/I2Cdev.h"

//#include "MPU6050_6Axis_MotionApps_V6_12.h"

#include "mpu6050/MPU6050_6Axis_MotionApps_V6_12.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
   ========================================================================= */

/* =========================================================================
   NOTE: Arduino v1.0.1 with the Leonardo board generates a compile error
   when using Serial.write(buf, len). The Teapot output uses this method.
   The solution requires a modification to the Arduino USBAPI.h file, which
   is fortunately simple, but annoying. This will be fixed in the next IDE
   release. For more info, see these links:

   http://arduino.cc/forum/index.php/topic,109987.0.html
   http://code.google.com/p/arduino/issues/detail?id=958
   ========================================================================= */

// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
//#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
//#define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
//#define OUTPUT_READABLE_WORLDACCEL

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
//#define OUTPUT_TEAPOT

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 gy;         // [x, y, z]            gyro sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

#define IMU_SKIP 2
int16_t imuOffset[9] = {0, 0, 0,
                        0, 0, 0,
                        0, 0, 0
                       };

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

//volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
//void dmpDataReady() {
//  mpuInterrupt = true;
//}


void print6Axis() {
  PT(ypr[0] );
  PT('\t');
  PT(ypr[1] );
  PT('\t');
  PT(ypr[2] );
  //  PT("\t");
  //  PT(aaWorld.x);
  //  PT("\t");
  //  PT(aaWorld.y);
  //  PT("\t");
  //  PTL(aaWorld.z / 1000);
  //  PT('\t');
  //  PT(aaReal.z);
  PTL();
}
bool read_IMU() {
  //  mpuInterrupt = false;
  //  mpuIntStatus = mpu.getIntStatus();
  //  PTL(mpuInterrupt);
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    //    mpu.dmpGetAccel(&aa, fifoBuffer);
    //    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

    for (int i = 0; i < 3; i++)
      ypr[i] *= degPerRad;
    if (printGyro)
      print6Axis();
    exceptions = fabs(ypr[2]) > 90;// && aaReal.z < 0; //the second condition is used to filter out some noise
    //however, its change is very slow.
    return true;
  }
  return false;
}

void readEnvironment() {
  if (checkGyro && !(frame % IMU_SKIP))
    read_IMU();
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

//This function will write a 2-byte integer to the EEPROM at the specified address and address + 1

void imuSetup() {
  wdt_enable(WDTO_2S);
  do {
    delay(100);
    // initialize device
    PTLF("Init IMU");
    mpu.initialize();
    //    pinMode(GYRO_PIN, INPUT);
    // verify connection
    //    PTLF("Test connection");
  } while (!mpu.testConnection());
  //  PTLF("Connected");

  // load and configure the DMP
  //  PTLF("Init DMP");
  devStatus = mpu.dmpInitialize();
  wdt_disable();

  // supply the gyro offsets here, scaled for min sensitivity
  for (byte axis = 0; axis < 6; axis++) {
    imuOffset[axis] = EEPROMReadInt(MPUCALIB + axis * 2);
    if (axis < 3)
      mpu.setAccelOffset(axis, imuOffset[axis]);
    else
      mpu.setGyroOffset(axis, imuOffset[axis]);
    }
  //  mpu.setXAccelOffset(imuOffset[0]);
  //  mpu.setYAccelOffset(imuOffset[1]);
  //  mpu.setZAccelOffset(imuOffset[2]);  //gravity
  //  mpu.setXGyroOffset(imuOffset[3]);   //yaw
  //  mpu.setYGyroOffset(imuOffset[4]);   //pitch
  //  mpu.setZGyroOffset(imuOffset[5]);   //roll

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // Calibration Time: generate offsets and calibrate our MPU6050
#ifndef MAIN_SKETCH
    beep(10);
#ifndef AUTO_INIT
    PTLF("\nLay the robot/board FLAT on a table. Calibrate IMU?(Y/n):");
    char choice = getUserInputChar();
    PTL(choice);
    if (choice == 'Y' || choice == 'y') {
#endif
      beep(15, 400, 600);
      PTLF("Calibrating...");
      mpu.CalibrateAccel(10);
      mpu.CalibrateGyro(10);
      for (byte axis = 0; axis < 6; axis++) {
        if (axis < 3)
          EEPROMWriteInt(MPUCALIB + axis * 2, mpu.getAccelOffset(axis));
        else
          EEPROMWriteInt(MPUCALIB + axis * 2, mpu.getGyroOffset(axis));
      }
      //      EEPROMWriteInt(MPUCALIB, mpu.getXAccelOffset());
      //      EEPROMWriteInt(MPUCALIB + 2, mpu.getYAccelOffset());
      //      EEPROMWriteInt(MPUCALIB + 4, mpu.getZAccelOffset());
      //      EEPROMWriteInt(MPUCALIB + 6, mpu.getXGyroOffset());
      //      EEPROMWriteInt(MPUCALIB + 8, mpu.getYGyroOffset());
      //      EEPROMWriteInt(MPUCALIB + 10, mpu.getZGyroOffset());

#ifndef AUTO_INIT
    }
#endif
//    printEEPROM();
    beep(18, 50, 70, 6);
#endif
    //    mpu.PrintActiveOffsets(); //it takes 7% flash!
    // turn on the DMP, now that it's ready
    //    PTLF("Enable DMP");
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    //    PTL("Enable interrupt");
    //    attachInterrupt(digitalPinToInterrupt(GYRO_PIN), dmpDataReady, RISING);
    //    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //    PTLF("DMP ready!");
    dmpReady = true;
    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    PTF("DMP failed (code ");
    PT(devStatus);
    PTL(')');
  }
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
/*
  void imuExample() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;
  // read a packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet
  #ifdef OUTPUT_READABLE_QUATERNION
    // display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    PT("quat\t");
    PT(q.w);
    PT("\t");
    PT(q.x);
    PT("\t");
    PT(q.y);
    PT("\t");
    PTL(q.z);
  #endif

  #ifdef OUTPUT_READABLE_EULER
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetEuler(euler, &q);
    PT("euler\t");
    PT(euler[0] * degPerRad);
    PT("\t");
    PT(euler[1] * degPerRad);
    PT("\t");
    PTL(euler[2] * degPerRad);
  #endif

  #ifdef OUTPUT_READABLE_YAWPITCHROLL
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    PT("ypr\t");
    PT(ypr[0] );
    PT("\t");
    PT(ypr[1] );
    PT("\t");
    PTL(ypr[2] );

    mpu.dmpGetAccel(&aa, fifoBuffer);
    PT("\tRaw Accl XYZ\t");
    PT(aa.x);
    PT("\t");
    PT(aa.y);
    PT("\t");
    PT(aa.z);
    mpu.dmpGetGyro(&gy, fifoBuffer);
    PT("\tRaw Gyro XYZ\t");
    PT(gy.x);
    PT("\t");
    PT(gy.y);
    PT("\t");
    PT(gy.z);

  #endif

  #ifdef OUTPUT_READABLE_REALACCEL
    // display real acceleration, adjusted to remove gravity
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    PT("areal\t");
    PT(aaReal.x);
    PT("\t");
    PT(aaReal.y);
    PT("\t");
    PTL(aaReal.z);
  #endif

  #ifdef OUTPUT_READABLE_WORLDACCEL
    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    PT("aworld\t");
    PT(aaWorld.x);
    PT("\t");
    PT(aaWorld.y);
    PT("\t");
    PTL(aaWorld.z);
  #endif
  }
  }*/
