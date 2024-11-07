// Petoi Voice Command Module
// Doc: https://docs.petoi.com/extensible-modules/voice-command-module
// use the software serial port on the NyBoard to read the module. connect the module to the grove socket with pin 8 and 9.
// or use the serial 2 port on the BiBoard to read the module. connect the module to the pin Tx2 and Rx2.
// if you wire the module up with the USB programmer directly, connect the module's Tx to the programmer's Rx, and Rx to Tx.
// Rongzhong Li
// Petoi LLC
// Jan 12, 2023
#ifdef VOICE
#include <SoftwareSerial.h>
SoftwareSerial Serial2(8, 9);  //Rx, Tx
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

#ifdef ROBOT_ARM
const char voice1[] PROGMEM = "kpickUpL";     //捡起来
const char voice2[] PROGMEM = "kputDownL";    //放下
const char voice3[] PROGMEM = "kshowOff";     //展示
const char voice4[] PROGMEM = "khuntL";       //捕猎
const char voice5[] PROGMEM = "kputAwayL";    //收起来
const char voice6[] PROGMEM = "kthrowAwayL";  //丢出去
const char voice7[] PROGMEM = "klaunchL";     //发射
const char voice8[] PROGMEM = "kclapL";       //鼓掌
const char voice9[] PROGMEM = "ktossL";       //抛出去
#else
const char voice1[] PROGMEM = "T";                                    //call the last skill data sent by the Skill Composer ## Will cause bug when skill is longer than 5 frames
#ifdef BITTLE
const char voice2[] PROGMEM = "kpu1";                                 //单手俯卧撑	 single-handed pushups
#elif defined NYBBLE
const char voice2[] PROGMEM = "kwsf";  //洗脸	  wash face
#endif
const char voice3[] PROGMEM = "m0 80 0 -80";                          //动头 move head
const char voice4[] PROGMEM = "kmw";                                  //moonwalk
const char voice5[] PROGMEM = "b14,8,14,8,21,8,21,8,23,8,23,8,21,4";  //小星星 twinkle star
const char voice6[] PROGMEM = "6th";
const char voice7[] PROGMEM = "7th";
const char voice8[] PROGMEM = "8th";
const char voice9[] PROGMEM = "9th";
#endif
const char voice10[] PROGMEM = "10th";
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
bool enableVoiceQ = true;

void voiceSetup() {
  PTLF("Init voice");
  Serial2.begin(SERIAL2_BAUD_RATE);
  Serial2.setTimeout(5);
  listLength = min(sizeof(voiceTable) / sizeof(voiceTable[0]), MAX_CUSTOMIZED_CMD);
  PTLF("Customized voice:");
  PTL(listLength);
}

void set_voice() {  // send some control command directly to the module
                    // XAa: switch English
                    // XAb: switch Chinese
                    // XAc: turn on the sound response
                    // XAd: turn off the sound response
                    // XAe: start learning
                    // XAf: stop learning
                    // XAg: clear the learning data
  byte c = 0;
  while (newCmd[c++] != '~')
    ;
  newCmd[c - 1] = '\0';
  // Serial.print('X');
  // Serial.println(newCmd);
  Serial2.print('X');
  Serial2.println(newCmd);
  while (Serial2.available() && Serial2.read())
    ;
  if (!strcmp(newCmd, "Ac"))  // enter "XAc" in the serial monitor or add button "X65,99" in the mobile app to enable voice reactions
                              // 在串口监视器输入指令“XAc”或在手机app创建按键"X65,99"来激活语音动作
    enableVoiceQ = true;
  else if (!strcmp(newCmd, "Ad"))  // enter "XAd" in the serial monitor or add button "X65,100" in the mobile app to disable voice reactions
                                   // 在串口监视器输入指令“XAd”或在手机app创建按键"X65,100"来禁用语音动作
    enableVoiceQ = false;
  PTL(token);
  resetCmd();
}
void read_voice() {
  if (Serial2.available()) {
    String raw = Serial2.readStringUntil('\n');
    // PTL(raw);
    byte index = (byte)raw[2];  //interpret the 3rd byte as integer
    int shift = -1;
    if (index == 67)  //say "play sound" to enable voice reactions / 说“打开音效”激活语音动作
      enableVoiceQ = true;
    else if (index == 68)  //say "be quiet" to disable voice reactions / 说“安静点”禁用语音动作
      enableVoiceQ = false;
    else if (index > 10) {
      if (index < 21) {  //11 ~ 20 are customized commands, and their indexes should be shifted by 11
        index -= 11;
        PT(index);
        PT('\t');
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
        token = raw[3];         //T_SKILL;
        shift = 4;              //3;
      }
      if (enableVoiceQ) {
        const char *cmd = raw.c_str() + shift;
        tQueue->addTask(token, shift > 0 ? cmd : "", 2000);
        char end = cmd[strlen(cmd) - 1];
        if (!strcmp(cmd, "bk") || !strcmp(cmd, "x") || end >= 'A' && end <= 'Z' || end == 'x') {
          tQueue->addTask('k', "up");
        }
      }
    }
  }
}
