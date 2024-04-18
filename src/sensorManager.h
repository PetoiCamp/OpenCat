#define MAX_READING 1024.0
#define BASE_RANGE 1024.0

#ifdef VOICE
#include "voice.h"
#endif

#ifdef VOICE_LD3320
#include "voiceLD3320.h"
#endif

#ifdef CAMERA
#include "camera.h"
#endif

#ifdef ULTRASONIC
#include "ultrasonic.h"
#endif

#ifdef GESTURE
#include "gesture.h"
#endif

#ifdef PIR
#include "pir.h"
#endif

#ifdef DOUBLE_TOUCH
#include "doubleTouch.h"
#endif

#ifdef DOUBLE_LIGHT
#include "doubleLight.h"
#endif

#ifdef DOUBLE_INFRARED_DISTANCE
#include "doubleInfraredDistance.h"
#endif

#ifdef GROVE_SERIAL_PASS_THROUGH
#define ULTRASONIC
#include "ultrasonic.h"
#endif

#ifdef OTHER_MODULES
#endif

#ifdef ALL_RANDOM
#else
// #define GYRO_PIN 0
#endif


int8_t moduleList[] = {
  EXTENSION_DOUBLE_TOUCH,
  EXTENSION_DOUBLE_LIGHT,
  EXTENSION_DOUBLE_IR_DISTANCE,
  EXTENSION_PIR,
  EXTENSION_ULTRASONIC,
  EXTENSION_GESTURE,
  EXTENSION_CAMERA_MU3,
  EXTENSION_VOICE
};
bool *sensorActivatedQ;
int8_t indexOfSensor(char sensorName) {
  for (byte i = 0; i < sizeof(moduleList) / sizeof(char); i++)
    if (sensorName == moduleList[i])
      return i;
  return -1;
}
int8_t activeSensorIdx() {
  for (byte i = 0; i < sizeof(moduleList) / sizeof(char); i++)
    if (sensorActivatedQ[i])
      return i;
  return -1;
}
void initModule(byte idx) {
  switch (moduleList[idx]) {
#ifdef VOICE
    case EXTENSION_VOICE:
      {
        voiceSetup();
        break;
      }
#endif
#ifdef ULTRASONIC
    case EXTENSION_ULTRASONIC:
      {
        PT('=');
        PTL(readUltrasonic((int8_t)newCmd[1], (int8_t)newCmd[2]));
        break;
      }
#endif
#ifdef DOUBLE_TOUCH
    case EXTENSION_DOUBLE_TOUCH:
      {
        touchSetup();
        break;
      }
#endif
#ifdef DOUBLE_LIGHT
    case EXTENSION_DOUBLE_LIGHT:
      {
        doubleLightSetup();
        break;
      }
#endif
#ifdef DOUBLE_IR_DISTANCE
    case EXTENSION_DOUBLE_IR_DISTANCE:
      {
        doubleInfraredDistanceSetup();
        break;
      }
#endif
#ifdef PIR
    case EXTENSION_PIR:
      {
        pirSetup();
        break;
      }
#endif
#ifdef GESTURE
    case EXTENSION_GESTURE:
      {
        gestureSetup();
        break;
      }
#endif
#ifdef CAMERA
    case EXTENSION_CAMERA_MU3:
      {
        cameraSetup();
        break;
      }
#endif
  }
}
void reconfigureTheActiveModule(byte idx) {  // negative number will deactivate all the sensors
  for (byte i = 0; i < sizeof(moduleList) / sizeof(char); i++)
    if (i == idx) {
      initModule(idx);
      sensorActivatedQ[i] = true;
    } else {
      if (sensorActivatedQ[i]) {
        // stopModule();//no need for now
        sensorActivatedQ[i] = false;
      }
    }
}

void showSensorStatus() {
  byte sensorCount = sizeof(moduleList) / sizeof(char);
  printListWithoutString((char *)moduleList, sensorCount);
  printListWithoutString(sensorActivatedQ, sensorCount);
}
void initSensorManager() {
  byte sensorCount = sizeof(moduleList) / sizeof(char);
  PTL(sensorCount);
  sensorActivatedQ = new bool[sensorCount];
  for (byte i = 0; i < sensorCount; i++)
    sensorActivatedQ[i] = false;
  showSensorStatus();
}