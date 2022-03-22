#define EVERY_X_SECONDS 1

const char mind0[] PROGMEM = "IRand";
const char mind1[] PROGMEM = "ksit";
const char mind2[] PROGMEM = "kbalance";
const char mind3[] PROGMEM = "kpee";
const char mind4[] PROGMEM = "kpu";
const char mind6[] PROGMEM = "kck";
const char mind7[] PROGMEM = "kjy";
const char mind8[] PROGMEM = "kvt";
const char mind11[] PROGMEM = "o ";
const char mind12[] PROGMEM = "u ";
const char mind13[] PROGMEM = "kstr";

const char* const mindList[] PROGMEM = {mind0, mind0, mind0, mind0, mind0, mind1, mind12, mind13, 
#ifdef BITTLE
                                        mind8, mind6,
#endif
                                       };

long idleTimer;
byte randomMindListLength = sizeof(mindList) / 2;

void allRandom() {
  char tokenSet[] = {T_INDEXED_SIMULTANEOUS_BIN, T_MOVE_BIN};
  int8_t jointSet[] = {0, 1, 2, 12, 13};
  int8_t rangeSet[] = {120, 80, 180, 20, 20};

  token = tokenSet[random() % 2];
  cmdLen = rand() % 3 + 3;
  for (byte r = 0; r < cmdLen; r++) {
    byte j = rand() % sizeof(jointSet);
    newCmd[r * 2] = jointSet[j];
    newCmd[r * 2 + 1] = currentAng[jointSet[j]] + rand() % rangeSet[j] - rangeSet[j] / 2;
//    PT(jointSet[j]);PT('\t');PTL(int(newCmd[r * 2 + 1]));
  }
  cmdLen *= 2;
  newCmd[cmdLen] = '\0';
}
void randomMind() {
  int randomChoice = random() % (randomMindListLength * EVERY_X_SECONDS); //on average every EVERY_X_SECONDS seconds
  if (millis() - idleTimer > 1000) {//every second throw a dice
    idleTimer = millis();
    if (randomChoice < randomMindListLength) {
      if (randomChoice == 0) {
        allRandom();
      }
      else {
        strcpy_P(newCmd, (char *)pgm_read_word(&mindList[randomChoice]));
        token = newCmd[0];
        strcpy(newCmd, newCmd + 1);// this is duable only because newCmd+1 is after newCmd!

      }
//      PTL(newCmd);
      newCmdIdx = 5;
    }
  }
}
