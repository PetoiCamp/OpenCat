//code provided by the manufacturer
//modified by Rongzhong Li for better demonstration.
//Feb.16, 2021

#include "rgbUltrasonic/RgbUltrasonic.h"
// #define REGULAR_ULTRASONIC
RgbUltrasonic ultrasonic(6, 7);  //(signal, RGB)
//RgbUltrasonic ultrasonic(8, 9);//(signal, RGB)
//The RGB LED ultrasonic module should be plugged in the fourth grove socket with D6, D7

long colors[] = { RGB_RED, RGB_PURPLE, RGB_GREEN, RGB_BLUE, RGB_YELLOW, RGB_WHITE };
long ultraTimer;
int ultraInterval = 1000;
int distance;
#ifdef BITTLE
int feedbackDirection = -1;
#elif defined NYBBLE
int feedbackDirection = 1;
#endif
void readRGBultrasonic() {
  if (millis() - ultraTimer > ultraInterval) {  //|| token == T_SKILL && millis() - ultraTimer > 3000) {
    ultraTimer = millis();
    ultraInterval = 0;
    randomInterval = 1000;
    distance = ultrasonic.GetUltrasonicDistance();
    if (distance > 120) {
      return;
    }

    //Add up multiple distance readings to reduce sensor noise
    static unsigned int totalDistances = 0;
    static byte readings = 0;
    totalDistances += distance;
    readings += 1;	
    if (readings < 10) {
      return;
    }
    //Take the average distance of the readings
    distance = totalDistances / readings;
    //Reset tracking variables
    totalDistances = distance;
    readings = 1;

    if (distance > 90) {
      if (!manualEyeColorQ)
        ultrasonic.SetRgbEffect(E_RGB_ALL, RGB_WHITE, E_EFFECT_BREATHING);
      ultraInterval = 1000;
      //      autoSwitch = true;
      randomInterval = 1000;
    } else if (distance > 70) {
      if (!manualEyeColorQ)
        ultrasonic.SetRgbEffect(E_RGB_ALL, RGB_YELLOW, E_EFFECT_ROTATE);
    } else if (distance > 50) {
      if (!manualEyeColorQ)
        ultrasonic.SetRgbEffect(E_RGB_ALL, RGB_BLUE, E_EFFECT_FLASH);
    } else if (distance < 3) {
      ultraInterval = 2000;
      randomInterval = 5000;
      tQueue->addTask('k', "str", 1000);
      tQueue->addTask('k', "vtF", 1500);
      tQueue->addTask('k', "up");
    } else if (distance < 6) {
      if (!manualEyeColorQ)
        ultrasonic.SetRgbEffect(E_RGB_ALL, RGB_RED, E_EFFECT_FLASH);
      meow(random() % 3 + 1, distance * 2);
      int8_t allRand[] = { 0, int8_t(currentAng[0] + random() % 20 - 10), 1, int8_t(currentAng[1] + random() % 20 - 10), 2, int8_t(currentAng[2] + random() % 20 - 10) };
      //      for (byte i = 0; i < cmdLen; i++)
      //        newCmd[i] = (int8_t)allRand[i];
      arrayNCPY(newCmd, allRand, cmdLen);
      newCmd[cmdLen] = '~';
      tQueue->addTask('I', newCmd);
      tQueue->addTask('i', "");
    } else if (distance < 10) {
      if (!manualEyeColorQ)
        ultrasonic.SetRgbColor(E_RGB_ALL, RGB_RED);
      tQueue->addTask('k', "sit", 2000);
      tQueue->addTask('k', "up");
    }

    else {  //10~50
      distance -= 9;
      if (!manualEyeColorQ)
        ultrasonic.SetRgbColor(E_RGB_ALL, colors[int(max(min(distance / 10, 5), 0))]);
#ifdef BITTLE
      int mid[] = {
        15,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        30,
        30,
        90,
        90,
        30,
        30,
        -30,
        -30,
      };
#elif defined NYBBLE
      int mid[] = {
        15,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        30,
        30,
        -30,
        -30,
        30,
        30,
        -30,
        -30,
      };
#endif
      int servoAngle[] = { 
		mid[0] - distance / 2,			//Head pan
	-10 + distance / 2,				//Head tilt (Nybble only)
	distance * (random() % 50 < 1 ? int(random() % 2 - 1) : 1),	//Tail (Nybble only)
	0, 0, 0, 0, 0,
	mid[8] - 15 + distance / 2, 			//Front upper legs
	mid[9] - 15 + distance / 2, 
	mid[10] - 30 + distance * feedbackDirection, 	//Back upper legs
	mid[11] - 30 + distance * feedbackDirection,
	mid[12] + 35 - distance, 			//Front lower legs
	mid[13] + 35 - distance, 
	mid[14] + 40 - distance * feedbackDirection, 	//Back lower legs
	mid[15] + 40 - distance * feedbackDirection 
      };
      //      printList(servoAngle);
      cmdLen = 16;
      for (byte i = 0; i < cmdLen; i++)
        newCmd[i] = (int8_t)min(max(servoAngle[i], -128), 127);
      newCmd[cmdLen] = '~';
      randomInterval = 5000;
      tQueue->addTask('L', newCmd);
    }
  }
}

float readUltrasonic(int trigger, int echo = -1) {  //give two parameters for the traditional ultrasonic sensor
                                                    //give one parameter for the one pin ultrasonic sensor that shares the trigger and echo pins
  if (echo == -1)
    echo = trigger;
  int longestDistance = 200;  // 200 cm = 2 meters
  float farTime = longestDistance * 2 / 0.034;
  pinMode(trigger, OUTPUT);

  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  pinMode(echo, INPUT);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echo, HIGH, farTime);
  // Calculating the distance
  return duration * 0.034 / 2;  // 10^-6 * 34000 cm/s
}
