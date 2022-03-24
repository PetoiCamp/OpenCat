//code provided by the manufacturer
//modified by Rongzhong Li for better demonstration.
//Feb.16, 2021

#include "rgbUltrasonic/RgbUltrasonic.h"

RgbUltrasonic mRUS04(6, 7);//(signal, RGB)
//The RGB LED ultrasonic module should be plugged in the fourth grove socket with D6, D7

int interval = 3;
#define ULTRASONIC_IMU_SKIP 10
long colors[] = {RGB_RED, RGB_PURPLE, RGB_GREEN, RGB_BLUE, RGB_YELLOW, RGB_WHITE};
long ultraTimer;
int ultraInterval = 1000;
int distance;
void read_ultrasonic() {
  if  (millis() - ultraTimer > ultraInterval)
  { //|| token == 'k' && millis() - ultraTimer > 3000) {
    ultraTimer = millis();
    ultraInterval = 0;
    randomInterval = 1000;
    autoSwitch = false;
    distance = mRUS04.GetUltrasonicDistance();

    if (distance > 60) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_WHITE, E_EFFECT_BREATHING);
      ultraInterval = 1000;
      autoSwitch = true;
      randomInterval = 1000;
    }
    else if (distance > 40) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_YELLOW, E_EFFECT_ROTATE);
    }
    else if (distance < 2) {
      token = T_SKILL;
      strcpy(newCmd, "bk");
      newCmdIdx = 6;
      ultraInterval = 2000;
      randomInterval = 5000;
    }
    else if (distance < 4) {
      mRUS04.SetRgbEffect(E_RGB_ALL, RGB_RED, E_EFFECT_FLASH);
      meow(rand() % 3 + 1, distance * 2);
      token = T_INDEXED_SIMULTANEOUS_BIN;
      char allRand[] = {0, currentAng[0] + rand() % 40 - 20, 1, currentAng[0] + rand() % 80 - 40, 2, currentAng[0] + rand() % 80 - 40
                       };
      cmdLen = 3;
      for (byte i = 0; i < cmdLen; i++)
        newCmd[i] = allRand[i];
      newCmd[cmdLen] = '\0';
      newCmdIdx = 6;
    }
    else if (distance < 6) {
      mRUS04.SetRgbColor(E_RGB_ALL, RGB_RED);
      token = 'k';
      strcpy(newCmd, "sit");
      newCmdIdx = 6;
    }

    else { //6~40
      distance -= 6;
      mRUS04.SetRgbColor(E_RGB_ALL, colors[max(min(distance / 7, 5), 0)]);
      token = T_LISTED_BIN;
      int8_t mid[] = {0,   0,   0,   0,   0,   0,   0,   0,  30,  30, -30, -30,  30,  30, -30, -30,};
      int8_t allParameter[] = { currentAng[0] / 2 - distance / 2, currentAng[1] / 2 + distance / 1.5 , distance * 3  - 50, 0,
                                0, 0, 0, 0,
                                mid[8] - 20 + distance / 2 , mid[9] - 20 + distance, mid[10] - 25 + distance, mid[11] - 25 + distance,
                                mid[12] + 30 - distance, mid[13] + 30 - distance, mid[14] + 50 - distance * 2, mid[15] + 50 - distance * 2
                              };
      //      printList(allParameter);
      cmdLen = 16;
      for (byte i = 0; i < cmdLen; i++)
        newCmd[i] = allParameter[i];
      newCmd[cmdLen] = '\0';
      newCmdIdx = 6;
    }
  }
}
