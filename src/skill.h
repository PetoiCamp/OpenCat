#define WIRE_BUFFER 30 //Arduino wire allows a 32-byte buffer, with 2 bytes for address.
#define WIRE_LIMIT 16 //That leaves 30 bytes for data. Use 16 to balance each writes
#define PAGE_LIMIT 32 //AT24C32D 32-byte Page Write Mode. Partial Page Writes Allowed
#define EEPROM_SIZE (65536/8)
bool EEPROMOverflow = false;

class Skill {
  public:
    byte jointIndex;
    char* skillName; //use char array instead of String to save memory
    int8_t offsetLR;
    int8_t period;            //the period of a skill. 1 for posture, >1 for gait, <-1 for behavior
    float transformSpeed;
    byte frameSize;
    int expectedRollPitch[2]; //expected body orientation (roll, pitch)
    byte angleDataRatio;      //divide large angles by 1 or 2. if the max angle of skill is >128, all the angles will be divided by 2
    byte loopCycle[3];        //the looping section of a behavior (starting row, ending row, repeating cycles)
    byte firstMotionJoint;
    int8_t* dutyAngles;         //the data array for skill angles and parameters

    Skill() {
      //      period = 0;
      //      transformSpeed = 2;
      //      expectedRollPitch[0] = yprTilt[2] = 0;
      //      expectedRollPitch[1] = yprTilt[1] = 0;
      //      dutyAngles = NULL;
      //      jointIndex = 0;
    }

    void copyDataFromBufferToI2cEeprom(unsigned int eeAddress, int8_t *dataBuffer) {
      period = dataBuffer[0];//automatically cast to char*
      int len = dataLen(period) + 1;
      int writtenToEE = 0;
      while (len > 0) {
        Wire.beginTransmission(DEVICE_ADDRESS);
        Wire.write((int)((eeAddress) >> 8));   // MSB
        Wire.write((int)((eeAddress) & 0xFF)); // LSB
        byte writtenToWire = 0;
        do {
          if (eeAddress == EEPROM_SIZE) {
            PTL();
            PTLF("I2C EEPROM overflow! Delete some skills!\n");
            EEPROMOverflow = true;
#ifdef BUZZER
            beep(10, 100, 50, 2);
#endif
            return;
          }
          Wire.write((byte)dataBuffer[writtenToEE++]);
          writtenToWire++;
          eeAddress++;
        } while ((--len > 0 ) && (eeAddress  % PAGE_LIMIT ) && (writtenToWire < WIRE_LIMIT));//be careful with the chained conditions
        //self-increment may not work as expected
        Wire.endTransmission();
        delay(10);  // needs 5ms for page write
        //PTL("\nwrote " + String(writtenToWire) + " bytes.");
      }
      //PTLF("finish copying to I2C EEPROM");
    }
#ifndef MAIN_SKETCH
    void copyDataFromPgmToI2cEeprom(unsigned int &eeAddress, unsigned int pgmAddress) {
      period = pgm_read_byte(pgmAddress);//automatically cast to char*
      int len = dataLen(period) + 1;
      int writtenToEE = 0;
      while (len > 0) {
        Wire.beginTransmission(DEVICE_ADDRESS);
        Wire.write((int)((eeAddress) >> 8));   // MSB
        Wire.write((int)((eeAddress) & 0xFF)); // LSB
        byte writtenToWire = 0;
        do {
          if (eeAddress == EEPROM_SIZE) {
            PTL();
            PTLF("I2C EEPROM overflow! Delete some skills!\n");
            EEPROMOverflow = true;
#ifdef BUZZER
            beep(10, 100, 50, 2);
#endif
            return;
          }
          Wire.write((byte)pgm_read_byte(pgmAddress + writtenToEE++));
          writtenToWire++;
          eeAddress++;
        } while ((--len > 0 ) && (eeAddress  % PAGE_LIMIT ) && (writtenToWire < WIRE_LIMIT));//be careful with the chained conditions
        //self-increment may not work as expected
        Wire.endTransmission();
        delay(10);  // needs 5ms for page write
        //PTL("\nwrote " + String(writtenToWire) + " bytes.");
      }
      //PTLF("finish copying to I2C EEPROM");
    }

    void saveSkillInfoFromProgmemToOnboardEeprom() {
      int skillAddressShift = 0;
      unsigned int i2cEepromAddress = INITIAL_SKILL_DATA_ADDRESS; //won't hurt if unused

      PTLF("Save skills");
      EEPROM.update(NUM_SKILLS, sizeof(skillNameWithType) / 2);
      for (byte s = 0; s < eeprom(NUM_SKILLS); s++) {//save skill info to on-board EEPROM
        byte len = strlen(skillNameWithType[s]); //len includes type. the saved value doesn't
        EEPROM.update(SKILLS + skillAddressShift++, len - 1); //the last char in name denotes skill type, I(nstinct) on external eeprom, N(ewbility) on progmem
        PT(skillNameWithType[s][len - 1] == 'I' ? "I nstinct\t" : "N ewbility\t");
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
          if (skillNameWithType[s][len - 1] == 'I') { //  if there's instinct and there's i2c eeprom, and user decide to update.
            // save the data array to i2c eeprom. its address will be saved to onboard eeprom
            EEPROMWriteInt(SKILLS + skillAddressShift, i2cEepromAddress);
            copyDataFromPgmToI2cEeprom(i2cEepromAddress,  (unsigned int) progmemPointer[s]);
            //            PT(int(SKILLS + skillAddressShift + 2)); PT(" I2C EE "); PTL(int(i2cEepromAddress));
          }
#endif
        skillAddressShift += 2; // one int (2 bytes) for address
      }
      EEPROMWriteInt(SERIAL_BUFF, (unsigned int)i2cEepromAddress);//SERIAL_BUFF saves the I2C EEPROM tail address for saving a buffer received from the serial port
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
      PTLF("Load skills");
      for (byte s = 0; s < sizeof(progmemPointer) / 2; s++) { //save skill info to on-board EEPROM, load skills to SkillList
        //        PTL(s);
        byte nameLen = EEPROM.read(SKILLS + skillAddressShift++); //without last type character
        //    for (int n = 0; n < nameLen; n++)
        //      PT((char)EEPROM.read(SKILLS + skillAddressShift + n));
        skillAddressShift += nameLen;
        char skillType = EEPROM.read(SKILLS + skillAddressShift++);
        //    PT(skillType);
        //    PT('\t');
        //    PTL((int8_t)EEPROM.read(SKILLS + skillAddressShift));
#ifdef I2C_EEPROM
        if (skillType == 'N') // the address of I(nstinct) has been written in previous operation: saveSkillNameFromProgmemToOnboardEEPROM() in instinct.ino
          // if skillType == N(ewbility), save pointer address of progmem data array to onboard eeprom.
          // it has to be done for different sketches because the addresses are dynamically assigned
#endif
          EEPROMWriteInt(SKILLS + skillAddressShift + 1, (int)progmemPointer[s]);
#if defined(I2C_EEPROM) && defined (MAIN_SKETCH)
        else
          s--;
#endif
        skillAddressShift += 3;
      }
      //  PTLF("Load finished!");
    }

    int lookupAddressByName(const char* skillName) {
      int skillAddressShift = 0;
      for (byte s = 0; s < eeprom(NUM_SKILLS); s++) {//save skill info to on-board EEPROM, load skills to SkillList
        byte nameLen = EEPROM.read(SKILLS + skillAddressShift++);
        char* readName = new char[nameLen + 1];
        for (byte l = 0; l < nameLen; l++) {
          readName[l] = EEPROM.read(SKILLS + skillAddressShift++);
        }
        readName[nameLen] = '\0';
        if (!strcmp(readName, skillName) || readName[nameLen - 1] == 'L' && !strncmp(readName, skillName, nameLen - 1)) {
          delete[]readName;
          period = EEPROM.read(SKILLS + skillAddressShift + 1);
          int bufferLen = dataLen(period);
          return SKILLS + skillAddressShift;
        }
        delete[]readName;
        skillAddressShift += 4;//1 byte type, 1 byte period, 1 int address
      }
      PTLF("?");//key not found
      return -1;
    }

    int dataLen(int8_t p) {
      byte skillHeader = p > 0 ? 4 : 7;
      frameSize = p > 1 ?
                  WALKING_DOF :       //gait
                  p == 1 ? DOF : //posture
                  DOF + 4;            //behavior
      int len = skillHeader + abs(p) * frameSize;
      return len;
    }

    void loadDataFromProgmem(unsigned int pgmAddress) {
      //      dataBuffer = new int8_t [bufferLen + 1];
      //      dataBuffer[0] = pgm_read_byte(pgmAddress++);
      int bufferLen = dataLen(period);
      for (int i = 0; i < bufferLen; i++)
        dataBuffer[i] = pgm_read_byte(pgmAddress++);
      //      dataBuffer[bufferLen] = '\0';
    }

    void loadDataFromI2cEeprom(unsigned int eeAddress) {
      Wire.beginTransmission(DEVICE_ADDRESS);
      Wire.write((int)((eeAddress) >> 8));   // MSB
      Wire.write((int)((eeAddress) & 0xFF)); // LSB
      Wire.endTransmission();
      Wire.requestFrom((uint8_t)DEVICE_ADDRESS, (uint8_t)1);
      period = dataBuffer[0] = Wire.read();
      int bufferLen = dataLen(period);
      //      int tail = bufferLen;
      int readFromEE = 0;
      int readToWire = 0;
      while (bufferLen > 0) {
        //PTL("request " + String(min(WIRE_BUFFER, len)));
        Wire.requestFrom((uint8_t)DEVICE_ADDRESS, (uint8_t)min(WIRE_BUFFER, bufferLen));
        readToWire = 0;
        do {
          if (Wire.available()) dataBuffer[1 + readFromEE++] = Wire.read();
          /*PT( (int8_t)dutyAngles[readFromEE - 1]);
            PT('\t')*/
        } while (--bufferLen > 0 && ++readToWire < WIRE_BUFFER);
        //PTL();
      }
      //      dataBuffer[tail] = '\0';
    }

    void formatSkill() {
      transformSpeed = 2;
      for (int i = 0; i < 2; i++) {
        expectedRollPitch[i] = (int8_t)dataBuffer[1 + i];
        yprTilt[2 - i] = 0;
      }
      angleDataRatio = dataBuffer[3];
      byte skillHeader = 4;
      if (period < 0) {
        for (byte i = 0; i < 3; i++)
          loopCycle[i] = dataBuffer[skillHeader++];
      }

#ifdef POSTURE_WALKING_FACTOR
      postureOrWalkingFactor = (period == 1 ? 1 : POSTURE_WALKING_FACTOR);
#endif
      firstMotionJoint = (period <= 1) ? 0 : DOF - WALKING_DOF;
      dutyAngles = dataBuffer + skillHeader;
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
      //      period = dataBuffer[0];
      //      dataLen(period);
      formatSkill();
      frame = 0;
      transform(dutyAngles + frame * frameSize, angleDataRatio, transformSpeed, firstMotionJoint);
    }

    void loadFrame(const char* skillName) {//get lookup information from on-board EEPROM and read the data array from storage
      char lr = skillName[strlen(skillName) - 1];
      offsetLR = (lr == 'L' ? 25 : (lr == 'R' ? -25 : 0));
      int onBoardEepromAddress = lookupAddressByName(skillName);
      if (onBoardEepromAddress == -1)
        return;//won't delete the dataBuffer if the new key is wrong.
      //      if (dataBuffer != NULL)
      //        delete [] dataBuffer;
      char skillType = EEPROM.read(onBoardEepromAddress); //load data by onboard EEPROM address
      unsigned int dataArrayAddress = EEPROMReadInt(onBoardEepromAddress + 2);
#ifdef I2C_EEPROM
      if (skillType == 'I')  //copy instinct data array from external i2c eeprom
        loadDataFromI2cEeprom(dataArrayAddress);
      else                    //copy newbility data array from progmem
#endif
        loadDataFromProgmem(dataArrayAddress);
      formatSkill();
      if (strcmp(skillName, "calib") && period == 1)
        protectiveShift = random() % 100 / 10.0 - 5;
      else
        protectiveShift = 0;
      for (byte i = 0; i < DOF; i++)
        dutyAngles[i] += protectiveShift;
      if (offsetLR < 0 || period <= 1 && offsetLR == 0 && random(100) % 2 && token != T_CALIBRATE)
        mirror();
      frame = 0;
      transform( dutyAngles + frame * frameSize, angleDataRatio, transformSpeed, firstMotionJoint);
      //      delay(10);
    }

    void mirror() {
      expectedRollPitch[0] = -expectedRollPitch[0];
      for (int k = 0; k < abs(period); k++) {
        if (period <= 1) {
          dutyAngles[k * frameSize ] = -dutyAngles[k * frameSize ];
          dutyAngles[k * frameSize + 2] = -dutyAngles[k * frameSize + 2];
        }
        for (byte col = (period > 1) ? 0 : 2; col < ((period > 1) ? WALKING_DOF : DOF) / 2 ; col++) {
          int8_t temp = dutyAngles[k * frameSize + 2 * col];
          dutyAngles[k * frameSize + 2 * col] = dutyAngles[k * frameSize + 2 * col + 1];
          dutyAngles[k * frameSize + 2 * col + 1] = temp;
        }
      }
    }

    void info() {
      PTL("period:" + String(period) + ",\texpected(pitch,roll):(" + expectedRollPitch[0] + "," + expectedRollPitch[1] + ")");
      for (int k = 0; k < abs(period); k++) {
        for (int col = 0; col < frameSize ; col++) {
          PT(String((int8_t)dutyAngles[k * frameSize + col]) + ",\t");
        }
        PTL();
      }
      PTL();
    }

    void perform() {
      if (period < 0) {   //behaviors
        int8_t repeat = loopCycle[2] >= 0 && loopCycle[2] < 2 ? 0 : loopCycle[2] - 1;
        for (byte c = 0; c < abs(period); c++) { //the last two in the row are transition speed and delay
          //          PT("step "); PTL(c);
          //          printList(dutyAngles + c * frameSize);

          transform(dutyAngles + c * frameSize, angleDataRatio, dutyAngles[DOF + c * frameSize] / 4.0);

#ifdef GYRO_PIN //if opt out the gyro, the calculation can be really fast
          if (dutyAngles[DOF + 2 + c * frameSize]) {
            int triggerAxis = dutyAngles[DOF + 2 + c * frameSize];
            int triggerAngle = dutyAngles[DOF + 3 + c * frameSize];

            float currentYpr = ypr[abs(triggerAxis)]  ;
            float previousYpr = currentYpr;
            //            long triggerTimer = millis();
            while (1) {
              read_IMU();
              currentYpr = ypr[abs(triggerAxis)];
              //              print6Axis();
              //              PT(currentYpr);
              //              PT('\t');
              //              PTL(triggerAngle);
              if ((180 - fabs(currentYpr) > 2)  //IMU_SKIP the angle when the reading jumps from 180 to -180
                  && (triggerAxis * currentYpr > triggerAxis * triggerAngle && triggerAxis * previousYpr < triggerAxis * triggerAngle )
                 ) //the sign of triggerAxis will deterine whether the current angle should be larger or smaller than the trigger angle
                break;
              previousYpr = currentYpr;
            }
          }
#endif
          delay(abs(dutyAngles[DOF + 1 + c * frameSize] * 50));
          if (repeat != 0 && c != 0 && c == loopCycle[1]) {
            c = loopCycle[0] - 1;
            if (repeat > 0) //if repeat <0, infinite loop. only the reset button will break the loop
              repeat--;
          }
        }
      }
      else {//postures and gaits
#ifdef GYRO_PIN
        // if (imuUpdated)
        if (!(frame % IMU_SKIP)) {
          for (byte i = 0; i < 2; i++) {
            RollPitchDeviation[i] = ypr[2 - i]  - expectedRollPitch[i]; //all in degrees
            RollPitchDeviation[i] = sign(ypr[2 - i]) * max(float(fabs(RollPitchDeviation[i]) - levelTolerance[i]), float(0)) + yprTilt[2 - i]; //filter out small angles
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
        //          PT(jointIndex); PT('\t');
        float duty;
        if (jointIndex < firstMotionJoint && abs(period) > 1)
          duty = (jointIndex != 1 ? offsetLR : 0) //look left or right
                 + 10 * sin (frame * (jointIndex + 2) * M_PI / abs(period));
        else
          duty = dutyAngles[frame * frameSize + jointIndex - firstMotionJoint] * angleDataRatio;

        //          PT(duty); PT('\t');
        calibratedPWM(jointIndex, duty
#ifdef GYRO_PIN
                      + (checkGyro && !exceptions ?
                         (!(frame % IMU_SKIP) ? adjust(jointIndex) : currentAdjust[jointIndex]) : 0)
#endif
                     );
        jointIndex++;
      }
      //        PTL();
    }

};

Skill skill;

#ifdef BITTLE
int8_t expect[] = {57,  43,  60,  47, -18,   7, -17,   7,};
#elif defined NYBBLE
int8_t expect[] = {51,  39, -57, -43, -18,   7,  19,  -7,};
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
  beep(20);
  int melodyAddress = MELODY_NORMAL;
  saveMelody(melodyAddress, melodyNormalBoot, sizeof(melodyNormalBoot));
  saveMelody(melodyAddress, melodyInit, sizeof(melodyInit));
  saveMelody(melodyAddress, melodyLowBattery, sizeof(melodyLowBattery));
  saveMelody(melodyAddress, melody1, sizeof(melody1));
#ifndef AUTO_INIT
  playMelody(MELODY_INIT);
#endif
#ifndef AUTO_INIT
  PTLF("Reset joint offsets?(Y/n)");//(Input ‘Y’ and hit enter, if you want to reset all the joint offsets to 0)
  char resetJointCalibrationQ = getUserInputChar();
  PTL(resetJointCalibrationQ);
#endif
  for (byte i = 0; i < DOF; i++) {
#ifndef AUTO_INIT
    if (resetJointCalibrationQ == 'Y' || resetJointCalibrationQ == 'y') {
      EEPROM.update(CALIB + i, servoCalib[i]);
      delay(10);
    }
#endif
    EEPROM.update(PWM_PIN + i, pwm_pin[i]);
    EEPROM.update(MID_SHIFT + i, middleShift[i]);
    EEPROM.update(ROTATION_DIRECTION + i, rotationDirection[i]);
    for (byte j = 0; j < 2; j++) {
      EEPROM.update(ADAPT_PARAM + i * 2 + j, (int8_t)round(adaptiveParameterArray[i][j]));
      EEPROMWriteInt(ANGLE_LIMIT + i * 4 + j * 2, angleLimit[i][j]);
    }
  }
  //PTL();
}

int configureEEPROM() {
  PTLF("\n* Change model and board definitions in OpenCat.ino!");
  PTLF("\nConfigure EEPROM");
  writeConst(); // only run for the first time when writing to the board.
  wdt_enable(WDTO_8S);
  skill.saveSkillInfoFromProgmemToOnboardEeprom();
  wdt_reset();
  return 1;
}
#endif
