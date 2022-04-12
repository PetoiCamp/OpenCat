#define EVERY_X_SECONDS 2

const char mind0[] PROGMEM = "iRand";
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

const char* const mindList[] PROGMEM = {mind0, mind0, mind0, mind1, mind12,
#ifdef BITTLE
                                        mind6, //mind8, 
#endif
                                       };

long idleTimer;
int randomInterval = 1000;
void allRandom() {

  char tokenSet[] = {T_INDEXED_SIMULTANEOUS_BIN, T_MOVE_BIN};

  int8_t jointSet[] = {0, 1, 2, 8, 9,  12, 13, 14, 15};
  PTL(sizeof(jointSet));
  byte rangeSet[] = {90, 90, 180, 5, 5, 10, 10, 5, 5};

  token = tokenSet[random() % 2];
  cmdLen = rand() % 4 + 4;
  for (byte r = 0; r < cmdLen; r++) {
    byte j = rand() % sizeof(jointSet);
    dataBuffer[r * 2] = jointSet[j];
    dataBuffer[r * 2 + 1] = (int8_t)min(max(currentAng[jointSet[j]] + rand() % rangeSet[j] - rangeSet[j] / 2, -128), 127);
    //    PT(jointSet[j]); PT('\t'); PTL(int(dataBuffer[r * 2 + 1]));
  }
  cmdLen *= 2;
  dataBuffer[cmdLen] = '\0';
}
void randomMind() {
  byte randomMindListLength = sizeof(mindList) / 2;
  int randomChoice = random() % (randomMindListLength * EVERY_X_SECONDS); //on average every EVERY_X_SECONDS seconds
  if (millis() - idleTimer > randomInterval) {//every second throw a dice
    idleTimer = millis();
    if (randomChoice < randomMindListLength) {
      if (randomChoice < 3) {
        allRandom();
      }
      else {
        strcpy_P((char *)dataBuffer, (char *)pgm_read_word(&mindList[randomChoice]));
        token = dataBuffer[0];
        if (token == T_SKILL)
          strcpy(newCmd, (char *)dataBuffer + 1);// this is duable only because newCmd+1 is after newCmd!
        else
          strcpy((char *)dataBuffer, (char *)dataBuffer + 1);// this is duable only because dataBuffer+1 is after dataBuffer!

      }
      //      PTL(newCmd);
      newCmdIdx = 5;
    }
  }
}
