// --------------------------------------
// i2c_scanner
//
// Version 1
//    This program (or code that looks like it)
//    can be found in many places.
//    For example on the Arduino.cc forum.
//    The original author is not know.
// Version 2, Juni 2012, Using Arduino 1.0.1
//     Adapted to be as simple as possible by Arduino.cc user Krodal
// Version 3, Feb 26  2013
//    V3 by louarnold
// Version 4, March 3, 2013, Using Arduino 1.0.3
//    by Arduino.cc user Krodal.
//    Changes by louarnold removed.
//    Scanning addresses changed from 0...127 to 1...119,
//    according to the i2c scanner by Nick Gammon
//    http://www.gammon.com.au/forum/?id=10896
// Version 5, March 28, 2013
//    As version 4, but address scans now to 127.
//    A sensor seems to use address 120.
// Version 6, November 27, 2015.
//    Added waiting for the Leonardo serial communication.
//
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//
byte testID = 0;

#include <Wire.h>
#include "Adafruit_NeoPixel.h"
#include "sound.h"

char instruction[] = "---\n1-Touch\t 2-Gesture 3-light\n4-Distns 5-PIR     6-Noise\n*\t *\t   *\n16-Old\t\t\n19-New\t 20-Print  21-Sound\n---";
bool compatible_3V = true;
#include "infrared.h"
#include "pitches.h"
#include "Arduino_APDS9960.h"
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 10
#define LED_COUNT 7  // How many NeoPixels are attached to the Arduino?
#define IR_PIN 4
#define PIR_PIN 8



int digitalPins[] = { 6, 7 };
int analogPins[] = { A2, A3 };
int LightPins = A4;
int VoicePins = A5;
byte pirState = 0;
byte loopCounter = 0;

#define TARGET_ADDRESS 0x39

int notes[] = {
  NOTE_C4,
  NOTE_D4,
  NOTE_E4,
  NOTE_F4,
};


byte touchCounter[2];
byte pirCounter = 0;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:

void indicatePass(int passCase = 0) {
  strip.setPixelColor(testID, strip.Color(0, 255, 0));
  if (passCase == 1)
    strip.setPixelColor(testID + 1, strip.Color(255, 0, 0));  //Conditional pass. The consistency or the device ID doesn't match.
  strip.show();
  if (passCase == 1)
    playMelody(fail, sizeof(fail) / 2);
  if (makeSound)
    playMelody(pass, sizeof(pass) / 2);
  else
    delay(1000);
  delay(100);
}

void indicateFail() {
  strip.setPixelColor(testID, strip.Color(255, 0, 0));
  strip.show();
  if (makeSound)
    playMelody(fail, sizeof(fail) / 2);
  else
    delay(1000);
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Start...");
  Serial.println(instruction);
  pinMode(PIR_PIN, INPUT);
  pinMode(analogPins[0], INPUT);
  pinMode(analogPins[1], INPUT);
  while (!Serial)
    ;                       // Leonardo: wait for serial monitor
  irrecv.enableIRIn();      // Start the receiver
  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  read_infrared();
  if (testID < 6) {
    strip.clear();
    strip.setPixelColor(testID, strip.Color(0, 0, 255));
    if (testID == 2 || testID == 3)
      strip.setPixelColor(4, strip.Color(128, compatible_3V ? 128 : 0, compatible_3V ? 0 : 255));  //新黄 旧紫
    strip.show();
  }
  switch (testID) {
    case 0: testTouch(); break;
    case 1:
      if (!loopCounter)
        testI2Cdevice(TARGET_ADDRESS);
      break;
    case 2: testDoubleAnalog(2); break;  //light
    case 3: testDoubleAnalog(3); break;  //distance
    case 4: testPIR(); break;
    case 5: testNoise(); break;
  }
  loopCounter = (loopCounter + 1) % 10;
}

void testTouch() {
  bool touched = false;
  for (int thisSensor = 0; thisSensor < 2; thisSensor++) {
    // get a sensor reading:
    int sensorReading = digitalRead(digitalPins[thisSensor]);
    if (sensorReading) {
      //       play the note corresponding to this sensor:
      if (makeSound)
        tone(BUZZER, notes[thisSensor], 20);

      touchCounter[thisSensor]++;
      touched = true;
    }
  }
  if (touched) {
    for (int thisSensor = 0; thisSensor < 2; thisSensor++) {
      Serial.print(touchCounter[thisSensor]);
      Serial.print('\t');
    }
    Serial.println();
  }
  if (touchCounter[0] > 4 && touchCounter[1] > 4) {
    indicatePass();
    touchCounter[0] = touchCounter[1] = 0;
  }
  if (touchCounter[0] > 30 || touchCounter[1] > 30) {
    indicateFail();
    touchCounter[0] = touchCounter[1] = 0;
  }
  delay(10);
}

void testPIR() {
  int sensorReading = digitalRead(PIR_PIN);
  if (sensorReading != pirState) {
    if (printValue) {
      Serial.print(sensorReading);
      Serial.print('\t');
      Serial.println(pirCounter);
    }
    pirCounter++;
    pirState = sensorReading;
    if (pirCounter > 1) {
      indicatePass();
      pirCounter = 0;
      pirState = 0;
    }
  }
}
bool maxReading[2], minReading[2];
long consistencyQ = 0;

void testDoubleAnalog(int test) {
  int readMin, readMax, diff;
  if (compatible_3V) {
    readMin = test == 2 ? 25 : 30;
    readMax = test == 2 ? 45 : 500;
    diff = test == 2 ? 50 : 15;
  } else {
    readMin = test == 2 ? 5 : 30;
    readMax = test == 2 ? 60 : 900;
    diff = test == 2 ? 50 : 15;
  }
  int sen[2];
  for (int thisSensor = 0; thisSensor < 2; thisSensor++) {
    // get a sensor reading:
    int sensorReading = sen[thisSensor] = analogRead(analogPins[thisSensor]);
    if (sensorReading < readMin)
      minReading[thisSensor] = true;
    if (sensorReading > readMax)
      maxReading[thisSensor] = true;
    if (printValue) {
      Serial.print(sensorReading);
      Serial.print('\t');
    }
    if (makeSound)
      beep(sensorReading / 20 + 1, 10, 0);
    int brightness = 255 * sensorReading / 1024;
    strip.setPixelColor(thisSensor + 5, strip.Color(brightness, brightness, brightness));
    strip.show();
    delay(10);
  }
  if (printValue)
    Serial.print(sen[0] - sen[1]);
  consistencyQ += (abs(sen[0] - sen[1]) < diff ? 1 : -1);
  if (maxReading[0] && minReading[0] && maxReading[1] && minReading[1]) {
    if (consistencyQ > 0) {
      indicatePass();
    } else {
      indicatePass(1);
    }
    consistencyQ = maxReading[0] = minReading[0] = maxReading[1] = minReading[1] = 0;
  }
  if (printValue)
    Serial.println();
}

void testI2Cdevice(int targetAddress) {
  bool found = false;
  byte error, address;
  int nDevices;

  Serial.println("Scanning  I2C Scanner...");
  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
      if (address == targetAddress)
        found = true;

    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Serial.print(nDevices);
  Serial.println(" devices found\n");
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else if (nDevices > 5)
    Serial.println("More I2C devices found\n");
  else {
    if (found) {
      Serial.println("Target I2C device found!\n");
      if (!APDS.begin()) {
        Serial.println("Error initializing APDS-9960 sensor!");
        indicatePass(1);  //found the gesture but the begin fails
      } else
        indicatePass();
    } else {
      indicateFail();
    }
  }
}

void testNoise() {
  int LightData = analogRead(LightPins);
  int VoiceData = analogRead(VoicePins);

  //  Serial.print(LightData);
  //  Serial.print('\t');
  //  Serial.print(VoiceData);
  //  if (makeSound)
  //    beep(sensorReading / 20 + 1, 10, 0);
  int noise = map((1024 - VoiceData), 0, 1024, 0, 255);
  int brightness = 255 * LightData / 1024;
  //  Serial.print('\t');
  //  Serial.print(brightness);
  strip.setPixelColor(5, strip.Color(noise, noise, noise));
  strip.setPixelColor(6, strip.Color(brightness, brightness, brightness));
  strip.show();
  delay(100);

  // Serial.println();
}
