/*
   Rongzhong Li
   January 2021

   Copyright (c) 2021 Petoi LLC.
   All rights reserved
*/

//abbreviations
#define PT(s) Serial.print(s)  //makes life easier
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))//trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))

#define BATT A7
#include "IRremote.h"
#define IR 4  // signal pin of IR receiver to Arduino pin 4
/*-----( Declare objects )-----*/
IRrecv irrecv(IR);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

#define BUZZER 5 // the ACTIVE buzzer is attached to PWM pin 5

void beep(int note = 1, float duration = 10, int pause = 0, byte repeat = 1 ) {
  if (note == 0) {
    analogWrite(BUZZER, 0);
    delay(duration);
    return;
  }
  int freq = 220 * pow(1.059463, note - 1); // 220 is note A3
  //1.059463 comes from https://en.wikipedia.org/wiki/Twelfth_root_of_two
  float period = 1000000.0 / freq / 2.0;
  for (byte r = 0; r < repeat; r++) {
    for (float t = 0; t < duration * 1000; t += period * 2) {
      analogWrite(BUZZER, 150);      // Almost any value can be used except 0 and 255. it can be tuned as the amplitude of the note
      // experiment to get the best tone
      delayMicroseconds(period);          // wait for a delayms ms
      analogWrite(BUZZER, 0);       // 0 turns it off
      delayMicroseconds(period);          // wait for a delayms ms
    }
    delay(pause);
  }
}
void meow(int repeat = 1, int pause = 200, int startF = 150,  int endF = 255, int increment = 5) {
  for (int r = 0; r < repeat; r++) {
    for (int amp = startF; amp <= endF; amp += increment) {
      analogWrite(BUZZER, amp);
      delay(30); // wait for 30 milliseconds to allow the buzzer to vibrate
    }
    delay(500);
    analogWrite(BUZZER, 0);
    delay(pause);
  }
}

char translateIR() {// takes action based on IR code received
  switch (results.value) {
    case 0xFF6897: PTLF(" 0");            return ('0');       //stop
    case 0xFF9867: PTLF(" group");        return ('G');       //switch between 8 or 16 servos
    case 0xFFB04F: PTLF(" loop");         return ('L');       //loop through individual servos

    case 0xFF30CF: PTLF(" 1");            return ('1');       //speed 1
    case 0xFF18E7: PTLF(" 2");            return ('2');       //      2
    case 0xFF7A85: PTLF(" 3");            return ('3');       //      3

    case 0xFF10EF: PTLF(" 4");            return ('4');       //      4. faster speed will be unstable for running all 16 metal servos
    case 0xFF38C7: PTLF(" 5");            return ('5');       //      5
    case 0xFF5AA5: PTLF(" 6");            return ('6');       //      6

    case 0xFF42BD: PTLF(" 7");            return ('7');       //      7
    case 0xFF4AB5: PTLF(" 8");            return ('8');       //      8
    case 0xFF52AD: PTLF(" 9");            return ('9');       //      9

    case 0xFFFFFFFF: return ('?'); //Serial.println(" REPEAT");
    default:
      Serial.print("other button");
      Serial.print("\t");
      Serial.println(results.value, HEX);
      return ('?');
  }// End Case
  delay(500); // Do not get immediate repeat
}

char getUserInput() { //limited to one character
  char in;
  while (!Serial.available());
  do {
    in = Serial.read();
    //Serial.print("\"");
    Serial.print(in);
    /*Serial.print("\"");
      Serial.print(", Ascii#: ");
      Serial.println((int)in);*/
    delay(5);
  }
  while (Serial.available()) ;
  return in;
}

#include <Wire.h>
#define DEVICE_ADDRESS 0x54    //Address of 24LC256 eeprom chip
#define WIRE_BUFFER 30 //Arduino wire allows 32 byte buffer, with 2 byte for address.
#define WIRE_LIMIT 16 //That leaves 30 bytes for data. use 16 to balance each writes
#define PAGE_LIMIT 32 //AT24C32D 32-byte Page Write Mode. Partial Page Writes Allowed
#define SIZE 65536/8
#define START 65536/8-300
#define WRITE

byte i2c_eeprom_read_byte( unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(DEVICE_ADDRESS, 1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void writeLong(unsigned int &eeAddress, char *data, int len) {
  //byte locationInPage = eeAddress % PAGE_LIMIT;
  PTL("Write " + String(len) + " bytes");
  int writtenToEE = 0;
  while (len > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((eeAddress) >> 8));   // MSB
    Wire.write((int)((eeAddress) & 0xFF)); // LSB
    byte writtenToWire = 0;
    do
    {
      if (eeAddress == SIZE) {
        PTL();
        PTL("EEPROM overflow!\n");
      }
      Wire.write((byte)data[writtenToEE]);
      writtenToWire++;
      writtenToEE++;
      eeAddress++;
      len--;
    } while (len > 0 && (eeAddress  % PAGE_LIMIT ) && writtenToWire < WIRE_LIMIT);
    Wire.endTransmission();
    delay(6);  // needs 5ms for page write
  }
  PTL("Finish writing");
}

void readLong(unsigned int &eeAddress, char *data) {
  int len = i2c_eeprom_read_byte(eeAddress);
  data[len] = '\0';
  PTL("Read " + String(len) + " bytes");
  int readFromEE = 0;
  int readToWire = 0;

  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write((int)((eeAddress) >> 8));   // MSB
  Wire.write((int)((eeAddress) & 0xFF)); // LSB
  Wire.endTransmission();
  while (len > 0) {
    Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
    readToWire = 0;
    do {
      if (Wire.available()) data[readFromEE] = Wire.read();
      readFromEE++;
      //eeAddress++;
    } while (--len > 0 && ++readToWire < WIRE_BUFFER);
  }
  PTL("Finish reading");
}

#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
int range = SERVOMAX - SERVOMIN;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>


#define PACKET_SIZE 42
#define OVERFLOW_THRESHOLD 128

//#if OVERFLOW_THRESHOLD>1024-1024%PACKET_SIZE-1   // when using (1024-1024%PACKET_SIZE) as the overflow resetThreshold, the packet buffer may be broken
// and the reading will be unpredictable. it should be replaced with previous reading to avoid jumping
#define FIX_OVERFLOW
//#endif
#define HISTORY 2
int8_t lag = 0;
float ypr[3];         // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float yprLag[HISTORY][2];

MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[PACKET_SIZE]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
float degPerRad = 180 / M_PI;
float radPerDeg = M_PI / 180;
void checkBodyMotion()  {
  if (!dmpReady) return;
  // wait for MPU interrupt or extra packet(s) available
  //while (!mpuInterrupt && fifoCount < packetSize) ;
  if (mpuInterrupt || fifoCount >= packetSize)
  {
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    //PTL(fifoCount);
    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount > OVERFLOW_THRESHOLD) { //1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      // otherwise, check for DMP data ready interrupt (this should happen frequently)

      // -- RzLi --
#ifdef FIX_OVERFLOW
      PTLF("FIFO overflow! Using last reading!");
      lag = (lag - 1 + HISTORY) % HISTORY;
#endif
      // --
    }
    else if (mpuIntStatus & 0x02) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);

      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;

#ifdef OUTPUT_READABLE_YAWPITCHROLL
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

#endif
      PT(ypr[1] * degPerRad);
      PTF("\t");
      PTL(ypr[2] * degPerRad);
      // overflow is detected after the ypr is read. it's necessary to keep a lag recrod of previous reading.  -- RzLi --
#ifdef FIX_OVERFLOW
      for (byte g = 0; g < 2; g++) {
        yprLag[lag][g] = ypr[g + 1] * degPerRad;
        ypr[g + 1] = yprLag[(lag - 1 + HISTORY) % HISTORY][g] * radPerDeg;
      }
      lag = (lag + 1) % HISTORY;
#endif
    }
  }
}


int loopCounter = 0;
String tests[] = {
  "Check servo driver",
  "Check MPU6050",
  "Check serial monitor and buzzer",
  "Check I2C EEPROM",
  "Check I2C connection with RasPi",
  "Check serial connection with RasPi",
};
byte totalTests = sizeof(tests) / 6;
bool *passed = new bool[totalTests];
// our servo # counter
bool skip = false;
int num;
float t = 0;
float tStep = 5;
int i0 = 0, it = 16;
int single = 0;
int group = 0;
int8_t dly = 0;
void setup()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //Wire.setClock(400000);
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
  Serial.begin(115200);
  while (!Serial);
  delay(5);
  PTL();
  pinMode(BATT, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  randomSeed(analogRead(0));

  PTLF("Connecting MPU6050...");
  mpu.initialize();
  //do
  {
    delay(500);
    // verify connection
    PTLF("Testing connections...");
    PTL(mpu.testConnection() ? F("MPU successful") : F("MPU failed"));//sometimes it shows "failed" but is ok to bypass.
  } //while (!mpu.testConnection());

  // load and configure the DMP
  do {
    PTLF("Initializing DMP...");
    devStatus = mpu.dmpInitialize();
    delay(500);
    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      PTLF("Enabling DMP...");
      mpu.setDMPEnabled(true);

      // enable Arduino interrupt detection
      PTLF("Enabling interrupt detection");
      attachInterrupt(0, dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      PTLF("DMP ready!");
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      PTLF("DMP failed (code ");
      PT(devStatus);
      PTLF(")");
      PTL();
    }
  } while (devStatus);

  PTLF("\n** NyBoard Test Sketch **");

  /*PTLF("If you cannot see this sentence on serial monitor");
    PTLF("\tThere's something wrong with your Arduino IDE's configuration.");
    PTLF("\tMake sure that you have burned the bootloader with the 20MHz hex file,");
    PTLF("\tand selected the 20MHz board when uploading sketch.");
    PTLF("\tThe baudrate of serial monitor is 57600");*/
  PTF("The sketch will check ");
  PT(totalTests);
  PTLF(" items.");
  PTL();
  irrecv.enableIRIn(); // Start the receiver
  for (byte i = 0; i < totalTests; i++)
    passed[i] = false;

  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  meow(1, 100);
}

void loop() {
  if (loopCounter >= totalTests) {
    if (loopCounter == totalTests) {
      byte totalPass = 0;
      PTL("Test summary:");
      PT("   ");
      for (byte i = 0; i < totalTests; i++) {
        PT(passed[i]);
        PT(" ");
        totalPass += passed[i];
      }
      PTL();
      PT(totalPass);
      PTLF(" tests passed");
      if (totalPass == totalTests)
        meow();
      else
        beep(5, 1000);
      loopCounter = totalTests + 1;

    }
    delay(1000);
  }
  else {
    PT(loopCounter + 1);
    PT(". ");
    PTL(tests[loopCounter++]);
    switch (loopCounter) {
      case 1: {
          PTLF("Power the board with two Li-ion batteries in series (6~8.4V). Check:");
          PTLF("   a. Both of the LEDs should lit up.");
          PTLF("   b. Dial the potentiometer to make the voltage over servos around 6V.");
          PTLF("   c. Plug 16 servos to the PWM pins. Allow the servos rotate without colliding into each other");
          PTLF("\tuse IR remote. Only location of the keys matter.");
          PTL();
          PTLF("\to  o  o\n\to  o  o\n\to  o  o\n\t0  G  L\n\t1  2  3\n\t4  5  6\n\t7  8  9");
          PTL();
          PTLF("\t'S' for switching between 8/16 servos. 'L' for looping trhough individual servo.");
          PTLF("\t'0' for stop. Other numbers for speed. When driving all 16 servos, use speed < 5.");
          PTLF("   Do they rotate? (Y/n)");
          int t = 0;
          while (!Serial.available()) {
            if (irrecv.decode(&results)) // have we received an IR signal?
            {
              char in = translateIR();
              PTL(in);
              irrecv.resume(); // receive the next value
              if (in != '?')
                if (in == '0') {
                  for (int servonum = 0; servonum < 16; servonum++)
                    pwm.setPWM(servonum, 0, 4096);
                  skip = true;

                }
                else {
                  dly = 0;
                  skip = false;
                  if (in == 'G') {
                    switch (group) {
                      case 0: i0 = 0; it = 8; break;
                      case 1: i0 = 8; it = 16; break;
                      case 2: i0 = 0; it = 16; break;
                    }
                    group = (group + 1) % 3;
                  }
                  else if (in == 'L') {
                    i0 =  single++ % 16;
                    it = i0 + 1;
                    PT("move ");
                    PTL(i0);
                    dly = 2;
                    tStep = 1;
                  }
                  else {
                    tStep = in - '0';
                  }
                }
            }
            if (skip)
              delay(100);
            else
            {
              for (int servonum = i0; servonum < it; servonum++) {
                float duty =  (sin(2 * 3.14159 * t / 360.0 + (servonum / 2 * 3.14159 / 4))) * range*3/4;/// 2 ;
                pwm.setPWM(servonum, 0, SERVOMIN + range / 2 + duty);
                delayMicroseconds(500);
                PT(duty);
                PT('\t');
              }
              PTL();
              delay(dly);
              t += (tStep*2);
              if (t > 360)
                t = 0;
            }
          }
          char in = Serial.read();
          if (in == 'Y')
            passed[loopCounter - 1] = true;
          for (int servonum = 0; servonum < 16; servonum++)
            pwm.setPWM(servonum, 0, 4096);

          break;
        }

      case 2: {
          PTLF("You will read the rotation angles from MPU6050. ");
          PTLF("Rotate NyBoard slightly from level position.");
          PTLF("   If the readings change with rotation and are close to 0, enter \"Y\"");
          PTLF("   Otherwise, enter \"n\" ");
          PTLF("Enter any character to start the test...");
          getUserInput();

          char in;
          while (!Serial.available()) {
            checkBodyMotion();
            delay(2);
          }
          do {
            in = Serial.read();
            //Serial.print("\"");
            Serial.print(in);
            /*Serial.print("\"");
              Serial.print(", Ascii#: ");
              Serial.println((int)in);*/
            delay(5);
          }
          while (Serial.available()) ;
          if (in == 'Y')
            passed[loopCounter - 1] = true;
          break;
        }
      /*case 3: {//reset button
          PTLF("Press the reset button. Does the sketch restart? (Y/n)");
          if (getUserInput() == 'Y')
            passed[loopCounter - 1] = true;
          break;
        }*/
      case 3: {//buzzer
          meow(1, 100);
          PTLF("Do you hear a \"Meow\"? (Y/n)");
          if (getUserInput() == 'Y')
            passed[loopCounter - 1] = true;
          break;
        }

      /*case 4: {//IR
          PTLF("You need an Arduino IR remote to finish this test.");
          PTLF("Only location matters in this test.");
          PTLF("Press the numbered buttons on the remote in order");
          PTLF("\to  1  o\n\to  o  o\n\to  o  o\n\to  o  o\n\to  o  o\n\to  o  o\n\to  2  o");

          byte irPressed = 0;
          String s;
          while (1) {
            if (irrecv.decode(&results) ) {
              beep();
              if (translateIR() == "FORWARD") {
                irPressed++;
                break;
              }
              irrecv.resume();
            }
          }
          while (1) {
            if (irrecv.decode(&results) ) {
              beep();
              if (translateIR() == "0") {
                irPressed++;
                break;
              }
              irrecv.resume();
            }
          }
          if (irPressed == 2)
            passed[loopCounter - 1] = true;
          break;
        }*/

      case 4: {//I2C EEPROM
          PTLF("Make sure the I2C jumper (SW2) is at \"Ar\".");
          char data[] = " The quick brown fox jumps over the lazy dog. \
The five boxing wizards jump quickly."; // data to write
          int rnd = random(1000);
          unsigned int eeAddress = START - rnd;
          data[0] = strlen(data);
#ifdef WRITE
          writeLong(eeAddress, data, strlen(data));
#endif
          eeAddress = START - rnd;
          int len = i2c_eeprom_read_byte(eeAddress);
          char *buffer = new char[len + 1];
          readLong(eeAddress, buffer);
          PTL();
          PTL("The text is: ");
          for (int i = 1; i < len; i++)
            PT(buffer[i]);
          PTL();
          if (!strcmp(buffer + 1, &data[1]))
            passed[loopCounter - 1] = true;
          PTL();
          for (int i = 0; i < 20; i++)
            PT("-");
          break;
        }

      case 5: {
          PTL();
          PTLF("   Dial the I2C jumper (SW2) to \"Pi\".");
          PTLF("   Connect the board with Pi through the 2x5 socket.");
          PTLF("   Wait until Pi boots up and open its terminal.");
          PTLF("   Type: \" i2cdetect -y 1 \" in the terminal and press \"Enter\".");
          PTLF("Does it show the I2C devices' address table with 40, 50, 68? (Y/n)");
          if (getUserInput() == 'Y')
            passed[loopCounter - 1] = true;
          break;
        }

      case 6: {
          PTLF("Type \" ./ardSignal.py Y \" in the terminal.");
          if (getUserInput() == 'Y')
            passed[loopCounter - 1] = true;
          break;
        }
      default:
        ;
    }
    beep(loopCounter * 2);
    if (!passed[loopCounter - 1]) {
      PTLF("** TEST FAIL! **");
      beep(5, 500, 100);
    }
    PTL();
    delay(500);
  }
}
