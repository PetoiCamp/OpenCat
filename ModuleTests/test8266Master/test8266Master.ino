// This code can be uploaded to the ESP8266 WiFi dongle
// and act as a task scheduler for the Petoi robots based on ATMega328P
//
// Rongzhong Li
// Nov.9, 2022


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(5);
  bool connected = false;
  while (!connected) {
    Serial.print("b 20 8 22 8 24 8");
    for (byte t = 0; t < 100; t++) {
      if (Serial.available())
        if (Serial.read() == 'b') {
          connected = true;
          while (Serial.available() && Serial.read())
            ;
          break;
        }
      delay(10);
    }
    delay(1000);
  }
}

void sendCMD(const char cmd[], int wait = 0) {
  Serial.print(cmd);
  while (true) {
    if (Serial.available() && toLowerCase(Serial.read()) == cmd[0]) {
      delay(10);
      while (Serial.available() && Serial.read())
        ;
      break;
    }
    delay(2);
  }
  delay(wait);
}

void loop() {
  sendCMD("d", 500);                    //坐下
  sendCMD("khi");                       //打招呼
  sendCMD("kpu");                       //俯卧撑
  sendCMD("kvtF", 1000);                //原地踏步
  sendCMD("g");                         //加速
  sendCMD("kwkF", 1500);                //行走
  sendCMD("kck");                       //观察
  sendCMD("kpu1");                      //单手俯卧撑
  sendCMD("kvtR", 2000);                //旋转
  sendCMD("g", 100);                    //减速
  sendCMD("ktrF", 1500);                //跑步
  sendCMD("kjy", 0);                    //加油
  sendCMD("i 0 45 8 -90 9 -90", 1000);  //伸手转头
  sendCMD("ksit", 1000);                //坐下
}