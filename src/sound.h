#define LENGTH_FACTOR 2
#define BASE_PITCH 1046.50
// tone: pause,1,  2,  3,  4,  5,  6,  7,  1,  2, 3,
// code: 0,    1,  3,  5,  6,  8,  10, 12, 13, 15, 17

byte melodyNormalBoot[] = {8, 13, 10, 13, 8, 5, 8, 3, 5, 8,  //tone
                           4,  4,  8,  8, 4, 8, 8, 8, 8, 4,  //relative duration, 8 means 1/8 note length
                          };
byte melodyInit[] = {15, 12, 15, 12, 15, 12, 8, 0, 10, 13, 12, 10, 15,//tone
                     4,  4,  4,  4,  4,  4,  4, 32, 4, 4,  4,  4,  4,//relative duration, 8 means 1/8 note length
                    };
byte melodyLowBattery[] = {15, 11, 13, 10, 11, //tone
                           4,  4,  4,  4,  4, //relative duration, 8 means 1/8 note length
                          };

void beep(float note, float duration = 50, int pause = 0, byte repeat = 1 ) {
  for (byte r = 0; r < repeat; r++) {
    if (note)
      tone(BUZZER, BASE_PITCH * pow(1.05946, note), duration * LENGTH_FACTOR);
    else
      delay(duration * LENGTH_FACTOR);
    delay(pause * LENGTH_FACTOR);
  }
}

void playMelody(int start) {
  byte len = (byte)EEPROM.read(start) / 2;
  for (int i = 0; i < len; i++)
    beep(EEPROM.read(start - 1 - i), 1000 / EEPROM.read(start - 1 - len - i), 50);
}
//
//void meow(int repeat = 0, int pause = 200, int startF = 50,  int endF = 200, int increment = 5) {
//  for (int r = 0; r < repeat + 1; r++) {
//    for (int amp = startF; amp <= endF; amp += increment) {
//      analogWrite(BUZZER, amp);
//      delay(15); // wait for 15 milliseconds to allow the buzzer to vibrate
//    }
//    delay(100 + 500 / increment);
//    analogWrite(BUZZER, 0);
//    if (repeat)
//      delay(pause * 2);
//  }
//}

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
  for (uint8_t i = startF; i < endF; i++) {
    playTone(i, duration);
  }
}
void soundFallOver() {
  for (int i = 0; i < 2; i++)
    chirp();
}
