#include "HardwareSerial.h"
char getUserInputChar() {  //take only the first character, allow "no line ending", "newline", "carriage return", and "both NL & CR"
  while (!Serial.available())
    ;
  char result = Serial.read();
  delay(1);                     //wait for the remainder to arrive
  while (Serial.available()) {  //flush the '\r' or '\n' if any
    Serial.read();
  }
  return result;
}

void read_serial() {
  if (Serial.available() > 0) {
    // serialDominateQ = true;
    token = Serial.read();
    lowerToken = tolower(token);
    newCmdIdx = 2;
    delay(1);  //leave enough time for serial read

    char terminator = (token >= 'A' && token <= 'Z') ? '~' : '\n';  //capitalized tokens use binary encoding for long data commands
                                                                    //'~' ASCII code = 126; may introduce bug when the angle is 126 so only use angles <= 125
    long lastSerialTime = millis();
    int serialTimout = (token == T_SKILL_DATA || lowerToken == T_BEEP) ? SERIAL_TIMEOUT_LONG : SERIAL_TIMEOUT;
    do {
      if (Serial.available()) {
        // long current = millis();
        // PTH("SR\t", current - lastSerialTime);
        do {
          if ((token == T_SKILL || lowerToken == T_INDEXED_SIMULTANEOUS_ASC || lowerToken == T_INDEXED_SEQUENTIAL_ASC) && cmdLen >= spaceAfterStoringData
              || cmdLen >= BUFF_LEN) {
            PTLF("OVF");
            delay(500);
            // beep(5, 100, 50, 5);
            do { Serial.read(); } while (Serial.available());
            PTL(token);
            token = T_SKILL;
            strcpy(newCmd, "up");
            // cmdLen = 2;  //not necessary
            return;
          }
          newCmd[cmdLen++] = Serial.read();
        } while (Serial.available());
        lastSerialTime = millis();
      }
    } while (newCmd[cmdLen - 1] != terminator
             && long(millis() - lastSerialTime) < serialTimout);  //the lower case tokens are encoded in ASCII and can be entered in Arduino IDE's serial monitor
                                                                  //if the terminator of the command is set to "no line ending" or "new line", parsing can be different
                                                                  //so it needs a timeout for the no line ending case
    // PTH("*SR\t", long(millis() - lastSerialTime));
    cmdLen = (newCmd[cmdLen - 1] == terminator) ? cmdLen - 1 : cmdLen;
    newCmd[cmdLen] = (token >= 'A' && token <= 'Z') ? '~' : '\0';
    // PTL(cmdLen);
    // printCmdByType(token, newCmd, cmdLen);
#ifdef DEVELOPER
    PTF("Mem:");
    PTL(freeMemory());
#endif
  }
}
#ifdef MAIN_SKETCH
void readSignal() {
#if defined IR_PIN
  // if (!serialDominateQ)  //serial connection will disable infrared receiver
  read_infrared();  //  newCmdIdx = 1
#endif
  read_serial();  //  newCmdIdx = 2
#ifdef VOICE
  read_voice();
#endif
#ifdef VOICE_LD3320
  read_voice_ld3320();
#endif
  long current = millis();
  if (newCmdIdx && newCmdIdx <= 2) {
    idleTimer = millis() +
#ifdef DOUBLE_INFRARED_DISTANCE
                0
#else
                IDLE_TIME
#endif
      ;
  } else if (token != T_CALIBRATE && current - idleTimer > 0) {
    // serialDominateQ = false;
#ifdef CAMERA
    read_camera();
#endif
#ifdef ULTRASONIC
    readRGBultrasonic();
#endif
#ifdef GESTURE
    read_gesture();
#endif
#ifdef PIR
    read_PIR();
#endif
#ifdef DOUBLE_TOUCH
    read_doubleTouch();
#endif
#ifdef DOUBLE_LIGHT
    read_doubleLight();
#endif
#ifdef DOUBLE_INFRARED_DISTANCE
    read_doubleInfraredDistance();
#endif
#ifdef RANDOM_MIND
    if (autoSwitch && newCmdIdx == 0)
      randomMind();  //make the robot do random demos
#endif
  }
  // powerSaver -> 4
  // other -> 5
  // randomMind -> 100
}
#endif

// #define READING_COUNT 100
// bool soundLightSensorQ = false;
// bool restQ = false;
// int lightLag = 0;

// bool sensorConnectedQ(int n) {
//   float mean = 0;
//   float bLag = analogRead(A3);
//   for (int i = 0; i < READING_COUNT; i++) {
//     int a, b;
//     a = analogRead(A2);
//     b = analogRead(A3);
//     mean = mean + ((a - b) * (a - b) - mean) / (i + 1);

//     if (abs(a - b) > 50 && abs(b - bLag) < 5) {
//       return true;
//     }

//     bLag = b;
//     delay(1);
//   }
//   return sqrt(mean) > 20 ? true : false;
// }
