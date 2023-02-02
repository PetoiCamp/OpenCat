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
  // if (Serial.available()) {// send some control command directly to the module
  //   String cmd = Serial.readStringUntil('\n');
  //   int idx = 0;
  //   Serial.println(cmd);
  //   Serial2.print(cmd);
  // }

  if (Serial2.available()) {
    String cmd = Serial2.readStringUntil('\n');
    if ((byte)cmd[2] <= 20) {
      Serial.print("Customized Command Index: ");
      int index = (byte)cmd[2] - 11;  //Convert the third byte to integer index of the customized command, starting from 0
      Serial.println(index);
      Serial.println((index < listLength) ? customizedCmdList[index] : "Undefined!");
    } else {
      Serial.print(cmd[0]);  //It should be X
      Serial.print(' ');
      Serial.print(cmd[1]);  //It should be A
      Serial.print(' ');
      // Serial.print((byte)cmd[2]);  //Convert the third byte to integer
      Serial.print((byte)cmd[2] - 21);  //Convert the third byte to integer index, starting from 0
      Serial.print(' ');
      Serial.println(cmd.c_str() + 3);  //print the bytes as char strings from the 4th byte}
    }
  }
}