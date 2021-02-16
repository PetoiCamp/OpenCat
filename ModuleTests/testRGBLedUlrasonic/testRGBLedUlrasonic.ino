//code provided by the manufacturer
//modified by Rongzhong Li for better demonstration.
//Feb.16, 2021

#include "RgbUltrasonic.h"

RgbUltrasonic mRUS04(6, 7);//(signal, RGB)
//The RGBLED module should be plugged in teh fourth grove socket with D6, D7

#define BUZZER 5 // the PWM pin the ACTIVE buzzer is attached to
void beep(int note, float duration = 10, int pause = 0, byte repeat = 1 ) {
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
      analogWrite(BUZZER, 100);      // Almost any value can be used except 0 and 255. it can be tuned as the amplitude of the note
      // experiment to get the best tone
      delayMicroseconds(period);          // wait for a delayms ms
      analogWrite(BUZZER, 0);       // 0 turns it off
      delayMicroseconds(period);          // wait for a delayms ms
    }
    delay(pause);
  }
}

int interval = 3;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
}
void loop() {
  int distance = mRUS04.GetUltrasonicDistance();

  if (distance > 50) {
    mRUS04.SetRgbEffect(E_RGB_ALL, RGB_WHITE, E_EFFECT_BREATHING);
  }
  else {
    beep(50 - distance / interval * 2 , 20 + distance / 2, 5 + distance );
    Serial.print(50 - distance / interval * 2 );
    Serial.print("\t" );
    Serial.print(20 +  distance / 2 );
    Serial.print("\t");
    Serial.println(5 + distance );
    if (distance < 2) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_RED, E_EFFECT_FLASH);
    }
    else if (distance < 10 + interval) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_RED);
    }
    else if (distance < 10 + interval * 3) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_BLUE);
    }
    else if (distance < 10 + interval * 5) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_GREEN);
    }
    else if (distance < 10 + interval * 7) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_YELLOW);
    }
    else if (distance < 10 + interval * 9) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_PURPLE);
    }
    else if (distance < 10 + interval * 11) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_WHITE);
    }
    else {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_YELLOW, E_EFFECT_ROTATE);
    }
  }
}
