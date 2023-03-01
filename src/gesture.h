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
    PTLF("Error initializing APDS-9960 sensor!");
  }
  // for setGestureSensitivity(..) a value between 1 and 100 is required.
  // Higher values make the gesture recognition more sensitive but less accurate
  // (a wrong gesture may be detected). Lower values makes the gesture recognition
  // more accurate but less sensitive (some gestures may be missed).
  // Default is 80
  //APDS.setGestureSensitivity(80);

  PTLF("Detecting gestures ...");
}
int8_t melody54321[] = { 19, 64, 17, 64, 16, 64, 14, 64, 12, 32, '~' };
int8_t melody12345[] = { 12, 64, 14, 64, 16, 64, 17, 64, 19, 32, '~' };
int8_t melody67345[] = { 21, 16, 23, 32, 16, 64, 17, 64, 19, 64, '~' };
int8_t melody32654[] = { 16, 64, 14, 16, 21, 64, 19, 32, 17, 16, '~' };
void read_gesture() {
  if (APDS.gestureAvailable()) {
    // a gesture was detected, read and print to Serial Monitor
    int gesture = APDS.readGesture();
    PTF("Detected ");
    switch (gesture) {
      case GESTURE_UP:
        {
          PTLF("UP\t↑");
          tQueue->push_back(new Task(T_BEEP_BIN, melody12345, 0));
          tQueue->push_back(new Task('k', "fiv", 2000));
          tQueue->push_back(new Task('k', "up"));
          break;
        }

      case GESTURE_DOWN:
        {
          PTLF("DOWN\t↓");
          tQueue->push_back(new Task('i', ""));
          tQueue->push_back(new Task(T_BEEP_BIN, melody54321, 0));
          tQueue->push_back(new Task('k', "sit"));

          break;
        }

      case GESTURE_LEFT:
        {
          PTLF("LEFT\t←");
          int8_t move[] = { 0, -70, 0, -65, '~' };
          tQueue->push_back(new Task(T_BEEP_BIN, melody32654, 0));
          tQueue->push_back(new Task(T_INDEXED_SEQUENTIAL_BIN, move, 1000));
          break;
        }
      case GESTURE_RIGHT:
        {
          PTLF("RIGHT\t→");
          int8_t move[] = { 0, 70, 0, 65, '~' };
          tQueue->push_back(new Task(T_BEEP_BIN, melody67345, 0));
          tQueue->push_back(new Task(T_INDEXED_SEQUENTIAL_BIN, move, 0));
          break;
        }
      default:
        // ignore
        break;
    }
  }
}
