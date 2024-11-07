/*
   The driver for OpenCat, runs on ATmega328P-based NyBoard (as Arduino Uno).
   Compatible with Petoi Nybble, Bittle, and many other 8 or 12 DoF quadruped robots.
   Drives up to 16 PWM servos.

   Rongzhong Li
   Mar 15rd, 2022
   Copyright (c) 2022 Petoi LLC.

   This sketch may also include others' codes under MIT or other open-source licenses.
   Check those licenses in the corresponding module test folders.
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

#define MAIN_SKETCH  //the Petoi App only works when this mode is on
// #define AUTO_INIT //automatically select 'Y' for the reset joint and IMU prompts
// #define DEVELOPER //to print out some verbose debugging data
// it may increase the code size and crash the bootloader.
// make sure you know ISP and how to reset the bootloader!!!

#define BITTLE  //Petoi 9 DOF robot dog: 1x on head + 8x on leg
// #define NYBBLE  //Petoi 11 DOF robot cat: 2x on head + 1x on tail + 8x on leg

// #define NyBoard_V0_1
// #define NyBoard_V0_2
// #define NyBoard_V1_0
// #define NyBoard_V1_1
#define NyBoard_V1_2

// you can also activate the following modes (they will disable the gyro to save programming space)
// allowed combinations: RANDOM_MIND + ULTRASONIC, RANDOM_MIND, ULTRASONIC, VOICE, CAMERA
// #define RANDOM_MIND  //advanced random behaviors. use token 'z' to activate/deactivate
// #define TASK_QUEUE  //allow executing a sequence of tasks, if you enabled the other modules, the task queue will be automatically enabled. \
                    // because it takes up memory, it will be disabled if the GYRO is enabled. See "#undef TASK_QUEUE" under ifdef GYRO
// #define ULTRASONIC      //for Nybble's ultrasonic sensor
// #define VOICE  //Petoi Grove voice module
// #define VOICE_LD3320    //for LD3320 module
// #define PIR             //for PIR (Passive Infrared) sensor
//#define DOUBLE_TOUCH  //for double touch sensor
//#define DOUBLE_LIGHT  //for double light sensor
// #define DOUBLE_INFRARED_DISTANCE  //for double infrared distance sensor
// #define GESTURE  //for Gesture module
// #define CAMERA  //for human body tracking or ball tracking using an intelligent camera
// You need to install https://github.com/mu-opensource/MuVisionSensor3 as a zip library in Arduino IDE.
// Set the four dial switches on the camera as **v ^ v v** (the second switch dialed up to I2C) and connect the camera module to the I2C grove on NyBoard.
// The battery should be turned on to drive the servos.
//
// You can use these 3D printed structures to attach the camera module.
// https://github.com/PetoiCamp/NonCodeFiles/blob/master/stl/MuIntelligentCamera_mount.stl
// https://github.com/PetoiCamp/NonCodeFiles/blob/master/stl/bone.stl
// After uploading the code, you may need to press the reset buttons on the module and then the NyBoard.
// The tracking demo works the best with a yellow tennis ball or some other round objects. Demo: https://www.youtube.com/watch?v=CxGI-MzCGWM
// #define GROVE_SERIAL_PASS_THROUGH  //allow analog/digital read/write GPIO pins through serial protocol
// #define OTHER_MODULES  //uncomment this line to disable the gyroscope code to save programming resources for other modules.
// #define ROBOT_ARM

#define IR_PIN 4  // Signal Pin of IR receiver to Arduino Digital Pin 4
#include "src/OpenCat.h"

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  // while (Serial.available() && Serial.read());  // empty buffer

  // join I2C bus (I2Cdev library doesn't do this automatically)
  //#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
  Wire.setClock(500000L);
  //#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  //  Fastwire::setup(400, true);
  //#endif
  initRobot();
}

void loop() {
#ifdef MAIN_SKETCH
#ifdef GYRO_PIN
  readEnvironment();     //reads the IMU (Inertia Measurement Unit, i.e. acceleration and angles).
                         //May read more sensors in the future
  dealWithExceptions();  //fall over, lifted, etc.
#endif

#ifdef TASK_QUEUE
  if (!tQueue->empty()) {
    tQueue->popTask();
  } else
#endif
  {
    readSignal();  //commands sent by user interfaces and sensors
#ifdef OTHER_MODULES
    otherModule();  //you can create your own code here
                    //or put it in the readSignal() function of src/io.h
#endif
  }
  reaction();  //handle different commands
#ifdef VOLTAGE_DETECTION_PIN
  lowBattery();  //  block the loop if battery is low
  //  can be disabled to save programming space and reduce the low voltage interruptions
#endif
#else
  calibratePCA9685();
#endif
}

#ifdef OTHER_MODULES  //remember to activate the #define OTHER_MODULES at the begining of this code to activate the following section
int prevReading = 0;
void otherModule() {  //this is an example that use the analog input pin A2 as a touch pad
                      //The A2 pin is in the second Grove socket of the NyBoard
  int currentReading = analogRead(A2);
  if (abs(currentReading - prevReading) > 50) {  //filter noise
    PT("Reading on pin A2:\t");
    PTL(currentReading);
    if (currentReading < 100) {  //touch and hold on the A2 pin until the condition is met
      beep(10, 20, 50, 3);       //make sound within this function body
      tQueue->createTask();      //more tokens are defined in OpenCat.h
    } else {
      strcpy(newCmd, "sit");          //load a skill to be processed by the later reaction function
      if (strcmp(lastCmd, newCmd)) {  //won't repeatively load the same skill
        newCmdIdx = 5;
        token = T_SKILL;  //T_SKILL loads a skill
      }
    }
  }
  prevReading = currentReading;
}
#endif
