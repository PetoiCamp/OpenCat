/*
  APDS-9960 - Gesture Sensor
  https://www.arduino.cc/reference/en/libraries/arduino_apds9960/

  This example reads gesture data from the on-board APDS-9960 sensor of the
  Nano 33 BLE Sense and prints any detected gestures to the Serial Monitor.

  Gesture directions are as follows:
  - UP:    from USB connector towards antenna
  - DOWN:  from antenna towards USB connector
  - LEFT:  from analog pins side towards digital pins side
  - RIGHT: from digital pins side towards analog pins side

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/

#include "gesture9960/Arduino_APDS9960.h"

void gestureSetup() {
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor!");
  }
  // for setGestureSensitivity(..) a value between 1 and 100 is required.
  // Higher values make the gesture recognition more sensitive but less accurate
  // (a wrong gesture may be detected). Lower values makes the gesture recognition
  // more accurate but less sensitive (some gestures may be missed).
  // Default is 80
  //APDS.setGestureSensitivity(80);

  Serial.println("Detecting gestures ...");
}

void read_gesture() {
  if (APDS.gestureAvailable()) {
    // a gesture was detected, read and print to Serial Monitor
    int gesture = APDS.readGesture();

    newCmdIdx = 5;
    switch (gesture) {
      case GESTURE_UP:
        Serial.println("↑ Detected UP gesture");
        token = T_SKILL;
        strcpy(newCmd, "hi");
        break;

      case GESTURE_DOWN:
        Serial.println("↓ Detected DOWN gesture");
        token = T_SKILL;
        strcpy(newCmd, "sit");
        break;

      case GESTURE_LEFT:
        Serial.println("← Detected LEFT gesture");
        token = T_INDEXED_SEQUENTIAL_ASC;
        strcpy((char*)dataBuffer, "0 -50 0 0");
        break;

      case GESTURE_RIGHT:
        Serial.println("→ Detected RIGHT gesture");
        token = T_BEEP;
        strcpy((char*)dataBuffer, "12 8 14 8 16 8 17 8 19 4");
        break;

      default:
        // ignore
        break;
    }
  }
}
