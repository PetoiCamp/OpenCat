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

void resetCmd() {
  if (strcmp(newCmd, "rc")) {
    delete[] lastCmd;
    lastCmd = new char(strlen(newCmd) + 1);
    strcpy(lastCmd, newCmd);
  }
  newCmdIdx = 0;
  lastToken = token;
  if (token != T_SKILL && token != T_CALIBRATE)
    token = '\0';
  newCmd[0] = '\0';
  cmdLen = 0;
}

void read_serial() {
  if (Serial.available() > 0) {
    serialDominateQ = true;
    token = Serial.read();
    delay(1);  //leave enough time for serial read

    bufferPtr = (token == T_SKILL || token == T_INDEXED_SIMULTANEOUS_BIN) ? newCmd : (char *)dataBuffer;                           // save in a independent memory to avoid breaking the current running skill
    char terminator = (token < 'a') ? '~' : '\n';                                                                                  //capitalized tokens use binary encoding for long data commands
                                                                                                                                   //'~' ASCII code = 126; may introduce bug when the angle is 126 so only use angles <= 125
    int timeout = (token == T_SKILL_DATA || token == T_BEEP_BIN || token == T_BEEP) ? SERIAL_TIMEOUT_LONG : SERIAL_TIMEOUT_SHORT;  //the lower case tokens are encoded in ASCII and can be entered in Arduino IDE's serial monitor
                                                                                                                                   //if the terminator of the command is set to "no line ending" or "new line", parsing can be different
                                                                                                                                   //so it needs a timeout for the no line ending case
    long lastTime = 0;
    do {
      if (Serial.available()) {
        if (cmdLen >= CMD_LEN && bufferPtr == newCmd || cmdLen >= BUFF_LEN && bufferPtr == (char *)dataBuffer) {  //} || token == T_INDEXED_SIMULTANEOUS_ASC)) {
          PTLF("OVF");                                                                                            //when it overflows, the head value of dataBuffer will be changed. why???
          do { Serial.read(); } while (Serial.available());
          PTL(token);
          token = T_SKILL;
          strcpy(newCmd, "vtF");
          cmdLen = 7;
          break;
        }
        bufferPtr[cmdLen++] = Serial.read();
        lastTime = millis();
      }
    } while ((char)bufferPtr[cmdLen - 1] != terminator && long(millis() - lastTime) < timeout);
    cmdLen = (bufferPtr[cmdLen - 1] == terminator) ? cmdLen - 1 : cmdLen;
    bufferPtr[cmdLen] = '\0';
    newCmdIdx = 2;
    PTL(cmdLen);

#ifdef DEVELOPER
    PTF("Mem:");
    PTL(freeMemory());
#endif
    // PTL("lastT:" + String(lastToken) + Z"\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  }
}

void readSignal() {
#if defined IR_PIN
  if (!serialDominateQ)  //serial connection will disable infrared receiver
    read_infrared();     //  newCmdIdx = 1
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
    serialDominateQ = false;

#ifdef CAMERA
    read_camera();
#endif
#ifdef ULTRASONIC
    read_ultrasonic();
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
#ifdef RANDOM_MIND
    if (autoSwitch && newCmdIdx == 0)
      randomMind();  //make the robot do random demos
#endif
  }
  // powerSaver -> 4
  // other -> 5
  // randomMind -> 100
}

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
