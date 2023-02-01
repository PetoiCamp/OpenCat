#ifdef VOICE
#include <SoftwareSerial.h>
SoftwareSerial Serial2(6, 7);
#endif
#define SERIAL2_BAUD_RATE 9600
#define MAX_CUSTOMIZED_CMD 10

String customizedCmdList[] = {
  "kbalance", "ksit", "d", "m0 80 0 -80 0 0", "khi", "krc", "kvtF", "kck", "kjy", "kstr"//define up to 10 customized commands.
};
int listLength = 0;

void voiceSetup() {
  PTLF("Init voice");
  Serial2.begin(SERIAL2_BAUD_RATE);
  Serial2.setTimeout(5);
  listLength = min(sizeof(customizedCmdList) / sizeof(customizedCmdList[0]), MAX_CUSTOMIZED_CMD);
  PTLF("Number of customized commands on the main board: ");
  PTL(listLength);
}

void read_voice() {
  // put your main code here, to run repeatedly:
  if (Serial2.available()) {
    String temp = Serial2.readStringUntil('\n');
    PTL(temp);
    byte index = (byte)temp[2];  //interpret the 3rd byte as integer
    if (index > 10) {
      if (index < 21) {  //11 ~ 20 are customized commands, and their indexes should be shifted by 11
        index -= 11;
        PT(index);
        PT(' ');
        if (index < listLength) {
          temp = customizedCmdList[index];
          token = temp[0];
          if (token == T_SKILL)
            strcpy(newCmd, temp.c_str() + 1);
          else
            strcpy((char*)dataBuffer, temp.c_str() + 1);
        } else {
          PTLF("Undefined!");
        }
      } else if (index < 61) {  //21 ~ 60 are preset commands, and their indexes should be shifted by 21.
                                //But we don't need to use their indexes.
        token = T_SKILL;
        strcpy(newCmd, temp.c_str() + 3);
      }
      newCmdIdx = 5;
      PTL(newCmd);
    }
  }
}