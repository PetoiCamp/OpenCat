//use the software serial port on the NyBoard to read the module. connect the module to the grove socket with pin 6 and 7.
//if you wire the module up with the USB programmer directly, connect the module's Tx to the programmer's Rx, and Rx to Tx.

#include <SoftwareSerial.h>
#define BAUD_RATE 115200
#define SOFT_BAUD_RATE 9600
SoftwareSerial mySerial(6, 7);  //(Rx,Tx)

String customizedCmdList[] = {
  "first", "second", "third", "4th", "5th", "6th", "7th", "8th", "9th", "10th", "\0"  //define up to ten customized commands.
};                                                                                    //put "\0" as the last element to end the list
int listLength = 0;

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(SOFT_BAUD_RATE);
  mySerial.setTimeout(5);
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(5);
  while (customizedCmdList[listLength] != "\0")
    listLength++;
  Serial.print("Number of customized commands on the main board: ");
  Serial.println(listLength);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    int idx = 0;
    Serial.println(cmd);
    mySerial.print(cmd);
  }
  if (mySerial.available()) {
    String cmd = mySerial.readStringUntil('\n');
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