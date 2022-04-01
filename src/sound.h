#define LENGTH_FACTOR 2
#define BASE_PITCH 1046.50
// tone: pause,1,  2,  3,  4,  5,  6,  7,  1,  2, 3, 4
// code: 0,    8, 10, 12, 13, 15, 17, 19, 20, 22,24,25

byte melodyNormalBoot[] = {8, 13, 10, 13, 8, 5, 8, 3, 5, 8,  //tone
                           4,  4,  8,  8, 4, 8, 8, 8, 8, 4,  //relative duration, 8 means 1/8 note length
                          };
byte melodyInit[] = {15, 12, 15, 12, 15, 12, 8, 0, 10, 13, 12, 10, 15,//tone
                     4,  4,  4,  4,  4,  4,  4, 32, 4, 4,  4,  4,  4,//relative duration, 8 means 1/8 note length
                    };
byte melodyLowBattery[] = {15, 11, 13, 10, 11, //tone
                           4,  4,  4,  4,  4, //relative duration, 8 means 1/8 note length
                          };
byte melody1[] = {15, 8, 10, 12, 13, 15, 8, 8, 0, 17, 13, 15, 17, 19, 20, 8, 8,
                  1, 2,  2,  2,  2,  1, 2, 4, 8,  1,  2,  2,  2,  2,  1, 2, 4
                 };

void beep(float note, float duration = 50, int pause = 0, byte repeat = 1 ) {
  for (byte r = 0; r < repeat; r++) {
    if (note)
      tone(BUZZER, BASE_PITCH * pow(1.05946, note), duration * LENGTH_FACTOR);
    else
      delay(duration * LENGTH_FACTOR);
    delay(pause * LENGTH_FACTOR);
    noTone(BUZZER);
  }
}

void playMelody(int start) {
  byte len = (byte)EEPROM.read(start) / 2;
  for (int i = 0; i < len; i++)
    beep(EEPROM.read(start - 1 - i), 1000 / EEPROM.read(start - 1 - len - i), 50);
}

void playTone(uint16_t tone1, uint16_t duration) {
  if (tone1 < 50 || tone1 > 15000) return; // these do not play on a piezo
  for (long i = 0; i < duration * 1000L; i += tone1 * 2) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(tone1);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(tone1);
  }
}

void meow(byte repeat = 2, byte duration = 10, byte startF = 210 + rand() % 10,  byte endF = 220 + rand() % 10) { // Bird chirp
  for (byte r = 0; r < repeat; r++)
    for (byte i = startF; i < endF; i++) {
      playTone(i, duration);
    }
}
void soundFallOver() {
  meow();
}
