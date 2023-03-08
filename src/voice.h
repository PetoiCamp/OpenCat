#ifdef VOICE
#include <SoftwareSerial.h>
SoftwareSerial Serial2(6, 7);
#endif
#define SERIAL2_BAUD_RATE 9600
#define MAX_CUSTOMIZED_CMD 10

// Speak "start learning" to record your voice commands in order. You can record up to 10 voice commands
// Speak "stop learning" to stop in the middle
// Speak one of the recorded voice commands to trigger the reaction
// Speak "clear the learning data" to delete all the recordings at once. (you cannot delete a specific recording)
// The reactions below are already defined in the program. You may use SkillComposer to design new skills then import them into InstinctX.h
// Other serial commands are also supported, such as joint movements and melody

// 说”开始学习“开始录音，最多10条
// 说”结束学习“停止录入
// 说出口令触发反应
// 说”清除数据“删除所有自定义口令（无法删除单条口令）
// 下列行为是程序中预设的，您可以用技能创作坊设计新技能并导入到 InstinctX.h
// 支持其他的串口指令，比如活动关节和旋律

// const char voice1[] PROGMEM = "b14,8,14,8,21,8,21,8,23,8,23,8,21,4";  //小星星 twinkle star
// const char voice2[] PROGMEM = "m0 80 0 -80 0 0";                                                         //动头 move head
// const char voice3[] PROGMEM = "scrh";                                                                    //挠痒痒 scratch
const char voice1[] PROGMEM = "kdg";    //刨地	dig
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
      tQueue->push_back(new Task('k', "up"));
      tQueue->push_back(new Task('p', ""));
    }
  }
}
