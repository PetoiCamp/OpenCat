#ifdef VOICE
#include <SoftwareSerial.h>
SoftwareSerial mySerial(6, 7);
#endif
#define SERIAL2_BAUD_RATE 9600

void voiceSetup() {
  Serial.println("Init voice");
  mySerial.begin(SERIAL2_BAUD_RATE);
}

void read_voice() {
  // put your main code here, to run repeatedly:
  if (mySerial.available()) {
    PTL("hello");
    String temp = mySerial.readStringUntil('\n');
    Serial.println(temp);
    Serial.print(temp[0]);  //X
    Serial.print(' ');
    Serial.print(temp[1]);  //A
    Serial.print(' ');
    byte index = (byte)temp[2];
    Serial.print(index);  //把第三位字符解析为数字
    Serial.print(' ');
    Serial.println(temp.c_str() + 3);  //从第4位打印剩余字符
    strcpy(newCmd, temp.c_str() + 3);

    if (index > 10) {
      if (index < 21) {
        PTL(newCmd);
      } 
      else if (index < 61) {
        token = T_SKILL;
        newCmdIdx = 5;
      }
    }
  }
}