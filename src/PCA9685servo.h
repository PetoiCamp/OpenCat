/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These drivers use I2C to communicate, 2 pins are required to
  interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/*
  Revised for conveniently writing angles and setting different servos with different parameters.

  Rongzhong Li
  Feb.2, 2022
*/
#include "Adafruit-PWM-Servo-Driver-Library/Adafruit_PWMServoDriver.h"

#define P_STEP 32
#define P_BASE 3000 + P_STEP * 4  //3128  range: 3000~3320 
#define P_HARD (P_BASE + P_STEP * 1)
#define P_SOFT (P_BASE - P_STEP * 2)

#define SERVO_FREQ 240
#ifndef PWM_READ_PIN
#define PWM_READ_PIN A3
#endif

#define F_MIN 23000
#define F_MAX 27500
#define MATCHING_TIMES 2
#define COUNT_TIMES 7

bool calibrated = false;
int lastValue = 0;

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

class ServoModel {
public:
  ServoModel(int range, int freq, int min, int max)
    : angleRange(range), frequency(freq), minPulse(min), maxPulse(max) {}
  int angleRange;
  int frequency;
  int minPulse;
  int maxPulse;
};

class Petoi_PWMServoDriver : public Adafruit_PWMServoDriver {
private:
#ifndef MAIN_SKETCH
  const static int SERVOMIN = 150;     // This is the 'minimum' pulse length count (out of 4096)
  const static int SERVOMAX = 600;     // This is the 'maximum' pulse length count (out of 4096)
  const static int USMIN = 600;        // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
  const static int USMAX = 2400;       // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
  const static int DEFAULT_FREQ = 50;  // Analog servos run at ~50 Hz updates
  const static int ANGLE_RANGE = 180;
#endif
  const static long OSCILLATOR_FREQ = 25000000;
public:
  int servoPWMFreq;
  long oscillatorFreq;
  float k_us2pulse;
  float b_offset;
  float *k_angle2pulse;

  using Adafruit_PWMServoDriver::Adafruit_PWMServoDriver;

  void setup(int8_t numServo = DOF, long oscillatorFreq = OSCILLATOR_FREQ) {

    setPWMFreq(SERVO_FREQ);
    this->oscillatorFreq = oscillatorFreq;
    setOscillatorFrequency(oscillatorFreq);

#ifndef MAIN_SKETCH
    servoPWMFreq = SERVO_FREQ;
    b_offset = servoPWMFreq / DEFAULT_FREQ * SERVOMIN;
    EEPROM.update(B_OFFSET, byte(b_offset / 10));
    delay(5);
    k_us2pulse = microsecondsToPulse(1);
    //pulse = angle * k_angle2pulse + b_offset
    k_angle2pulse = new float[numServo];
    //servo parameters
    ServoModel servoG41(180, SERVO_FREQ, 500, 2500);
    ServoModel servoP1S(270, SERVO_FREQ, 500, 2500);  //1s/4 = 250ms 250ms/2500us=100Hz
    for (int s = 0; s < numServo; s++) {
      ServoModel *model;
      switch (servoModelList[s]) {
        case G41:
          model = &servoG41;
          break;
        case P1S:
          model = &servoP1S;
          break;
      }

      zeroPosition[s] = model->angleRange / 2 + float(eeprom(MID_SHIFT, s)) * eeprom(ROTATION_DIRECTION, s);
      calibratedZeroPosition[s] = zeroPosition[s] + float(eeprom(CALIB, s)) * eeprom(ROTATION_DIRECTION, s);
      EEPROMWriteInt(ZERO_POSITIONS + s * 2, zeroPosition[s]);
      EEPROMWriteInt(CALIBRATED_ZERO_POSITIONS + s * 2, calibratedZeroPosition[s]);
      EEPROMWriteInt(ANGLE2PULSE_FACTOR + s * 2, int(calcAngleToPulseFactor(s, model->angleRange, model->minPulse, model->maxPulse) * 1000));
    }
#endif
  }
#ifndef MAIN_SKETCH
  float microsecondsToPulse(uint16_t microseconds) {
    float pulse = microseconds;
    float pulselength;
    pulselength = 1000000;  // 1,000,000 us per second
    // Read prescale
    uint16_t prescale = readPrescale();
    // Calculate the pulse for PWM based on Equation 1 from the datasheet section 7.3.5
    prescale += 1;
    pulselength *= prescale;
    pulselength /= oscillatorFreq;
    pulse /= pulselength;
    return pulse;
  }

  float calcAngleToPulseFactor(uint8_t servoNum, int angleRange = ANGLE_RANGE, int usMin = USMIN, int usMax = USMAX) {
    //  angle *  ______________k_angle2pulse______________ + _______________b_offset_______________
    // (angle * (usMax - usMin) / angleRange) * k_us2pulse + servoPWMFreq / DEFAULT_FREQ * SERVOMIN;
    k_angle2pulse[servoNum] = (usMax - usMin) / angleRange * k_us2pulse;
    return k_angle2pulse[servoNum];
    //      Serial.print(servoNum); Serial.print('\t');
    //      Serial.println(k_angle2pulse[PWM_pin[servoNum]]);
  }
#endif
  void writeAngle(uint8_t servoNum, float angle) {
    int clipped = min(max(EEPROMReadInt(ANGLE2PULSE_FACTOR + servoNum * 2) / 1000.0 * angle + (byte)eeprom(B_OFFSET) * 10, 500), 2500);
    setPWM(eeprom(PWM_PIN, servoNum), 0, clipped);
    //      if (servoNum == 1) {
    //        Serial.print(servoNum); Serial.print('\t');
    //        Serial.print(EEPROMReadInt(ANGLE2PULSE_FACTOR + servoNum * 2) / 1000.0 * angle + (byte)eeprom(B_OFFSET) * 10); Serial.print('\t');
    //        Serial.println(clipped);
    //      }
  }
  void shutServos(byte s0 = 0, byte s1 = DOF) {
    //todo:
    //shutServos() all shut
    //shutServos(x) shut x
    //shutServos(a, b) shut a to b
    for (byte s = s0; s < s1; s++) {
      setPWM(s, 0, 4096);
    }
    servoOff = true;
  }
};

// called this way, it uses the default address 0x40
Petoi_PWMServoDriver pwm = Petoi_PWMServoDriver();
// you can also call it with a different address you want
//Petoi_PWMServoDriver pwm = Petoi_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Petoi_PWMServoDriver pwm = Petoi_PWMServoDriver(0x40, Wire);

void setServoP(unsigned int p) {
  for (byte i = 0; i < 16; i++)
    pwm.writeMicroseconds(i, p);
}
/*
  In theory the internal oscillator (clock) is 25MHz but it really isn't
  that precise. You can 'calibrate' this by tweaking this number until
  you get the PWM update frequency you're expecting!
  The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
  is used for calculating things like writeMicroseconds()
  It is importaint to use an oscilloscope in setting the int.osc frequency
  for the I2C PCA9685 chip.
  1) Attach the oscilloscope to one of the PWM signal pins and ground on
    the I2C PCA9685 chip you are setting the value for.
  2) Enter the actual PWM's pulse-width until it becomes the
    expected value (1500 us).
  Setting the value here is specific to each individual I2C PCA9685 chip and
  affects the calculations for the PWM update frequency.
  Failure to correctly set the int.osc value will cause unexpected PWM results
*/
long initValue;
void PCA9685CalibrationPrompt() {
  PTF("Optional: Connect PWM ");
  PT(eeprom(PWM_PIN, 3));
  PTLF(" -> Grove pin A3 to calibrate PCA9685");
}

int measurePulseWidth(uint8_t pwmReadPin) {
  while (!digitalRead(pwmReadPin))
    ;
  long t1 = micros();
  while (digitalRead(pwmReadPin))
    ;
  return (micros() - t1);
}

byte match = 0;
void calibratePCA9685() {
  delay(10);
  if (!calibrated && !digitalRead(PWM_READ_PIN)) {  //the pins are connected
    //    for (byte i = 0; i < 16; i++)
    pwm.writeMicroseconds(eeprom(PWM_PIN, 3), 1500);
    int actualPulseWidth;
    actualPulseWidth = 0;
    for (int i = 0; i < COUNT_TIMES + 1; i++) {
      delay(5);  //the width is 1500 microsceonds, or 1.5 milliseconds. so the delay should be > 3. use 5 to be safe.
      int oneReading = measurePulseWidth(PWM_READ_PIN);
      if (i > 0)
        actualPulseWidth += oneReading;
    }
    actualPulseWidth /= COUNT_TIMES;
    long actualFreq = round(initValue * 1500 / actualPulseWidth);
    PTL(actualFreq);  //if the loop cannot stop and prints numbers larger than 27000, you may increase F_MAX
    if (actualFreq >= F_MIN && actualFreq <= F_MAX) {
      if (actualFreq == lastValue) {
        match++;
        if (match == MATCHING_TIMES) {
          EEPROMWriteInt(PCA9685_FREQ, actualFreq);
          PTF("Calibrated: ");
          PT(actualFreq);
          PTF(" kHz");
          beep(20, 100, 50, 3);
          calibrated = true;
          pwm.setup(DOF, long(actualFreq) * 1000);
          pwm.writeMicroseconds(eeprom(PWM_PIN, 3), 1500);
        }
      } else
        match = 0;
    }
    lastValue = actualFreq;
  }
}
void servoSetup() {
  PTF("Servo:");
  for (byte i = 0; i < DOF; i++) {
    servoCalib[i] = eeprom(CALIB, i);
  }
  pwm.begin();
  initValue = EEPROMReadInt(PCA9685_FREQ);
  if (initValue < F_MIN || initValue > F_MAX)
    initValue = 25000;
  PTL(float(initValue) / 1000);
  // PTF("MHz");
  pwm.setup(DOF, long(initValue) * 1000);
  pwm.shutServos();
}

void testLoop() {
  for (int an = -180; an < 180; an += 90) {  //0,90,180,270,
    for (int servonum = 0; servonum < 16; servonum++)
      pwm.writeAngle(servonum, an);
    //    calibratedPWM(servonum, an);
    delay(1000);
  }
  for (int an = 180; an > -180; an -= 90) {  //180,90
    for (int servonum = 0; servonum < 16; servonum++)
      pwm.writeAngle(servonum, an);
    //    calibratedPWM(servonum, an);
    delay(1000);
  }
  //  pwm.shutServos();
  //  delay(3000);
}