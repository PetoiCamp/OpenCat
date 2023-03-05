#define WIRE_BUFFER 30  //Arduino wire allows a 32-byte buffer, with 2 bytes for address.
#define WIRE_LIMIT 16   //That leaves 30 bytes for data. Use 16 to balance each writes
#define PAGE_LIMIT 32   //AT24C32D 32-byte Page Write Mode. Partial Page Writes Allowed
#define EEPROM_SIZE (65535 / 8)
bool EEPROMOverflow = false;

class Skill {
public:
  byte jointIndex;
  char* skillName;  //use char array instead of String to save memory
  int8_t offsetLR;
  int8_t period;  //the period of a skill. 1 for posture, >1 for gait, <-1 for behavior
  float transformSpeed;
  byte skillHeader;
  byte frameSize;
  int expectedRollPitch[2];  //expected body orientation (roll, pitch)
  byte angleDataRatio;       //divide large angles by 1 or 2. if the max angle of skill is >128, all the angles will be divided by 2
  byte loopCycle[3];         //the looping section of a behavior (starting row, ending row, repeating cycles)
  byte firstMotionJoint;
  int8_t* dutyAngles;  //the data array for skill angles and parameters

  Skill() {
    //      period = 0;
    //      transformSpeed = 2;
    //      expectedRollPitch[0] = yprTilt[2] = 0;
    //      expectedRollPitch[1] = yprTilt[1] = 0;
    //      dutyAngles = NULL;
    //      jointIndex = 0;
  }

  void copyDataFromBufferToI2cEeprom(unsigned int eeAddress, int8_t* newCmd) {  // eeAddress should not be changed out of this function
    period = (int8_t)newCmd[0];                                                 //automatically cast to char*
    int len = dataLen(period) + 1;
    if (eeAddress + len >= EEPROM_SIZE) {
      PTLF("OVF! Won't save!");
      EEPROMOverflow = true;
#ifdef BUZZER
      beep(10, 100, 100, 5);
#endif
      return;
    }
    int writtenToEE = 0;
    while (len > 0) {
      Wire.beginTransmission(DEVICE_ADDRESS);
      Wire.write((int)((eeAddress) >> 8));  // MSB
      Wire.write((int)((eeAddress)&0xFF));  // LSB
      byte writtenToWire = 0;
      do {

        Wire.write((byte)newCmd[writtenToEE++]);
        writtenToWire++;
        eeAddress++;
      } while ((--len > 0) && (eeAddress % PAGE_LIMIT) && (writtenToWire < WIRE_LIMIT));  //be careful with the chained conditions
      //self-increment may not work as expected
      Wire.endTransmission();
      delay(6);  // needs 5ms for page write
      //PTL("\nwrote " + String(writtenToWire) + " bytes.");
    }
    //PTLF("finish copying to I2C EEPROM");
  }
#ifndef MAIN_SKETCH
  void copyDataFromPgmToI2cEeprom(unsigned int& eeAddress, unsigned int pgmAddress) {  // eeAddress should be updated after this function
    period = (int8_t)pgm_read_byte(pgmAddress);                                        //automatically cast to char*
    int len = dataLen(period) + 1;
    if (eeAddress + len >= EEPROM_SIZE) {
      PTLF("OVF! Delete some skills!");
      EEPROMOverflow = true;
#ifdef BUZZER
      beep(10, 100, 100, 5);
#endif
      return;
    }
    int writtenToEE = 0;
    while (len > 0) {
      Wire.beginTransmission(DEVICE_ADDRESS);
      Wire.write((int)((eeAddress) >> 8));  // MSB
      Wire.write((int)((eeAddress)&0xFF));  // LSB
      byte writtenToWire = 0;
      do {

        Wire.write((byte)pgm_read_byte(pgmAddress + writtenToEE++));
        writtenToWire++;
        eeAddress++;
      } while ((--len > 0) && (eeAddress % PAGE_LIMIT) && (writtenToWire < WIRE_LIMIT));  //be careful with the chained conditions
      //self-increment may not work as expected
      Wire.endTransmission();
      delay(6);  // needs 5ms for page write
      //PTL("\nwrote " + String(writtenToWire) + " bytes.");
    }
    //PTLF("finish copying to I2C EEPROM");
  }

  void saveSkillInfoFromProgmemToOnboardEeprom() {
    int skillAddressShift = 0;
    unsigned int i2cEepromAddress = INITIAL_SKILL_DATA_ADDRESS;  //won't hurt if unused
    unsigned int theLastEEaddress;
    PTLF("Save skills");
    EEPROM.update(NUM_SKILLS, sizeof(skillNameWithType) / 2);
    for (byte s = 0; s < eeprom(NUM_SKILLS); s++) {  //save skill info to on-board EEPROM
      theLastEEaddress = i2cEepromAddress;
      byte len = strlen(skillNameWithType[s]);               //len includes type. the saved value doesn't
      EEPROM.update(SKILLS + skillAddressShift++, len - 1);  //the last char in name denotes skill type, I(nstinct) on external eeprom, N(ewbility) on progmem
      // PT(skillNameWithType[s][len - 1]== 'I' ? "I nstinct\t" : "N ewbility\t");
      for (byte l = 0; l < len; l++) {
        PT(skillNameWithType[s][l]);
        EEPROM.update(SKILLS + skillAddressShift++, skillNameWithType[s][l]);
      }
      PTL();
      int8_t period = pgm_read_byte(progmemPointer[s]);
      EEPROM.update(SKILLS + skillAddressShift++, period);

      //PTL("Current EEPROM address is " + String(SKILLS + skillAddressShift));
#ifdef I2C_EEPROM
      if (!EEPROMOverflow)
        if (skillNameWithType[s][len - 1] == 'I') {  //  if there's instinct and there's i2c eeprom, and user decide to update.
          // save the data array to i2c eeprom. its address will be saved to onboard eeprom
          EEPROMWriteInt(SKILLS + skillAddressShift, i2cEepromAddress);
          copyDataFromPgmToI2cEeprom(i2cEepromAddress, (unsigned int)progmemPointer[s]);
          //            PT(int(SKILLS + skillAddressShift + 2)); PT(" I2C EE "); PTL(int(i2cEepromAddress));
        }
#endif
      skillAddressShift += 2;  // one int (2 bytes) for address
    }
    // EEPROMWriteInt(SERIAL_BUFF, (unsigned int)i2cEepromAddress);  //save the I2C EEPROM tail address for saving a 'K' skill received from the serial port.
    //However, it will cause segfault when 'T' is called for the first time, because it has never been set by a 'K'. The following solution is used.
    EEPROMWriteInt(SERIAL_BUFF, theLastEEaddress);  //save the address of the last skill (named "zz") that is only used when calling token 'T' for the first time. the future 'K' command will overwrite it.
    PTL();
#ifdef I2C_EEPROM
    {
      PTF("Instincts used ");
      PT(INITIAL_SKILL_DATA_ADDRESS);
      PT('~');
      PT(i2cEepromAddress);
      PT('/');
      PT(EEPROM_SIZE);
      PTLF(" on I2C EEPROM\n");
    }
#endif
  }
#endif

  void assignSkillAddressToOnboardEeprom() {
    int skillAddressShift = 0;
    PTLF("Skill");
    for (byte s = 0; s < sizeof(progmemPointer) / 2; s++) {  //save skill info to on-board EEPROM, load skills to SkillList
      //        PTL(s);
      byte nameLen = EEPROM.read(SKILLS + skillAddressShift++);  //without last type character
      //    for (int n = 0; n < nameLen; n++)
      //      PT((char)EEPROM.read(SKILLS + skillAddressShift + n));
      skillAddressShift += nameLen;
      char skillType = EEPROM.read(SKILLS + skillAddressShift++);
      //    PT(skillType);
      //    PT('\t');
      //    PTL((int8_t)EEPROM.read(SKILLS + skillAddressShift));
#ifdef I2C_EEPROM
      if (skillType == 'N')  // the address of I(nstinct) has been written in previous operation: saveSkillNameFromProgmemToOnboardEEPROM() in instinct.ino
                             // if skillType == N(ewbility), save pointer address of progmem data array to onboard eeprom.
                             // it has to be done for different sketches because the addresses are dynamically assigned
#endif
        EEPROMWriteInt(SKILLS + skillAddressShift + 1, (int)progmemPointer[s]);
#if defined(I2C_EEPROM) && defined(MAIN_SKETCH)
      else
        s--;
#endif
      skillAddressShift += 3;
    }
    //  PTLF("Load finished!");
  }

  int lookupAddressByName(const char* key) {
    int skillAddressShift = 0;
    byte nSkills = eeprom(NUM_SKILLS);
    byte randSkillIdx = strcmp(key, "x") ? nSkills : random(nSkills);
    byte keyLen = strlen(key);
    char lr = key[keyLen - 1];
    for (byte s = 0; s < nSkills; s++) {  //save skill info to on-board EEPROM, load skills to SkillList
      byte nameLen = EEPROM.read(SKILLS + skillAddressShift++);
      char* readName = new char[nameLen + 1];
      for (byte l = 0; l < nameLen; l++) {
        readName[l] = EEPROM.read(SKILLS + skillAddressShift++);
      }
      readName[nameLen] = '\0';
      if (s == randSkillIdx          //random skill
          || !strcmp(readName, key)  //exact match: gait type + F or L, behavior
          // || readName[nameLen - 1] == 'L' && !strncmp(readName, key, nameLen - 1)  //gait type + R
          || readName[nameLen - 1] != 'F' && strcmp(readName, "bk") && !strncmp(readName, key, keyLen - 1) && (lr == 'L' || lr == 'R' || lr == 'X')  // L, R or X
      ) {
        delete[] readName;
        period = EEPROM.read(SKILLS + skillAddressShift + 1);
        int bufferLen = dataLen(period);
        return SKILLS + skillAddressShift;
      }
      delete[] readName;
      skillAddressShift += 4;  //1 byte type, 1 byte period, 1 int address
    }
    PT('?');  //key not found
    PTL(key);
    return -1;
  }

  int dataLen(int8_t p) {
    skillHeader = p > 0 ? 4 : 7;
    frameSize = p > 1 ? WALKING_DOF :  //gait
                  p == 1 ? DOF
                         :  //posture
                  DOF + 4;  //behavior
    int len = skillHeader + abs(p) * frameSize;
    return len;
  }

  void loadDataFromProgmem(unsigned int pgmAddress) {
    //      newCmd = new int8_t [bufferLen + 1];
    //      newCmd[0] = pgm_read_byte(pgmAddress++);
    int bufferLen = dataLen(period);
    for (int i = 0; i < bufferLen; i++)
      newCmd[i] = pgm_read_byte(pgmAddress++);
    //      newCmd[bufferLen] = '\0';
  }

  void loadDataFromI2cEeprom(unsigned int eeAddress) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((eeAddress) >> 8));  // MSB
    Wire.write((int)((eeAddress)&0xFF));  // LSB
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)DEVICE_ADDRESS, (uint8_t)1);
    period = newCmd[0] = Wire.read();
    int bufferLen = dataLen(period);
    //      int tail = bufferLen;
    int readFromEE = 0;
    int readToWire = 0;
    while (bufferLen > 0) {
      //PTL("request " + String(min(WIRE_BUFFER, len)));
      Wire.requestFrom((uint8_t)DEVICE_ADDRESS, (uint8_t)min(WIRE_BUFFER, bufferLen));
      readToWire = 0;
      do {
        if (Wire.available()) newCmd[1 + readFromEE++] = Wire.read();
        /*PT( (int8_t)dutyAngles[readFromEE - 1]);
            PT('\t')*/
      } while (--bufferLen > 0 && ++readToWire < WIRE_BUFFER);
      //PTL();
    }
    //      newCmd[tail] = '\0';
  }
  void inplaceShift() {
    int angleLen = abs(period) * frameSize;                 // need one extra byte for terminator '~'
    int shiftRequiredByNewCmd = CMD_LEN - skillHeader + 1;  // required shift to store CMD_LEN + 1 chars. it can hold a command with CMD_LEN chars. the additioanl byte is required by '\0'.
    spaceAfterStoringData = BUFF_LEN - angleLen - 1;        // the bytes before the dutyAngles. The allowed command's bytes needs to -1
    // PTH("request", shiftRequiredByNewCmd);
    // PTH("aloShft", BUFF_LEN - (skillHeader + angleLen));
    // if (CMD_LEN > spaceAfterStoringData)
    //   PTF("LMT");
    // PTL(spaceAfterStoringData);
    for (int i = 0; i <= angleLen; i++)
      newCmd[BUFF_LEN - i] = newCmd[skillHeader + angleLen - i];
    newCmd[BUFF_LEN] = '~';
    dutyAngles = (int8_t*)newCmd + BUFF_LEN - angleLen;
  }
  void formatSkill() {
    transformSpeed = 2;
    for (int i = 0; i < 2; i++) {
      expectedRollPitch[i] = (int8_t)newCmd[1 + i];
      yprTilt[2 - i] = 0;
    }
    angleDataRatio = newCmd[3];
    byte baseHeader = 4;
    if (period < 0) {
      for (byte i = 0; i < 3; i++)
        loopCycle[i] = newCmd[baseHeader + i];
    }
#ifdef POSTURE_WALKING_FACTOR
    postureOrWalkingFactor = (period == 1 ? 1 : POSTURE_WALKING_FACTOR);
#endif
    firstMotionJoint = (period <= 1) ? 0 : DOF - WALKING_DOF;
    // dutyAngles = newCmd + skillHeader;
    inplaceShift();
#ifdef DEVELOPER
    info();
#endif
  }

  //    void skillByName(const char* skillName, byte angleDataRatio = 1, float speedRatio = 1, bool shutServoAfterward = true) {
  //      loadFrame(skillName);
  //      transform(dutyAngles, angleDataRatio, speedRatio);
  //      if (shutServoAfterward) {
  //        pwm.shutServos();
  //        token = 'd';
  //      }
  //    }

  void loadFrameByDataBuffer() {
    //      period = newCmd[0];
    //      dataLen(period);
    formatSkill();
    frame = 0;
    transform(dutyAngles + frame * frameSize, angleDataRatio, transformSpeed, firstMotionJoint);
  }

  void loadFrame(const char* skillName) {  //get lookup information from on-board EEPROM and read the data array from storage
    char lr = skillName[strlen(skillName) - 1];
    offsetLR = (lr == 'L' ? 25 : (lr == 'R' ? -25 : 0));
    int onBoardEepromAddress = lookupAddressByName(skillName);
    if (onBoardEepromAddress != -1) {
      char skillType = EEPROM.read(onBoardEepromAddress);  //load data by onboard EEPROM address
      unsigned int dataArrayAddress = EEPROMReadInt(onBoardEepromAddress + 2);
      char tempName[CMD_LEN + 1];
      strcpy(tempName, skillName);
#ifdef I2C_EEPROM
      if (skillType == 'I')  //copy instinct data array from external i2c eeprom
        loadDataFromI2cEeprom(dataArrayAddress);
      else  //copy newbility data array from progmem
#endif
        loadDataFromProgmem(dataArrayAddress);
      formatSkill();
      strcpy(newCmd, tempName);
      if (strcmp(newCmd, "calib") && period == 1)
        protectiveShift = random() % 100 / 10.0 - 5;
      else
        protectiveShift = 0;
      for (byte i = 0; i < DOF; i++)
        dutyAngles[i] += protectiveShift;
      // info();
      if (lr == 'R' || (lr == 'X' || lr != 'L') && random(100) % 2)
        mirror();
      frame = 0;
      transform(dutyAngles + frame * frameSize, angleDataRatio, transformSpeed, firstMotionJoint);
#ifdef NYBBLE
      for (byte i = 0; i < HEAD_GROUP_LEN; i++)
        targetHead[i] = currentAng[i];
#endif
    }
  }
  void mirror() {
    expectedRollPitch[0] = -expectedRollPitch[0];
    for (int k = 0; k < abs(period); k++) {
      if (period <= 1) {
        dutyAngles[k * frameSize] = -dutyAngles[k * frameSize];
        dutyAngles[k * frameSize + 2] = -dutyAngles[k * frameSize + 2];
      }
      for (byte col = (period > 1) ? 0 : 2; col < ((period > 1) ? WALKING_DOF : DOF) / 2; col++) {
        int8_t temp = dutyAngles[k * frameSize + 2 * col];
        dutyAngles[k * frameSize + 2 * col] = dutyAngles[k * frameSize + 2 * col + 1];
        dutyAngles[k * frameSize + 2 * col + 1] = temp;
      }
    }
  }
#define PRINT_SKILL_DATA
  void info() {
    PTF("prd:");
    PT(period);
    PT(",\texpct(pitch,roll):");
    PT(expectedRollPitch[0]);
    PT(",");
    PT(expectedRollPitch[1]);
    PT("\t");
    PTF("ratio: ");
    PTL(angleDataRatio);
    if (period < 0) {
      PT("loop: ");
      for (byte i = 0; i < 3; i++)
        PT(String((byte)loopCycle[i]) + ", ");
      PTL();
    }
#ifdef PRINT_SKILL_DATA
    for (int k = 0; k < abs(period); k++) {
      for (int col = 0; col < frameSize; col++) {
        PT((int8_t)dutyAngles[k * frameSize + col]);
        PT("\t");
      }
      PTL();
    }
#endif
    PTL();
  }

  void convertTargetToPosture(int* targetFrame) {
    int extreme[2];
    getExtreme(targetFrame, extreme);
    if (extreme[0] < -125 || extreme[1] > 125) {
      angleDataRatio = 2;
      for (int i = 0; i < DOF; i++)
        targetFrame[i] /= 2;
    } else
      angleDataRatio = 1;
    arrayNCPY(dutyAngles, targetFrame, DOF);
    period = 1;
    firstMotionJoint = 0;
    frameSize = DOF;
    frame = 0;
  }
  void perform() {
    if (period < 0) {  //behaviors
      int8_t repeat = loopCycle[2] >= 0 && loopCycle[2] < 2 ? 0 : loopCycle[2] - 1;
      for (byte c = 0; c < abs(period); c++) {  //the last two in the row are transition speed and delay
                                                //          PT("step "); PTL(c);
                                                //          printList(dutyAngles + c * frameSize);

        transform(dutyAngles + c * frameSize, angleDataRatio, dutyAngles[DOF + c * frameSize] / 4.0);

#ifdef GYRO_PIN  //if opt out the gyro, the calculation can be really fast
        if (dutyAngles[DOF + 2 + c * frameSize]) {
          int triggerAxis = dutyAngles[DOF + 2 + c * frameSize];
          int triggerAngle = dutyAngles[DOF + 3 + c * frameSize];

          float currentYpr = ypr[abs(triggerAxis)];
          float previousYpr = currentYpr;
          //            long triggerTimer = millis();
          while (1) {
            read_IMU();
            currentYpr = ypr[abs(triggerAxis)];
            //              print6Axis();
            //              PT(currentYpr);
            //              PT('\t');
            //              PTL(triggerAngle);
            if ((180 - fabs(currentYpr) > 2)                                                                                           //skip the angle when the reading jumps from 180 to -180
                && (triggerAxis * currentYpr > triggerAxis * triggerAngle && triggerAxis * previousYpr < triggerAxis * triggerAngle))  //the sign of triggerAxis will deterine whether the current angle should be larger or smaller than the trigger angle
              break;
            previousYpr = currentYpr;
          }
        }
#endif
        delay(abs(dutyAngles[DOF + 1 + c * frameSize] * 50));
        if (repeat != 0 && c != 0 && c == loopCycle[1]) {
          c = loopCycle[0] - 1;
          if (repeat > 0)  //if repeat <0, infinite loop. only the reset button will break the loop
            repeat--;
        }
      }
    } else {          //postures and gaits
#if defined GYRO_PIN  //&& !defined RANDOM_MIND
      // if (imuUpdated)
      if (!(frame % imuSkip)) {
        for (byte i = 0; i < 2; i++) {
          RollPitchDeviation[i] = ypr[2 - i] - expectedRollPitch[i];                                                                          //all in degrees
          RollPitchDeviation[i] = sign(ypr[2 - i]) * max(float(fabs(RollPitchDeviation[i]) - levelTolerance[i]), float(0)) + yprTilt[2 - i];  //filter out small angles
        }
      }
#endif
      if (jointIndex >= DOF) {
        jointIndex = 0;
        // timer = (timer + 1) % abs(skill.period);
        frame += tStep;
        if (frame >= abs(period)) {
          frame = 0;
        }
      }
#ifndef HEAD
      if (jointIndex == 0)
        jointIndex = 2;
#endif
#ifndef TAIL
      if (jointIndex == 2)
        jointIndex = DOF - WALKING_DOF;
#endif
#if WALKING_DOF == 8
      if (jointIndex == 4)
        jointIndex = 8;
#endif
      float duty;
      if (abs(period) > 1 && jointIndex < firstMotionJoint || abs(period) == 1 && jointIndex < 4 && manualHeadQ) {
        if (!manualHeadQ && jointIndex < 4) {
          duty = (jointIndex != 1 ? offsetLR : 0)  //look left or right
                 + 10 * sin(frame * (jointIndex + 2) * M_PI / abs(period));
        } else {
          duty =
#ifdef DAMPED_MOTION
            currentAng[jointIndex] + max(-10, min(10, (targetHead[jointIndex] - currentAng[jointIndex])))
#else
            targetHead[jointIndex]
#endif
            - currentAdjust[jointIndex];
        }
      } else
        duty = dutyAngles[frame * frameSize + jointIndex - firstMotionJoint] * angleDataRatio;
      calibratedPWM(jointIndex, duty
#if defined GYRO_PIN  //&& !defined RANDOM_MIND
                                  + (!exceptions ? (!(frame % imuSkip) ? adjust(jointIndex) : currentAdjust[jointIndex]) : 0)
#endif
      );
      jointIndex++;
    }
  }
};

Skill skill;

#ifdef BITTLE
int8_t expect[] = {
  57,
  43,
  60,
  47,
  -18,
  7,
  -17,
  7,
};
#elif defined NYBBLE
int8_t expect[] = {
  51,
  39,
  -57,
  -43,
  -18,
  7,
  19,
  -7,
};
#endif

int testEEPROM(char* skillData) {
  skill.loadFrame(skillData);
  PTL(skill.period);
  int len = 0;
  while (len < 8) {
    if (skill.dutyAngles[len] != expect[len])
      return 0;
    PT(int8_t(skill.dutyAngles[len]));
    PT('\t');
    len++;
  }
  return 1;
}

#ifndef MAIN_SKETCH
void writeConst() {
  //  flushEEPROM();
  int melodyAddress = MELODY_NORMAL;
  saveMelody(melodyAddress, melodyNormalBoot, sizeof(melodyNormalBoot));
  saveMelody(melodyAddress, melodyInit, sizeof(melodyInit));
  saveMelody(melodyAddress, melodyLowBattery, sizeof(melodyLowBattery));
  // saveMelody(melodyAddress, melody1, sizeof(melody1));
  EEPROM.update(BOOTUP_SOUND_STATE, 1);  //always turn on the boot up sound after reset
#if !defined AUTO_INIT
  playMelody(MELODY_INIT);
#endif
#ifndef AUTO_INIT
  PTLF("Reset joint offsets?(Y/n)");  //(Input ‘Y’ and hit enter, if you want to reset all the joint offsets to 0)
  char resetJointCalibrationQ = getUserInputChar();
  PTL(resetJointCalibrationQ);
#endif
  for (byte i = 0; i < DOF; i++) {
#ifndef AUTO_INIT
    if (resetJointCalibrationQ == 'Y' || resetJointCalibrationQ == 'y')
#endif
      EEPROM.update(CALIB + i, servoCalib[i]);
    EEPROM.update(PWM_PIN + i, pwm_pin[i]);
    EEPROM.update(MID_SHIFT + i, middleShift[i]);
    EEPROM.update(ROTATION_DIRECTION + i, rotationDirection[i]
#ifdef INVERSE_SERVO_DIRECTION
                                            * -1
#endif
    );
    for (byte j = 0; j < 2; j++) {
      EEPROM.update(ADAPT_PARAM + i * 2 + j, (int8_t)round(adaptiveParameterArray[i][j]));
      EEPROMWriteInt(ANGLE_LIMIT + i * 4 + j * 2, angleLimit[i][j]);
    }
    delay(6);
  }
  //PTL();
}

int configureEEPROM() {
  PTLF("\n* Change model and board definitions in OpenCat.ino!");
  PTLF("\nConfigure EEPROM");
  writeConst();  // only run for the first time when writing to the board.
  wdt_enable(WDTO_8S);
  skill.saveSkillInfoFromProgmemToOnboardEeprom();
  wdt_reset();
  return 1;
}
#endif