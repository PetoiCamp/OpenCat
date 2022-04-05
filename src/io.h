//short tools
template <typename T> int8_t sign(T val) {
  return (T(0) < val) - (val < T(0));
}


void printRange(int r0 = 0, int r1 = 0) {
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      PT(i);
      PT('\t');
    }
  else
    for (byte i = r0; i < r1; i++) {
      PT(i);
      PT('\t');
    }
  PTL();
}
template <typename T> void printList(T * arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  PTL(temp);
}
template <typename T> void printTable(T * list) {
  printRange(0, DOF);
  printList(list, DOF);
}

char getUserInputChar() {//take only the first character, allow "no line ending", "newline", "carriage return", and "both NL & CR"
  while (!Serial.available());
  char result = Serial.read();
  delay(1);//wait for the remainder to arrive
  while (Serial.available()) {//flush the '\r' or '\n' if any
    Serial.read();
  }
  return result;
}

int readSerialUntil(char * destination, char terminator) {
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
    newCmdIdx = 2;
    token = Serial.read();
    delay(1); //leave enough time for serial read
    if (token == T_SKILL_DATA) {
      readSerialUntil((char*)dataBuffer, '~');
    }
    else if (Serial.available() > 0) {
      String cmdBuffer;//may overflow after many iterations. I use this method only to support "no line ending" in the serial monitor
      if (token == T_INDEXED_SIMULTANEOUS_BIN || token == T_LISTED_BIN || token == T_MOVE_BIN || token == T_BEEP_BIN) {
        cmdBuffer = Serial.readStringUntil('~');//'~' ASCII code = 126; may introduce bug when the angle is 126
      }
      else
        cmdBuffer = Serial.readStringUntil('\n');
      cmdLen = cmdBuffer.length();
      char *destination = (token == T_SKILL||token == T_TILT) ? newCmd : (char*)dataBuffer;
      for (int i = 0; i < cmdLen; i++)
        destination[i] = cmdBuffer[i];
      destination[cmdLen] = '\0';

#ifdef DEVELOPER
      PTF("Mem:"); PTL(freeMemory());
#endif
    }
//    PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  }
}

void readSignal() {
#ifdef ULTRASONIC
  read_ultrasonic();
#endif

#ifdef RANDOM_MIND
  if (autoSwitch)
    randomMind();//make the robot do random demos
#endif

#if defined IR_PIN
  read_infrared();//  newCmdIdx = 1
#endif
  read_serial();  //  newCmdIdx = 2
#ifdef VOICE
  read_voice();
#endif
#ifdef CAMERA
  read_camera();
#endif
  // powerSaver -> 4
  // randomMind -> 5
  // other -> 6
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
