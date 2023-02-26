#ifdef VOICE
#include <SoftwareSerial.h>
SoftwareSerial Serial2(6, 7);
#endif
#define SERIAL2_BAUD_RATE 9600
#define MAX_CUSTOMIZED_CMD 10

const char voice1[] PROGMEM = "kdg";  //刨地	dig
// const char voice2[] PROGMEM = "c";     //立正	attention
const char voice2[] PROGMEM = "khg";    //抱抱	hug
const char voice3[] PROGMEM = "khu";    //举手	hands up
const char voice4[] PROGMEM = "khds";   //倒立	handstand
const char voice5[] PROGMEM = "ksnf";   //闻一下	sniff
const char voice6[] PROGMEM = "khsk";   //握手	handshake
const char voice7[] PROGMEM = "kgdb";   //好孩子	Good boy
const char voice8[] PROGMEM = "ktbl";   //变桌子	Be table
const char voice9[] PROGMEM = "kbx";    //打拳	boxing
const char voice10[] PROGMEM = "kang";  //生气 angry
const char *const voiceTable[] PROGMEM = {
  voice1,
  voice2,
  voice3,
  voice4,
  voice5,
  voice6,
  voice7,
  voice8,
  voice9,
  voice10,
};

int listLength = 0;

void voiceSetup() {
  PTLF("Init voice");
  Serial2.begin(SERIAL2_BAUD_RATE);
  Serial2.setTimeout(5);
  listLength = min(sizeof(voiceTable) / sizeof(voiceTable[0]), MAX_CUSTOMIZED_CMD);
  PTLF("Customized voice:");
  PTL(listLength);
}

void read_voice() {
  // put your main code here, to run repeatedly:
  if (Serial2.available()) {
    String raw = Serial2.readStringUntil('\n');
    // PTL(raw);
    byte index = (byte)raw[2];  //interpret the 3rd byte as integer
    int shift = -1;
    if (index > 10) {
      if (index < 21) {  //11 ~ 20 are customized commands, and their indexes should be shifted by 11
        index -= 11;
        PT(index);
        if (index < listLength) {
          raw = strcpy_P(newCmd, (char *)pgm_read_word(&(voiceTable[index])));
          PTL(newCmd);
          token = raw[0];
          shift = 1;
        } else {
          PTLF("_?");
        }
      } else if (index < 61) {  //21 ~ 60 are preset commands, and their indexes should be shifted by 21.
                                //But we don't need to use their indexes.
        token = T_SKILL;
        shift = 3;
      }
      if (shift > 0)
        tQueue->push_back(new Task(token, raw.c_str() + shift, 2000));
      tQueue->push_back(new Task('p', ""));
    }
  }
}
