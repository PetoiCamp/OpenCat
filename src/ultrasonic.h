//code provided by the manufacturer
//modified by Rongzhong Li for better demonstration.
//Feb.16, 2021

#include "rgbUltrasonic/RgbUltrasonic.h"

RgbUltrasonic mRUS04(6, 7);//(signal, RGB)
//The RGB LED ultrasonic module should be plugged in the fourth grove socket with D6, D7

int interval = 3;
#define ULTRASONIC_IMU_SKIP 10
long colors[] = {RGB_RED, RGB_GREEN, RGB_BLUE, RGB_YELLOW, RGB_PURPLE, RGB_WHITE};
long ultraTimer;

void read_ultrasonic() {
  int distance;
  //  if  (millis() - ultraTimer > 10 )
  { //|| token == 'k' && millis() - ultraTimer > 3000) {
    ultraTimer = millis();
    distance = mRUS04.GetUltrasonicDistance();

    if (distance > 30) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_WHITE, E_EFFECT_BREATHING);
      //      token = 'k';
      //      strcpy(newCmd, "balance");
      //      newCmdIdx = 6;
    }
    else if (distance < 3) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_RED, E_EFFECT_FLASH);
      beep(20 - distance / interval * 2 , 20 + distance / 2, 5 + distance );
      token = T_INDEXED_SIMULTANEOUS_BIN;
      char allRand[] = {0, currentAng[0] + rand() % 80 - 40, 1, currentAng[0] + rand() % 80 - 40, 2, currentAng[0] + rand() % 80 - 40,
                        12, currentAng[0] + rand() % 10 - 5, 13, currentAng[0] + rand() % 10 - 5
                       };
      cmdLen = 10;
      for (byte i = 0; i < cmdLen; i++)
        newCmd[i] = allRand[i];
      newCmd[cmdLen] = '\0';
      //      newCmdIdx = 6;
    }
    else if (distance < 10) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_YELLOW, E_EFFECT_ROTATE);
      token = 'k';
      strcpy(newCmd, "sit");
      newCmdIdx = 6;
    }

    else { //10~30
      //      Serial.print(50 - distance / interval * 2 );
      //      Serial.print("\t" );
      //      Serial.print(20 +  distance / 2 );
      //      Serial.print("\t");
      //      Serial.println(5 + distance );
      //      beep(60 - distance / interval * 2 , 20 + distance / 2, 5 + distance );
      mRUS04.SetRgbColor(E_RGB_ALL, colors[max(min(distance / 6, 5), 0)]);

      token = T_LISTED_BIN;
      int8_t mid[] = {0,   0,   0,   0,   0,   0,   0,   0,  30,  30, -30, -30,  30,  30, -30, -30,};
      int8_t allParameter[] = {distance  - 15, 50 - distance , distance  - 15, 0,
                               0, 0, 0, 0,
                               mid[8] - 10 + distance , mid[9] - 10 + distance, mid[10] - 15 + distance, mid[11] - 15 + distance,
                               mid[12] + 10 - distance, mid[13] + 10 - distance, mid[14] + 10 - distance, mid[15] + 10 - distance
                              };
      printList(allParameter);
      cmdLen = 16;
      for (byte i = 0; i < cmdLen; i++)
        newCmd[i] = allParameter[i];
      newCmd[cmdLen] = '\0';
      newCmdIdx = 6;
    }
  }
}
