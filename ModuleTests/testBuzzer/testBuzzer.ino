/* Customized note and melody player for OpenCat, and MEOW \(=^_^=)/
   It also serves to test the baud rate of the serial moniter and the crystal frequency.
   OpenCat happens to use 57600 baud rate (and a 20MHz crystal on Petoi's dedicated PCB).
   If you cannot see this sentence on serial monitor,
      there's something wrong with your Arduino IDE's configuration.

   The beep() is better if played with a passive buzzer
   The meow() can only be played with an active buzzer on PWM pins
   On most Arduino, the PWM pins are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.
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
int loopCounter = 0;
// tone 1 corresponds to A3, tone 2 corresponds to B3, tone 13 corresponds to A4, etc.
// tone: pause,1,  2,  3,  4,  5,  6,  7,  high1,  high2
// code: 0,    1,  3,  5,  6,  8,  10, 12, 13,      15
byte melody[] = {8, 13, 10, 13, 8,  0,  5,  8,  3,  5, 8,//tone
                 8, 8,  32, 32, 8, 32, 32, 32, 32, 32, 8 //relative duration, 8 means 1/8 note length
                };

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
      analogWrite(BUZZER, 150);      // Almost any value can be used except 0 and 255. it can be tuned as the amplitude of the note
      // experiment to get the best tone
      delayMicroseconds(period);          // wait for a delayms ms
      analogWrite(BUZZER, 0);       // 0 turns it off
      delayMicroseconds(period);          // wait for a delayms ms
    }
    delay(pause);
  }
}
void playMelody(byte m[], int len) {
  for (int i = 0; i < len; i++)
    beep(m[i], 1000 / m[len + i], 100);
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

void setup()
{
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  Serial.println("if (you cannot see this sentence on serial monitor)");
  Serial.println("\tthere's something wrong with your Arduino IDE's configuration.");
}

void loop() {
  if (loopCounter == 0) {
    beep(1, 500, 500);
    beep(13, 500, 500);
    playMelody(melody, sizeof(melody) / 2);
    Serial.println(loopCounter++);
    delay(1000);
  }
  else if (loopCounter < 2) {
    meow(1, 1000, 100);
    meow(2, 100);
    Serial.println(loopCounter++);
  }
}
