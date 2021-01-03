#include <helper_3dmath.h>


/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 16 servos, one after the other

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

int range = SERVOMAX - SERVOMIN;
// our servo # counter
uint8_t servonum = 0;
int pulselen = 0;
int pulseStep = 1;
int resolution =360;
char num = '0';
int t = 0, tStep = 6;
byte pwmPin[16] =  {0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7, 6, 4, 5, 9, 10};//{ 0, 1, 0, 0,  0, 0,  0, 0, 3, 2,  6, 7,  5, 4,  8, 9};
int middleShifts[16] = {0, 50, 0, 0,
                        -45, -45, -45, -45,
                        0, 0, 0, 0,
                        0, 0, 0, 0
                       };
float mg90s = 180 / 180.0, mg92b =  145 / 180.0;
float rangeRatios[] =  {mg92b, mg90s, mg92b, mg92b,
                        mg92b, mg92b, mg92b, mg92b,
                        mg92b, mg92b, mg92b, mg92b,
                        mg92b, mg92b, mg92b, mg92b
                       };

int rotationDirections[16] = {1, -1, 1, 1,
                              1, -1, 1, -1,
                              1, -1, 1, -1,
                              -1, 1, -1, 1
                             };
int walk[][16] = {
  0,  0,  0,  0, -5,  5, -3,  3, 55, 64, 37, 38, 10, -1,  2, -8,
  0,  0,  0,  0, -5,  4, -3,  3, 58, 50, 33, 49, 10,  1,  2, -6,
  0,  0,  0,  0, -5,  2, -3,  1, 61, 33, 30, 58, 11,  9,  3,  0,
  0,  0,  0,  0, -5,  0, -3,  0, 64, 14, 26, 63, 13, 22,  4, 11,
  0,  0,  0,  0, -5, -3, -3, -2, 66,  7, 22, 63, 15, 28,  6, 18,
  0,  0,  0,  0, -5, -4, -3, -3, 67,  4, 19, 60, 17, 33,  9, 23,
  0,  0,  0,  0, -5, -5, -3, -3, 67,  8, 15, 58, 21, 32, 11, 22,
  0,  0,  0,  0, -5, -5, -3, -3, 67, 15, 11, 57, 24, 27, 15, 17,
  0,  0,  0,  0, -5, -5, -3, -3, 67, 22,  7, 55, 29, 22, 19, 13,
  0,  0,  0,  0, -5, -5, -3, -3, 66, 28,  3, 53, 34, 19, 23, 10,
  0,  0,  0,  0, -4, -5, -2, -3, 70, 34,  2, 50, 31, 16, 20,  7,
  0,  0,  0,  0, -1, -5, -1, -3, 76, 39,  4, 48, 24, 14, 14,  5,
  0,  0,  0,  0,  1, -5,  1, -3, 78, 44, 12, 45, 14, 12,  5,  4,
  0,  0,  0,  0,  4, -5,  2, -3, 76, 49, 23, 42,  4, 11, -4,  3,
  0,  0,  0,  0,  5, -5,  3, -3, 68, 53, 35, 38, -1, 10, -8,  2,
  0,  0,  0,  0,  5, -5,  3, -3, 55, 56, 46, 35,  0, 10, -7,  2,
  0,  0,  0,  0,  3, -5,  2, -3, 39, 60, 55, 32,  6, 11, -2,  3,
  0,  0,  0,  0,  1, -5,  0, -3, 20, 62, 62, 28, 17, 12,  7,  4,
  0,  0,  0,  0, -2, -5, -1, -3,  9, 65, 63, 24, 25, 14, 15,  5,
  0,  0,  0,  0, -4, -5, -2, -3,  4, 66, 61, 21, 32, 16, 22,  7,
  0,  0,  0,  0, -5, -5, -3, -3,  5, 67, 59, 17, 33, 19, 23, 10,
  0,  0,  0,  0, -5, -5, -3, -3, 13, 68, 57, 13, 28, 22, 19, 13,
  0,  0,  0,  0, -5, -5, -3, -3, 19, 67, 55,  9, 24, 27, 14, 17,
  0,  0,  0,  0, -5, -5, -3, -3, 26, 66, 53,  5, 20, 31, 11, 22,
  0,  0,  0,  0, -5, -4, -3, -3, 32, 67, 51,  2, 17, 33,  8, 23,
  0,  0,  0,  0, -5, -3, -3, -2, 37, 73, 49,  3, 14, 27,  6, 17,
  0,  0,  0,  0, -5,  0, -3,  0, 42, 77, 46,  6, 12, 21,  4, 11,
  0,  0,  0,  0, -5,  3, -3,  2, 47, 78, 43, 17, 11,  9,  3,  0,
  0,  0,  0,  0, -5,  4, -3,  3, 52, 73, 40, 29, 10,  1,  2, -6
};

void shutServos() {
  delay(250);
  for (byte i = 0; i < 16; i++) {
    pwm.setPWM(i, 0, 4096);
  }
}
void setup() {
  Serial.begin(115200);
  Serial.println("16 channel Servo test!");

  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  yield();
  for (int servonum = 0; servonum < 16; servonum++) {
    //Serial.println(servonum);
    float duty = SERVOMIN + range / 2 ;
    pwm.setPWM(pwmPin[servonum], 0, 4096);//duty);
  }
  delay(2000);
}

// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit");
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);

}


void loop() {
  // Drive each servo one at a time
  if (Serial.available()) {
    num = Serial.read();
    shutServos();
  }
  if (num == 'a')
    for (int servonum = 0; servonum < 16; servonum++) {
      float duty =  (sin(2 * 3.14159 * t / resolution)) * range / 2 * rotationDirections[servonum] / rangeRatios[servonum];
      pwm.setPWM(servonum, 0, SERVOMIN + range / 2 + duty);
    }
  else {
    int servonum = num - '0';
    //Serial.println(servonum);
    float duty =  (sin(2 * 3.14159 * t / resolution)) * range / 2 * rotationDirections[servonum] / rangeRatios[servonum];
    Serial.print(duty);
    Serial.print("\t");
    Serial.print(SERVOMIN + range / 2 + duty);
    pwm.setPWM(servonum, 0, SERVOMIN + range / 2 + duty);
    Serial.println();
  }

  delay(1);
  //pwm.setPWM(servonum, 0, 4096);

  t += tStep;
  t %= resolution;

}
