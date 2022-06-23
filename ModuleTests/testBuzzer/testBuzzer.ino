/* Customized note and melody player for OpenCat, and MEOW \(=^_^=)/
   It also serves to test the baud rate of the serial monitor and the crystal frequency.
   OpenCat happens to use 57600 baud rate (and a 20MHz crystal on Petoi's dedicated PCB).
   If you cannot see this sentence on the serial monitor,
      there's something wrong with your Arduino IDE's configuration.

   On most Arduino, the PWM pins are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10, and ~11.
   If your buzzer cannot meow(), it's probably a passive buzzer.

   Rongzhong Li
   August 2017

   Copyright (c) 2018 Petoi LLC.

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

#define BUZZER 5 // the PWM pin the ACTIVE buzzer is attached to
#define BASE_PITCH 1046.50

// tone 1 corresponds to A3, tone 2 corresponds to B3, tone 13 corresponds to A4, etc.
// tone: pause,1,  2,  3,  4,  5,  6,  7,  high1,  high2
// code: 0,    1,  3,  5,  6,  8,  10, 12, 13,      15

byte melodyNormalBoot[] = {8, 13, 10, 13, 8, 5, 8, 3, 5, 8, 0, //tone
                           4,  4,  8,  8, 4, 8, 8, 8, 8, 4, 2  //relative duration, 8 means 1/8 note length
                          };
byte melodyInit[] = {15, 12, 15, 12, 15, 12, 8, 10, 13, 12, 10, 15,//tone
                     4,  4,  4,  4,  4,  4,  2, 4, 4,  4,  4,  2,//relative duration, 8 means 1/8 note length
                    };

void beepDIY(int note, float duration = 10, int pause = 0, byte repeat = 1 ) {
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

void beep(float note, float duration = 50, int pause = 0, byte repeat = 1 ) {
  for (byte r = 0; r < repeat; r++) {
    if (note == 0)
      delay(duration);
    else
      tone(BUZZER, BASE_PITCH * pow(1.05946, note), duration);
    delay(pause);
  }
}

void playMelody(byte m[], int len) {
  for (int i = 0; i < len; i++) {
    if (!m[i])
      delay(4000 / m[len + i]);
    else
      tone(BUZZER, BASE_PITCH * pow(1.05946, m[i]), 1000 / m[len + i]);
    delay( 1000 / m[len + i]);
  }
  delay(100);
}

template <typename T> int8_t sign(T val) {
  return (T(0) < val) - (val < T(0));
}

void meow( int startF = 1,  int endF = 25, int duration = 5) {
  int s = sign(endF - startF);
  float increment = 0.1 * s;
  for (float amp = startF; s * amp < s * endF; amp += increment) {
    beep(amp, duration);
  }
}


// play tone on a piezo speaker: tone shorter values produce higher frequencies
//  which is opposite beep() but avoids some math delay - similar to code by Erin Robotgrrl

void playTone(uint16_t tone1, uint16_t duration) {
  if (tone1 < 50 || tone1 > 15000) return; // these do not play on a piezo
  for (long i = 0; i < duration * 1000L; i += tone1 * 2) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(tone1);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(tone1);
  }
}

void chirp(int startF = 180,  int endF = 200, int duration = 10) {  // Bird chirp
  for (uint8_t i = 180; i < 200; i++) {
    playTone(i, duration);
  }
}

void soundFallOver() {
  for (byte m = 35; m >= 5; m -= 5) {
    meow(m, m - 20, m / 5);
    meow(m - 20, m - 5, m / 5);
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  Serial.println("if (you cannot see this sentence on serial monitor)");
  Serial.println("\tthere's something wrong with your Arduino IDE's configuration.");
}

int loopCounter = 0;
void loop() {
  if (loopCounter == 0) {
    for (int i = 0; i < 2; i++)
      chirp();
    beep(1, 500, 500);
    beep(13, 500, 500);
    delay(500);
    playMelody(melodyNormalBoot, sizeof(melodyNormalBoot) / 2);
    playMelody(melodyInit, sizeof(melodyInit) / 2);
    Serial.println(loopCounter++);
    delay(500);
  }
  else if (loopCounter < 2) {
    meow(1, 20);
    delay(500);
    meow(30, 5);
    Serial.println(loopCounter++);
  }
}
