const char mind0[] PROGMEM = "iRand";
const char mind1[] PROGMEM = "ksit";
const char mind2[] PROGMEM = "kbalance";
const char mind3[] PROGMEM = "kpee";
const char mind4[] PROGMEM = "kpu";
const char mind5[] PROGMEM = "khi";
const char mind6[] PROGMEM = "kck";
const char mind7[] PROGMEM = "kjy";
const char mind8[] PROGMEM = "kvtF";
const char mind9[] PROGMEM = "kvtL";
const char mind10[] PROGMEM = "kvtR";
const char mind11[] PROGMEM = "o ";
const char mind12[] PROGMEM = "u ";
const char mind13[] PROGMEM = "kstr";
const char mind14[] PROGMEM = "kwkF";
const char mind15[] PROGMEM = "kwkL";
const char mind16[] PROGMEM = "kwkR";
const char mind17[] PROGMEM = "kpu1";
const char mind18[] PROGMEM = "kwkF";
const char mind19[] PROGMEM = "kbk";

const char* const mindList[] PROGMEM = {mind0, mind1, mind4, mind5, mind12, mind13,
#ifdef BITTLE
                                        mind3, mind6, mind7, mind8, mind9, mind10, mind17, mind18, mind19,
#endif
                                       };
byte choiceWeight[] = {
#ifdef ALL_RANDOM
  30,
#else
  10,
#endif
  5, 1, 2, 2, 1,
#ifdef BITTLE
  1, 3, 2, 1, 1, 1, 1, 1, 1
#endif
};

byte randomMindListLength = sizeof(mindList) / 2;
int randomBase = 0;
void allRandom() {
  char tokenSet[] = {T_INDEXED_SIMULTANEOUS_BIN, T_INDEXED_SEQUENTIAL_BIN};
  int8_t jointSet[] = {0, 1, 2,};// 8, 9,  12, 13, 14, 15};
  byte rangeSet[] = {60, 60, 180,};// 10, 10, 20, 20, 10, 10};

  token = tokenSet[random() % 2];
  cmdLen =  sizeof(jointSet) ;//random() % 2 + 2;
  for (byte r = 0; r < cmdLen; r++) {
    byte j = random() % sizeof(jointSet);
    dataBuffer[r * 2] = jointSet[j];
    dataBuffer[r * 2 + 1] = (int8_t)min(max(currentAng[jointSet[j]] + random() % rangeSet[j] - rangeSet[j] / 2, -90), 90);
    //    PT(jointSet[j]); PT('\t'); PTL(int(dataBuffer[r * 2 + 1]));
  }
  cmdLen *= 2;
  dataBuffer[cmdLen] = '\0';
}
void randomMind() {
  long current = millis();
  if (current - idleTimer > randomInterval) { //every randomInterval throw a dice
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
    }
    else {
      strcpy_P((char *)dataBuffer, (char *)pgm_read_word(&mindList[randomChoice]));
      token = dataBuffer[0];
      if (token == T_SKILL)
        strcpy(newCmd, (char *)dataBuffer + 1);// this is duable only because newCmd+1 is after newCmd!
      else
        strcpy((char *)dataBuffer, (char *)dataBuffer + 1);// this is duable only because dataBuffer+1 is after dataBuffer!
      //      PTL(newCmd);
    }
    newCmdIdx = 100;
  }
}
