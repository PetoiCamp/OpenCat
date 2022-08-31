/***************************************************
  PCA9685 frequency calibrator

  In theory, the internal oscillator (clock) is 25MHz, but it really isn't
  that precise. You can 'calibrate' this by tweaking this number until
  you get the PWM update frequency you're expecting!
  The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
  is used for calculating things like writeMicroseconds()
  It is important to use an
  oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
  1) Attach the oscilloscope to one of the PWM signal pins and ground on
    the I2C PCA9685 chip you are setting the value for.
  2) Adjust setOscillatorFrequency() until the PWM update frequency is the
    expected value (50Hz for most ESCs)
  Setting the value here is specific to each individual I2C PCA9685 chip and
  affects the calculations for the PWM update frequency.
  Failure to correctly set the int.osc value will cause unexpected PWM results

  Modified from Adafruit PCA9685 servo example.

  Petoi LLC
  Rongzhong Li
  Jul.15, 2022
****************************************************/

#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ    240 // Analog servos run at ~50 Hz updates
#define PCA9685_FREQ  275 // 2 bytes
#define PT(s) Serial.print(s)  // abbreviate print commands
#define PTL(s) Serial.println(s)
#define PTLF(s) Serial.println(F(s))
long initValue;
int target;
#ifndef PWM_READ_PIN
#define PWM_READ_PIN  A3
#endif

bool calibrated = false;
int lastValue = 0;

void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.update(p_address, lowByte);
  EEPROM.update(p_address + 1, highByte);
  delay(10);
}

//This function will read a 2-byte integer from the EEPROM at the specified address and address + 1
int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void PCA9685CalibrationPrompt() {
  PTLF("\n* PCA9685 Internal Oscillator Frequency Calibrator *\n");
  PTLF("Command list:\n");
  PTLF("AUTO - Automatically calibrate PCA9685's internal oscillator");
    PT("    1. Connect Uno's pin A3 with one of the PWM pins\n");
  PTLF("    2. The calibration will finish automatically\n");
  PTLF("s - Send a PWM signal in microseconds (us) to all the 16 pins");
  PTLF("    e.g. s1500\n");
  PTLF("o - Use an oscilloscope to calibrate PCA9685's internal oscillator");
  PTLF("    1. Attach the oscilloscope between one of the PWM signal pins and GND");
  PTLF("    2. Enter the actual PWM's pulse width reading until it matches the target signal");
  PTLF("    e.g. o1440\n");

  target = 1500;
  for (byte i = 0; i < 16; i++) {
    pwm.writeMicroseconds(i, target);
  }
}

int measurePulseWidth() {
  while (!analogRead(PWM_READ_PIN));
  long t1 = micros();
  while (analogRead(PWM_READ_PIN));
  return (micros() - t1);
}

void calibratePCA9685() {
  if (Serial.available()) {
    char calibrateToken = Serial.read();
    PTL(calibrateToken);
    if (calibrateToken == 's') {
      target = Serial.parseInt();
      for (byte i = 0; i < 16; i++) {
        pwm.writeMicroseconds(i, target);
      }
      PTL("Sent " + String(target) + " us\n");
    }
    else if (calibrateToken == 'o') {
      int actualPulseWidth = Serial.parseInt();
      Serial.print("Measured by the oscillator: ");
      Serial.println(String(actualPulseWidth) + " us");
      long actualFreq = round(initValue * target / actualPulseWidth);
      EEPROMWriteInt(PCA9685_FREQ, actualFreq);
      Serial.println("The PCA9685 has been calibrated as " + String(actualFreq) + " kHz!\n");
      pwm.setOscillatorFrequency(actualFreq * 1000);
      pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
      delay(10);
      for (byte i = 0; i < 16; i++) {
        pwm.writeMicroseconds(i, target);
      }
      initValue = actualFreq;
    }
    delay(10);
  }
  else if (!calibrated && analogRead(PWM_READ_PIN) == 0) {//the pins are connected
    //  if (Serial.available() && Serial.read()) {
    //    for (byte i = 0; i < 16; i++)
    pwm.writeMicroseconds(3, 1500);
    delay(5);
    int actualPulseWidth;
    actualPulseWidth = 0;
    for (int i = 0; i < 11; i++) {
      int oneReading = measurePulseWidth();
      if (i > 0)
        actualPulseWidth += oneReading;
    }
    actualPulseWidth /= 10;
    long actualFreq = round(initValue * 1500 / actualPulseWidth);
    if (actualFreq >= 23000 && actualFreq <= 27000) {
      PTL(actualFreq);
      if (actualFreq == lastValue) {//this condition is strong enough to ensure the calibration is correct
        EEPROMWriteInt(PCA9685_FREQ, actualFreq);
        Serial.println("Auto calibrated: " + String(actualFreq) + " kHz");
        calibrated = true;
      }
    }
    lastValue = actualFreq;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2);
  initValue = EEPROMReadInt(PCA9685_FREQ);
  Serial.println("\nInitial value: " + String(initValue));
  if (initValue < 23000 || initValue > 27000) {
    initValue = 25000;
    Serial.println("The PCA9685 has never been calibrated! Using default 25000 KHz.");
  }
  else
    Serial.println("The PCA9685 has been calibrated as " + String(initValue) + " kHz.");
  pwm.begin();
  pwm.setOscillatorFrequency(initValue * 1000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  PCA9685CalibrationPrompt();
}

void loop() {
  // Drive each servo one at a time using setPWM()
  calibratePCA9685();
  delay(10);
}
