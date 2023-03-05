const char mind0[] PROGMEM = "iRand";
const char mind1[] PROGMEM = "i";
const char mind2[] PROGMEM = "ksit";
const char mind3[] PROGMEM = "kstr";
const char mind4[] PROGMEM = "khi";
const char mind5[] PROGMEM = "kck";
const char mind6[] PROGMEM = "kchr";
const char mind7[] PROGMEM = "ksnf";
const char mind8[] PROGMEM = "kscrh";
const char mind9[] PROGMEM = "kpee";
const char mind10[] PROGMEM = "kpu";
const char mind11[] PROGMEM = "kcmh";
const char mind12[] PROGMEM = "kvtF";

const char *const mindList[] PROGMEM = {
#ifdef ALL_RANDOM
  mind0,
  mind1,
#endif
  mind2,
  mind3,
  mind4,
  mind5,
  mind6,
  mind7,
  mind8,
  mind9,
  mind10,
  mind11,
  mind12,
};
byte choiceWeight[] = {
#ifdef ALL_RANDOM
  30,
  20,
#endif
  10,
  5,
  3,
  3,
  3,
  3,
  2,
  2,
  2,
  2,
  2,
};

byte randomMindListLength = sizeof(mindList) / 2;
int randomBase = 0;
void allRandom() {
  char tokenSet[] = { T_INDEXED_SIMULTANEOUS_BIN, T_INDEXED_SEQUENTIAL_BIN };
  int8_t jointSet[] = {
    0,
    1,
    2,
  };  // 8, 9,  12, 13, 14, 15};
  byte rangeSet[] = {
    30,
    30,
    30,
  };  // 10, 10, 20, 20, 10, 10};

  token = tokenSet[random() % 2];
  cmdLen = sizeof(jointSet);  //random() % 2 + 2;
  for (byte r = 0; r < cmdLen; r++) {
    byte j = random() % sizeof(jointSet);
    newCmd[r * 2] = jointSet[j];
    newCmd[r * 2 + 1] = (int8_t)min(max(currentAng[jointSet[j]] - currentAdjust[jointSet[j]] + random() % rangeSet[j] - rangeSet[j] / 2, -90), 90);
    //    PT(jointSet[j]); PT('\t'); PTL(int(newCmd[r * 2 + 1]));
  }
  cmdLen *= 2;
  // newCmd[cmdLen] = '~';
}
void randomMind() {
  long current = millis();
  if (current - idleTimer > randomInterval) {  //every randomInterval throw a dice
    idleTimer = current;
    int randomNum = random() % randomBase;
    byte randomChoice = -1;
    while (randomNum >= 0) {
      randomChoice++;
      randomNum -= choiceWeight[randomChoice];
    }
    if (randomChoice == 0) {
#ifdef ALL_RANDOM
      allRandom();
#endif
    } else {
      strcpy_P((char *)newCmd, (char *)pgm_read_word(&mindList[randomChoice]));
      token = newCmd[0];

      if (token == T_SKILL)
        strcpy(newCmd, (char *)newCmd + 1);  // this is duable only because newCmd+1 is after newCmd!
      else
        strcpy((char *)newCmd, (char *)newCmd + 1);  // this is duable only because newCmd+1 is after newCmd!
      //      PTL(newCmd);
    }
    newCmdIdx = 100;
  }
}
