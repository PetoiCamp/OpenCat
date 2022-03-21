#define EVERY_X_SECONDS 3

#define MIND_PROGMEM

#ifdef MIND_PROGMEM
const char mind0[] PROGMEM = "allRand";
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

const char* const mindList[] PROGMEM = {mind0, mind0, mind0, mind0, mind1, mind1, mind12, mind13, mind3, mind4, mind6, mind11,
#ifdef BITTLE
                                        mind7,
#endif
                                       };

#else
const char *mindList[] = {"ksit", "kbalance", "kpee", "kpu", "m0 -45 1 30 0 45 0 0", "kck", "o ",
#ifdef BITTLE
                          "kjy", "kvt",
#else
                          "M0 -45 1 10", "m2 0 2 -20 2 20"
#endif
                         };
#endif
long idleTimer;
byte randomMindListLength = sizeof(mindList) / 2;
void randomMind() {
  int randomChoice = random() % (randomMindListLength * EVERY_X_SECONDS); //on average every EVERY_X_SECONDS seconds
  if (millis() - idleTimer > 1000) {//every second throw a dice
    idleTimer = millis();
    if (randomChoice < randomMindListLength) {
      if (randomChoice == 0) {
        token = T_INDEXED_SIMULTANEOUS_BIN;
        char allRand[] = {0, currentAng[0] + rand() % 80 - 40, 1, currentAng[0] + rand() % 80 - 40, 2, currentAng[0] + rand() % 80 - 40,
                          12, currentAng[0] + rand() % 10 - 5, 13, currentAng[0] + rand() % 10 - 5
                         };
        cmdLen = 10;
        for (byte i = 0; i < cmdLen; i++)
          newCmd[i] = allRand[i];
        newCmd[cmdLen] = '\0';
      }
      else {
#ifdef MIND_PROGMEM
        strcpy_P(newCmd, (char *)pgm_read_word(&mindList[randomChoice]));
        token = newCmd[0];
        strcpy(newCmd, newCmd + 1);// this is duable only because newCmd+1 is after newCmd!
#else
        token = mindList[randomChoice][0];
        strcpy(newCmd, mindList[randomChoice] + 1);
#endif
      }
      PTL(newCmd);
      newCmdIdx = 5;
    }
  }
}
