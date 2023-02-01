void dealWithExceptions() {
  if (checkGyro && token != T_CALIBRATE && exceptions) {  //the gyro reaction switch can be toggled on/off by the 'g' token
    //  soundFallOver();
    //  for (int m = 0; m < 2; m++)
    //    meow(30 - m * 12, 42 - m * 12, 20);
    //  beep(15, 100, 100, 2);
    meow();
    token = T_SKILL;
    strcpy(newCmd, "rc");
    newCmdIdx = -1;
  }
}

#ifdef VOLTAGE_DETECTION_PIN
bool lowBattery() {
  long currentTime = millis() / CHECK_BATTERY_PERIOD;
  if (currentTime > uptime) {
    uptime = currentTime;
    int voltage = analogRead(VOLTAGE_DETECTION_PIN);
    if (voltage < LOW_VOLTAGE && (voltage == lastVoltage || uptime < 2)) {  //if battery voltage < threshold, it needs to be recharged
      //give the robot a break when voltage drops after sprint
      //adjust the thresholds according to your batteries' voltage
      //if set too high, the robot will stop working when the battery still has power.
      //If too low, the robot may not alarm before the battery shuts off
      PTF("Low power:");
      PT(voltage / 99);
      PTL('V');
      playMelody(MELODY_LOW_BATTERY);
      //    int8_t bStep = 1;
      //    for (byte brightness = 1; brightness > 0; brightness += bStep) {
      //#ifdef NEOPIXEL_PIN
      //      pixel.setPixelColor(0, pixel.Color(brightness, 0, 0));
      //      pixel.show();
      //#endif
      //#if defined LED_PIN
      //      analogWrite(LED_PIN, 255 - brightness);
      //#endif
      //      if (brightness == 255)
      //        bStep = -1;
      //      delay(5);
      //    }
      delay(2000);
      lastVoltage = voltage;
      return true;
    }
    lastVoltage = voltage;
  }
  return false;
}
#endif

void resetCmd() {
  //  PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  if (strcmp(newCmd, "rc") && token != T_INDEXED_SIMULTANEOUS_BIN && token != T_INDEXED_SEQUENTIAL_BIN
#ifdef BINARY_COMMAND
      && token != T_LISTED_BIN && token != T_SKILL_DATA
#endif
  ) {
    delete[] lastCmd;
    lastCmd = new char(strlen(newCmd) + 1);
    strcpy(lastCmd, newCmd);
  }
  newCmdIdx = 0;
  lastToken = token;
  if (token != T_SKILL && token != T_CALIBRATE)
    token = '\0';
  //  PTL("******");
}

void reaction() {
  if (newCmdIdx) {
    //    PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
#ifdef MAIN_SKETCH
    if (newCmdIdx < 5 && token != T_BEEP && token != T_MEOW && token != T_TILT
#ifdef BINARY_COMMAND
        && token != T_LISTED_BIN && token != T_INDEXED_SIMULTANEOUS_BIN && token != T_COLOR
#endif
    )
      beep(15 + newCmdIdx, 5);  //ToDo: check the muted sound when newCmdIdx = -1
    if ((lastToken == T_CALIBRATE || lastToken == T_REST) && token != T_CALIBRATE) {
      setServoP(P_SOFT);
      checkGyro = true;
      PTL('G');
    }
    if (token != T_PAUSE && !tStep) {
      tStep = 1;
      PTL('p');
    }
#endif
    switch (token) {
#ifdef MAIN_SKETCH
#ifdef GYRO_PIN
      case T_GYRO:
#ifdef T_PRINT_GYRO
      case T_PRINT_GYRO:
#endif
#ifdef T_VERBOSELY_PRINT_GYRO
      case T_VERBOSELY_PRINT_GYRO:
#endif
#endif
#ifdef RANDOM_MIND
      case T_RANDOM_MIND:
#endif
#ifdef T_RAMP
      case T_RAMP:
#endif
        {
#ifdef GYRO_PIN
          if (token == T_GYRO) {
            checkGyro = !checkGyro;
            token = checkGyro ? 'G' : 'g';  //G for activated gyro
          }
#ifdef T_PRINT_GYRO
          else if (token == T_PRINT_GYRO) {
            print6Axis();
          }
#endif
#ifdef T_VERBOSELY_PRINT_GYRO
          else if (token == T_VERBOSELY_PRINT_GYRO) {
            printGyro = !printGyro;
            token = printGyro ? 'V' : 'v';  //V for verbosely print gyro data
          }
#endif
          else
#endif
#ifdef RANDOM_MIND
            if (token == T_RANDOM_MIND) {
            autoSwitch = !autoSwitch;
            token = autoSwitch ? 'Z' : 'z';  //Z for active random mind
          } else
#endif
#ifdef T_RAMP
            if (token == T_RAMP) {  //reverse the adjustment direction
            ramp = -ramp;
            token = ramp > 0 ? 'R' : 'r';
          }
#endif
          break;
        }
      case T_PAUSE:
        {
          tStep = !tStep;             //tStep can be -1
          token = tStep ? 'p' : 'P';  //P for pause activated
          if (!tStep)
            pwm.shutServos();
          break;
        }
      case T_JOINTS:
        {  //show the list of current joint anles
          printTable(currentAng);
          break;
        }
      case T_MELODY:
        {
          playMelody(MELODY_1);
          break;
        }
#ifdef ULTRASONIC
      case T_COLOR:
        {
          long color = ((long)(dataBuffer[0]) << 16) + ((long)(dataBuffer[1]) << 8) + (long)(dataBuffer[2]);
          if (dataBuffer[4] == -1)  //no special effect
            mRUS04.SetRgbColor(E_RGB_INDEX(dataBuffer[3]), color);
          else
            mRUS04.SetRgbEffect(E_RGB_INDEX(dataBuffer[3]), color, dataBuffer[4]);
          break;
        }
#endif
      case T_SAVE:
        {
          PTLF("saved");
          saveCalib(servoCalib);
          break;
        }
      case T_ABORT:
        {
          PTLF("aborted");
          for (byte i = 0; i < DOF; i++) {
            servoCalib[i] = eeprom(CALIB, i);
          }
          break;
        }
      case T_CALIBRATE:                 //calibration
      case T_INDEXED_SEQUENTIAL_ASC:    //move multiple indexed joints to angles once at a time (ASCII format entered in the serial monitor)
      case T_INDEXED_SIMULTANEOUS_ASC:  //move multiple indexed joints to angles simultaneously (ASCII format entered in the serial monitor)
#ifdef T_SERVO_MICROSECOND
      case T_SERVO_MICROSECOND:  //send pulse with unit of microsecond to a servo
#endif
      case T_TILT:  //tilt the robot, format: t axis angle. 0:yaw, 1:pitch, 2:roll
      case T_MEOW:  //meow
      case T_BEEP:  //beep(tone, duration): tone 0 is pause. the duration corresponds to 1/duration second
        {
          int targetFrame[DOF];
          //          for (int i = 0; i < DOF; i += 1) {
          //            targetFrame[i] = currentAng[i];
          //          }
          arrayNCPY(targetFrame, currentAng, DOF);
          char *pch;
          char *input = (token == T_TILT) ? newCmd : (char *)dataBuffer;
          pch = strtok((char *)input, " ,");
          do {  //it supports combining multiple commands at one time
            //for example: "m8 40 m8 -35 m 0 50" can be written as "m8 40 8 -35 0 50"
            //the combined commands should be less than four. string len <=30 to be exact.
            int target[2] = {};
            byte inLen = 0;
            for (byte b = 0; b < 2 && pch != NULL; b++) {
              target[b] = atoi(pch);
              pch = strtok(NULL, " ,\t");
              inLen++;
            }
            targetFrame[target[0]] = target[1];
            int angleStep = 0;
            if (token == T_CALIBRATE) {
              checkGyro = false;
              if (lastToken != T_CALIBRATE) {
                setServoP(P_HARD);
                strcpy(newCmd, "calib");
                skill.loadFrame(newCmd);
              }
              if (inLen == 2) {
                //                if (target[1] >= 1001) { // Using 1001 for incremental calibration. 1001 is adding 1 degree, 1002 is adding 2 and 1009 is adding 9 degrees
                //                  target[1] += (servoCalib[target[0]] - 1000);
                //                } else if (target[1] <= -1001) { // Using -1001 for incremental calibration. -1001 is removing 1 degree, 1002 is removing 2 and 1009 is removing 9 degrees
                //                  target[1] += (servoCalib[target[0]] + 1000);
                //                }
                servoCalib[target[0]] = target[1];
              }
              int duty = EEPROMReadInt(ZERO_POSITIONS + target[0] * 2) + float(servoCalib[target[0]]) * eeprom(ROTATION_DIRECTION, target[0]);
              pwm.writeAngle(target[0], duty);
              printTable(servoCalib);
              //              PT(token);
              //              printList(target, 2);
            } else if (token == T_INDEXED_SEQUENTIAL_ASC) {
              transform(targetFrame, 1, 2);
            }
#ifdef T_SERVO_MICROSECOND
            else if (token == T_SERVO_MICROSECOND) {
              pwm.writeMicroseconds(eeprom(PWM_PIN, target[0]), target[1]);
            }
#endif
            else if (token == T_TILT) {
              yprTilt[target[0]] = target[1];
            } else if (token == T_MEOW) {

              meow(random() % 3 + 1, (random() % 4 + 2) * 5);

            } else if (token == T_BEEP) {
              if (target[1])
                beep(target[0], 1000 / target[1], 50);
            }
          } while (pch != NULL);
          if (token == T_INDEXED_SIMULTANEOUS_ASC) {
            transform(targetFrame, 1, 2);
          }
          delete[] pch;
          break;
        }

      // this block handles array like arguments
      case T_INDEXED_SEQUENTIAL_BIN:
      case T_INDEXED_SIMULTANEOUS_BIN:
        {  //indexed joint motions: joint0, angle0, joint1, angle1, ... (binary encoding)
          int targetFrame[DOF];
          //          for (int i = 0; i < DOF; i++) {
          //            targetFrame[i] = currentAng[i];
          //          }
          arrayNCPY(targetFrame, currentAng, DOF);
          for (int i = 0; i < cmdLen; i += 2) {
            targetFrame[dataBuffer[i]] = dataBuffer[i + 1];
            if (token == T_INDEXED_SEQUENTIAL_BIN) {
              transform(targetFrame, 1, 2);
              //              delay(10);
            }
          }
          if (token == T_INDEXED_SIMULTANEOUS_BIN) {
            PTL(token);  //make real-time motion instructions more timely
            transform(targetFrame, 1, 4);
          }
          break;
        }
#ifdef BINARY_COMMAND
      case T_LISTED_BIN:
        {
          PTL(token);                                //make real-time motion instructions more timely
                                                     //list of all 16 joint: angle0, angle2,... angle15 (binary encoding)
          transform(dataBuffer, 1, transformSpeed);  //need to add angleDataRatio if the angles are large
          break;
        }
      case T_BEEP_BIN:
        {
          for (byte b = 0; b < cmdLen / 2; b++)
            beep(dataBuffer[2 * b], 1000 / dataBuffer[2 * b + 1]);
          break;
        }
#ifdef T_TEMP
      case T_TEMP:
        {  //call the last skill data received from the serial port
          skill.loadDataFromI2cEeprom((unsigned int)EEPROMReadInt(SERIAL_BUFF_RAND));
          skill.loadFrameByDataBuffer();
          PTL(token);
          token = T_SKILL;
          break;
        }
#endif
#ifdef T_SKILL_DATA
      case T_SKILL_DATA:
        {  //takes in the skill array from the serial port, load it as a regular skill object and run it locally without continuous communication with the master
          unsigned int i2cEepromAddress = EEPROMReadInt(SERIAL_BUFF) + random() % (EEPROM_SIZE - EEPROMReadInt(SERIAL_BUFF) - 500);
          EEPROMWriteInt(SERIAL_BUFF_RAND, i2cEepromAddress);                 //randomize the address of K data to protect the EEPROM
          skill.copyDataFromBufferToI2cEeprom(i2cEepromAddress, dataBuffer);  //must be before the loading to set the period
          skill.loadFrameByDataBuffer();
          token = T_SKILL;
          break;
        }
#endif
#endif
      case T_SKILL:
        {
          if (strcmp(lastCmd, newCmd)  //won't transform for the same gait. it's better to compare skill->skillName and newCmd. but need more logics for non skill cmd in between
              || skill.period <= 1) {  // for repeating behaviors. if it's set < 1, won't repeat the last behavior
            skill.loadFrame(newCmd);
          }
          break;
        }
#ifdef TASK_QUEUE
      case T_TASK_QUEUE:
        {
          tQueue->createTask();
          break;
        }
#endif
#endif

#ifdef T_SERVO_MICROSECOND
      case ';':
        {
          setServoP(P_SOFT);
          break;
        }
      case ':':
        {
          setServoP(P_HARD);
          break;
        }
#endif
      case T_REST:
        {
          strcpy(newCmd, "rest");
          skill.loadFrame(newCmd);
          pwm.shutServos();
          checkGyro = false;
          PTL(T_GYRO);
          break;
        }
    }
    if (token != T_SKILL || skill.period > 0) {
      if (token != T_INDEXED_SIMULTANEOUS_BIN && token != T_LISTED_BIN)
        PTL(token);  //postures, gaits, and other tokens can confirm completion by sending the token back
      char lowerToken = tolower(token);
      if (lastToken == T_SKILL && (lowerToken == T_GYRO || token == T_JOINTS || token == T_PAUSE || token == T_TILT
#ifdef T_PRINT_GYRO
                                   || lowerToken == T_PRINT_GYRO
#endif
#ifdef T_VERBOSELY_PRINT_GYRO
                                   || token == T_VERBOSELY_PRINT_GYRO
#endif
                                   ))
        token = T_SKILL;
    }

    //    PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
    resetCmd();
#ifdef DEVELOPER
    PTF("Mem:");
    PTL(freeMemory());
#endif
  }
#ifdef MAIN_SKETCH
  if (token == T_SKILL) {
    skill.perform();

#if defined NyBoard_V0_1 || defined NyBoard_V0_2 || !defined GYRO_PIN
    delayMicroseconds(850);  //Slow the robot down to smooth out motion; adjust delay PRN
#endif
  }
  if (skill.period < 0) {
    if (exceptions && lastCmd[strlen(lastCmd) - 1] < 'L' && skill.lookupAddressByName(lastCmd) > 0) {  //lastToken == T_SKILL && lastSkill->period > 0) {
      // if the last command is not a behavior and not a turning gait. case wkF, wkL, wkR, rlL, rlR

      strcpy(newCmd, lastCmd);
    } else {
      //      strcpy(newCmd, "balance");
      strcpy(newCmd, "");
      arrayNCPY(skill.dutyAngles, skill.dutyAngles + (abs(skill.period) - 1) * skill.frameSize, DOF);
      skill.period = 1;
      frame = 0;
    }
    for (int i = 0; i < DOF; i++)
      currentAdjust[i] = 0;
    if (strcmp(newCmd, ""))
      skill.loadFrame(newCmd);
    PTL(token);  //behavior can confirm completion by sending the token back
    //      PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  }
#endif
}