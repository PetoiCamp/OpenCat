/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald

 modified for the ESP32 on March 2017
 by John Bennett

 see http://www.arduino.cc/en/Tutorial/Sweep for a description of the original code

 * Different servos require different pulse widths to vary servo angle, but the range is 
 * an approximately 500-2500 microsecond pulse every 20ms (50Hz). In general, hobbyist servos
 * sweep 180 degrees, so the lowest number in the published range for a particular servo
 * represents an angle of 0 degrees, the middle of the range represents 90 degrees, and the top
 * of the range represents 180 degrees. So for example, if the range is 1000us to 2000us,
 * 1000us would equal an angle of 0, 1500us would equal 90 degrees, and 2000us would equal 1800
 * degrees.
 * 
 * Circuit: (using an ESP32 Thing from Sparkfun)
 * Servo motors have three wires: power, ground, and signal. The power wire is typically red,
 * the ground wire is typically black or brown, and the signal wire is typically yellow,
 * orange or white. Since the ESP32 can supply limited current at only 3.3V, and servos draw
 * considerable power, we will connect servo power to the VBat pin of the ESP32 (located
 * near the USB connector). THIS IS ONLY APPROPRIATE FOR SMALL SERVOS. 
 * 
 * We could also connect servo power to a separate external
 * power source (as long as we connect all of the grounds (ESP32, servo, and external power).
 * In this example, we just connect ESP32 ground to servo ground. The servo signal pins
 * connect to any available GPIO pins on the ESP32 (in this example, we use pin 18.
 * 
 * In this example, we assume a Tower Pro MG995 large servo connected to an external power source.
 * The published min and max for this servo is 1000 and 2000, respectively, so the defaults are fine.
 * These values actually drive the servos a little past 0 and 180, so
 * if you are particular, adjust the min and max values to match your needs.
 */

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
#define SERVOMIN 150  // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600  // this is the 'maximum' pulse length count (out of 4096)
int range = SERVOMAX - SERVOMIN;
// 16 servo objects can be created on the ESP32

int pos = 0;  // variable to store the servo position

int amplitude = 10;

byte pwm_pin[16] = { 12, 11, 4, 3,
                     13, 10, 5, 2,
                     14, 9, 6, 1,
                     15, 8, 7, 0 };

int baseShift = 30;
int zero[] = {
  90,
  90,
  90,
  90,

  90,
  90,
  90,
  90,

  90,
  90,
  90,
  90,
};
int shift[] = {
  0,
  0,
  0,
  0,
  baseShift,
  -baseShift,
  baseShift,
  -baseShift,
  baseShift,
  -baseShift,
  -baseShift,
  baseShift,
};

int rest[] = {
  0,
  0,
  0,
  0,
  45,
  -45,
  -45,
  45,
  45,
  -45,
  -45,
  45,
};
int stand[] = {
  0,
  0,
  0,
  0,
  30,
  -30,
  -30,
  30,
  -15,
  15,
  15,
  -15,
};


void testPosture() {
  for (byte s = 0; s < 16; s++)
    pwm.setPWM(s, 0, zero[s] + shift[s]);  // tell servo to go to position in variable 'pos'
  delay(2000);
  for (byte s = 0; s < 16; s++)
    pwm.setPWM(s, 0, zero[s] + shift[s] + stand[s]);  // tell servo to go to position in variable 'pos'
  delay(2000);
  for (byte s = 0; s < 16; s++)
    pwm.setPWM(s, 0, zero[s] + shift[s] + rest[s]);  // tell servo to go to position in variable 'pos'
  delay(2000);
  // for (byte s = 0; s < 12; s++)
  //   pwm.setPWM(n, 0, stand[s]);  // tell servo to go to position in variable 'pos'
  // delay(1000);
}

void testGait() {
  for (pos = -amplitude; pos <= +amplitude; pos += 1) {
    for (byte s = 0; s < 16; s++)
      pwm.setPWM(s, 0, pos * (s % 4 / 2 ? 1 : -1) * (s > 7 ? 1 : 1) + zero[s] + shift[s] + stand[s]);  // tell servo to go to position in variable 'pos'
    delay(10);                                                                                         // waits 15ms for the servo to reach the position
  }
  for (pos = +amplitude; pos >= -amplitude; pos -= 1) {
    for (byte s = 0; s < 16; s++)
      pwm.setPWM(s, 0, pos * (s % 4 / 2 ? 1 : -1) * (s > 7 ? 1 : 1) + zero[s] + shift[s] + stand[s]);  // tell servo to go to position in variable 'pos'
    delay(10);                                                                                         // waits 15ms for the servo to reach the position
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
    float duty = SERVOMIN + range / 2;
    pwm.setPWM(pwm_pin[servonum], 0, duty);  //duty);
    pwm.setPWM(pwm_pin[servonum], 0, 4096);  //duty);
  }
  delay(100);
}

void loop() {
  testGait();
}
