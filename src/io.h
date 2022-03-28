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
#ifdef GYRO_PIN
  if (token != T_SKILL && !checkGyro)
#endif
    delay(3);//leave enough time for serial read
  if (Serial.available() > 0) {
    newCmdIdx = 2;
    token = Serial.read();

    if (token == T_SKILL_DATA)
      readSerialUntil(dataBuffer, '~');
    else if (Serial.available() > 0) {
      String cmdBuffer;//may overflow after many iterations. I use this method only to support "no line ending" in the serial monitor
      if (token == T_INDEXED_SIMULTANEOUS_BIN || token == T_LISTED_BIN || token == T_MOVE_BIN || token == T_BEEP_BIN) {
        cmdBuffer = Serial.readStringUntil('~');//'~' ASCII code = 126; may introduce bug when the angle is 126
      }
      else
        cmdBuffer = Serial.readStringUntil('\n');
      cmdLen = cmdBuffer.length();
      char *destination = (token == T_SKILL) ? newCmd : dataBuffer;
      for (int i = 0; i < cmdLen; i++)
        destination[i] = cmdBuffer[i];
      destination[cmdLen] = '\0';
      //      PTL("lastT: " + String(lastToken) + "\tT: " + String(token) + "\tLastCmd: " + String(lastCmd) + "\tCmd: " + String(newCmd));
#ifdef DEVELOPER
      PTF(" memory "); PTL(freeMemory());
#endif
    }
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


//short tools


void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.update(p_address, lowByte);
  EEPROM.update(p_address + 1, highByte);
  delay(5);
}
//This function will read a 2-byte integer from the EEPROM at the specified address and address + 1
int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
inline int8_t eeprom(int address, byte x = 0, byte y = 0, byte yDim = 1) {
  return EEPROM.read(address + x * yDim + y);
}

void flushEEPROM(char b) {
  for (int j = 0; j < 1024; j++) {
    EEPROM.update(j, b);
  }
}

void printEPROM() {
  for (int i = 0; i < 1024; i++) {
    PT(eeprom(i));
    PT('\t');
    if ((i % 16) == 15)
      PTL();
  }
}

template <typename T> void printEEPROMList(int EEaddress, byte len = DOF) {
  for (byte i = 0; i < len; i++) {
    PT((T)(EEPROM.read(EEaddress + i)));
    PT('\t');
  }
  PTL();
}

void saveCalib(int8_t *var) {
  for (byte i = 0; i < DOF; i++) {
    EEPROM.update(CALIB + i, var[i]);
    //    calibratedZeroPosition[i] = EEPROMReadInt(ZERO_POSITIONS + i * 2) + float(var[i]) * eeprom(ROTATION_DIRECTION, i);
    EEPROMWriteInt(CALIBRATED_ZERO_POSITIONS + i * 2, EEPROMReadInt(ZERO_POSITIONS + i * 2) + float(var[i]) * eeprom(ROTATION_DIRECTION, i));
  }
}

void saveMelody(int &address, byte melody[], int len ) {
  EEPROM.update(address--, len);
  for (byte i = 0; i < len; i++)
    EEPROM.update(address --, melody[i]);
  PTL(address);
}


inline int loadAngleLimit(byte idx, byte dim) {
  return EEPROMReadInt(ANGLE_LIMIT + idx * 4 + dim * 2);
}
