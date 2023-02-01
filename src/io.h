
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

int readSerialUntil(char *destination, char terminator) {
  int c = 0;
  do {
    if (Serial.available())
      destination[c++] = Serial.read();
  } while ((char)destination[c - 1] != terminator);
  destination[c - 1] = '\0';
  return c - 1;
}

void read_serial() {
  if (Serial.available() > 0) {
    serialConnectedQ = true;
    newCmdIdx = 2;
    token = Serial.read();
    delay(1);  //leave enough time for serial read
#ifdef T_SKILL_DATA
    if (token == T_SKILL_DATA) {
      readSerialUntil((char *)dataBuffer, '~');
    } else
#endif
      if (Serial.available() > 0) {
      String cmdBuffer;  //may overflow after many iterations. I use this method only to support "no line ending" in the serial monitor
      if (token == T_COLOR || token == T_INDEXED_SIMULTANEOUS_BIN || token == T_INDEXED_SEQUENTIAL_BIN
#ifdef BINARY_COMMAND
          || token == T_LISTED_BIN || token == T_BEEP_BIN
#endif
      ) {
        cmdBuffer = Serial.readStringUntil('~');  //'~' ASCII code = 126; may introduce bug when the angle is 126
      } else

        cmdBuffer = Serial.readStringUntil('\n');
      cmdLen = cmdBuffer.length();
      char *destination = (token == T_SKILL || token == T_TILT) ? newCmd : (char *)dataBuffer;
      //      for (int i = 0; i < cmdLen; i++)
      //        destination[i] = cmdBuffer[i];
      arrayNCPY(destination, cmdBuffer.c_str(), cmdLen);
      destination[cmdLen] = '\0';

#ifdef DEVELOPER
      PTF("Mem:");
      PTL(freeMemory());
#endif
    }
    //    PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  }
}

void readSignal() {
#if defined IR_PIN
  if (!serialConnectedQ)  //serial connection will disable infrared receiver
    read_infrared();      //  newCmdIdx = 1
#endif
  read_serial();  //  newCmdIdx = 2
#ifdef VOICE
  read_voice();
#endif
#ifdef VOICE_LD3320
  read_voice_ld3320();
#endif
  long current = millis();
  if (newCmdIdx) {
    idleTimer = millis() + IDLE_TIME;
  } else if (token != T_CALIBRATE && current - idleTimer > 0) {
    serialConnectedQ = false;

#ifdef CAMERA
    read_camera();
#endif
#ifdef ULTRASONIC
    read_ultrasonic();
#endif
#ifdef GESTURE
    read_gesture();
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

#define READING_COUNT 100
bool soundLightSensorQ = false;
bool restQ = false;
int lightLag = 0;

bool sensorConnectedQ(int n) {
  float mean = 0;
  float bLag = analogRead(A3);
  for (int i = 0; i < READING_COUNT; i++) {
    int a, b;
    a = analogRead(A2);
    b = analogRead(A3);
    mean = mean + ((a - b) * (a - b) - mean) / (i + 1);

    if (abs(a - b) > 50 && abs(b - bLag) < 5) {
      return true;
    }

    bLag = b;
    delay(1);
  }
  return sqrt(mean) > 20 ? true : false;
}
