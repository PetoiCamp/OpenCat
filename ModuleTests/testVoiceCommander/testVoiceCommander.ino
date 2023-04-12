// Petoi Voice Command Module
// use the software serial port on the NyBoard to read the module. connect the module to the grove socket with pin 6 and 7.
// or use the serial 2 port on the BiBoard to read the module. connect the module to the pin Tx2 and Rx2.
// if you wire the module up with the USB programmer directly, connect the module's Tx to the programmer's Rx, and Rx to Tx.
// Rongzhong Li
// Petoi LLC
// Jan 12, 2023

#define SOFTWARE_SERIAL  //disable this line to use the serial 2 of BiBoard
#ifdef SOFTWARE_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial Serial2(6, 7);  //(Rx,Tx)
#endif

#define BAUD_RATE 115200
#define SOFT_BAUD_RATE 9600
#define MAX_CUSTOMIZED_CMD 10

// Speak "start learning" to record your voice commands in order. You can record up to 10 voice commands
// Speak "stop learning" to stop in the middle
// Speak one of the recorded voice commands to trigger the reaction
// Speak "clear the learning data" to delete all the recordings at once. (you cannot delete a specific recording)
// The following command will be the return values of the recorded voice commnads in order

// 说”开始学习“开始录音，最多10条
// 说”结束学习“停止录入
// 说出口令触发反应
// 说”清除数据“删除所有自定义口令（无法删除单条口令）
// 下列自定义指令会依次作为新学习的口令的返回值

String customizedCmdList[] = {
  "first", "second", "third", "4th", "5th", "6th", "7th", "8th", "9th", "10th"  //define up to ten customized commands.
};
int listLength = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(5);
  Serial2.begin(SOFT_BAUD_RATE);
  Serial2.setTimeout(5);
  listLength = min(int(sizeof(customizedCmdList) / sizeof(customizedCmdList[0])), MAX_CUSTOMIZED_CMD);
  Serial.print("Number of customized commands on the main board: ");
  Serial.println(listLength);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {  // send some control command directly to the module
                             // XAa: switch English
                             // XAb: switch Chinese
                             // XAc: turn on the sound response
                             // XAd: turn off the sound response
                             // XAe: start learning
                             // XAf: stop learning
                             // XAg: clear the learning data
    String cmd = Serial.readStringUntil('\n');
    int idx = 0;
    Serial.println(cmd);  //the user input
    Serial2.print(cmd);   //send the user input to the voice module through serial 2
  }

  if (Serial2.available()) {
    Serial.print("-----\nReturned value: ");
    String cmd = Serial2.readStringUntil('\n');

    Serial.print(cmd[0]);  //It should be X
    Serial.print(' ');
    Serial.print(cmd[1]);  //It should be A
    Serial.print(' ');
    int index = (byte)cmd[2];
    Serial.print((byte)cmd[2]);
    Serial.print(' ');
    Serial.println(cmd.c_str() + 3);
    if (index <= 20) {
      Serial.print("Customized Command Index: ");
      index = index - 11;  //Convert the third byte to integer index of the customized command, starting from 0
      Serial.println(index);
      Serial.println((index < listLength) ? customizedCmdList[index] : "Undefined!");
    } else if (index < 61) {
      // Serial.print((byte)cmd[2]);  //Convert the third byte to integer
      Serial.print("Predefined Command Index: ");
      Serial.println(index - 21);  //Convert the third byte to integer index, starting from 0
      Serial.println(cmd.c_str() + 3);
    } else {
      switch (tolower(index)) {
        case 'a':
          {
            Serial.println("Switch English");
            break;
          }
        case 'b':
          {
            Serial.println("Switch Chinese");
            break;
          }
        case 'c':
          {
            Serial.println("Turn on the audio response");
            break;
          }
        case 'd':
          {
            Serial.println("Turn off the audio response");
            break;
          }
        case 'e':
          {
            Serial.println("Start learning");
            break;
          }
        case 'f':
          {
            Serial.println("Stop learning");
            break;
          }
        case 'g':
          {
            Serial.println("Delete all learning data!");
            break;
          }
      }
    }
  }
}
