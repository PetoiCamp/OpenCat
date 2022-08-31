/*
  Keyboard

  Plays a pitch that changes based on a changing analog input

  circuit:
  - three force-sensing resistors from +5V to analog in 0 through 5
  - three 10 kilohm resistors from analog in 0 through 5 to ground
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneKeyboard
*/

#include "pitches.h"

const int threshold = 10;    // minimum reading of the sensors that generates a note
#define BUZZ_PIN 5
// notes to play, corresponding to the 3 sensors:
int notes[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
};

int pins[] = {6, 7, 8, 9};
void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int thisSensor = 0; thisSensor < 2; thisSensor++) {
    // get a sensor reading:
    int sensorReading = digitalRead(pins[thisSensor]);
    Serial.print(sensorReading);
    Serial.print('\t');
    // if the sensor is pressed hard enough:
    if (sensorReading) {
      // play the note corresponding to this sensor:
      tone(BUZZ_PIN, notes[thisSensor], 20);
    }
  }
  Serial.println();
}
