void dealWithExceptions() {
  if (gyroBalanceQ && exceptions) {  //the gyro reaction switch can be toggled on/off by the 'g' token
//  soundFallOver();
//  for (int m = 0; m < 2; m++)
//    meow(30 - m * 12, 42 - m * 12, 20);
//  beep(15, 100, 100, 2);
#ifdef T_MEOW
    meow();
#else
    beep(20, 50, 25, 5);
#endif
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

      if (eeprom(BOOTUP_SOUND_STATE))
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

void reaction() {
  if (newCmdIdx) {
    lowerToken = tolower(token);
#ifdef MAIN_SKETCH
    if (newCmdIdx < 5 && lowerToken != T_BEEP
#ifdef T_MEOW
        && token != T_MEOW
#endif
#ifdef BINARY_COMMAND
        && token != T_LISTED_BIN && token != T_INDEXED_SIMULTANEOUS_BIN
#endif
#ifdef GROVE_SERIAL_PASS_THROUGH
        && token != T_READ && token != T_WRITE
#endif
    )
      beep(15 + newCmdIdx, 5);                    //ToDo: check the muted sound when newCmdIdx = -1
    if (hardServoQ && (lowerToken == T_SKILL)) {  //|| lowerToken == T_INDEXED_SEQUENTIAL_ASC || lowerToken == T_INDEXED_SIMULTANEOUS_ASC)) {
      setServoP(P_SOFT);
      hardServoQ = false;
    }
    if ((lastToken == T_CALIBRATE || lastToken == T_REST) && token != T_CALIBRATE) {
      gyroBalanceQ = true;
      PTL('G');
    }
    if (token != T_PAUSE && !tStep) {
      tStep = 1;
      PTL('p');
    }
#endif
    switch (token) {
#ifdef MAIN_SKETCH
      case T_QUERY:
        {
          PTLF(MODEL);
          PTLF(SOFTWARE_VERSION);
          break;
        }

      case T_GYRO_FINENESS:
      case T_GYRO_BALANCE:
#ifdef GYRO_PIN
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
#ifdef T_SLOPE
      case T_SLOPE:
#endif
        {
          if (token == T_GYRO_FINENESS) {
#ifdef GYRO_PIN
            fineAdjust = !fineAdjust;
            imuSkip = fineAdjust ? IMU_SKIP : IMU_SKIP_MORE;
            token = fineAdjust ? 'G' : 'g';  //G for fine adjust
#else
            token = 'g';
#endif
          } else if (token == T_GYRO_BALANCE) {

#ifdef GYRO_PIN
            gyroBalanceQ = !gyroBalanceQ;
            token = gyroBalanceQ ? 'G' : 'g';  //G for activated balance
#else
            token = 'g';
#endif
          }
#ifdef GYRO_PIN
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
#ifdef T_SLOPE
            if (token == T_SLOPE) {  //reverse the adjustment direction
            slope = -slope;
            token = slope > 0 ? 'R' : 'r';
          }
#endif
          {}  //must have in cases the else combines with break
          break;
        }
      case T_PAUSE:
        {
          tStep = !tStep;             //tStep can be -1
          token = tStep ? 'p' : 'P';  //P for pause activated
          if (tStep)
            token = T_SKILL;
          else
            pwm.shutServos();
          break;
        }
      case T_JOINTS:
        {  //show the list of current joint anles
          PT('=');
          if (cmdLen)
            PTL(currentAng[atoi(newCmd)]);
          else
            printTable(currentAng);
          break;
        }
#ifdef T_MELODY
      case T_MELODY:
        {
          playMelody(MELODY_1);
          break;
        }
#endif
#ifdef ULTRASONIC
      case T_COLOR:
        {
          if (cmdLen < 2)  //a single 'C' will turn off the manual color mode
                           //the terminator of upper-case tokens is not '\n'. it may cause error when entered in the Arduino serial monitor
            manualEyeColorQ = false;
          else {  // turn on the manual color mode
            manualEyeColorQ = true;
            long color = ((long)(uint8_t(newCmd[0])) << 16) + ((long)(uint8_t(newCmd[1])) << 8) + (long)(uint8_t(newCmd[2]));
            mRUS04.SetRgbEffect(E_RGB_INDEX(uint8_t(newCmd[3])), color, uint8_t(newCmd[4]));
          }
          break;
        }
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
#ifdef T_TILT
      case T_TILT:  //tilt the robot, format: t axis angle. 0:yaw, 1:pitch, 2:roll
#endif
#ifdef T_MEOW
      case T_MEOW:  //meow
#endif
      case T_BEEP:  //beep(tone, duration): tone 0 is pause. the duration corresponds to 1/duration second
#ifdef T_TUNER
      case T_TUNER:
#endif
        {
          if (token == T_INDEXED_SIMULTANEOUS_ASC && cmdLen == 0)
            manualHeadQ = false;
          else {
            int targetFrame[DOF];
            nonHeadJointQ = false;
            for (int i = 0; i < DOF; i++) {
              targetFrame[i] = currentAng[i] - (gyroBalanceQ ? currentAdjust[i] : 0);
            }
            // arrayNCPY(targetFrame, currentAng, DOF);
            char *pch;
            pch = strtok(newCmd, " ,");
            do {  //it supports combining multiple commands at one time
              //for example: "m8 40 m8 -35 m 0 50" can be written as "m8 40 8 -35 0 50"
              //the combined commands should be less than four. string len <=30 to be exact.
              int target[2] = {};
              int inLen = 0;
              for (byte b = 0; b < 2 && pch != NULL; b++) {
                target[b] = atoi(pch);
                pch = strtok(NULL, " ,\t");
                inLen++;
              }
              if ((token == T_INDEXED_SEQUENTIAL_ASC || token == T_INDEXED_SIMULTANEOUS_ASC) && target[0] >= 0 && target[0] < DOF) {
                targetFrame[target[0]] = target[1];
                if (target[0] < 4) {
                  targetHead[target[0]] = target[1];
                  manualHeadQ = true;
                } else
                  nonHeadJointQ = true;
              }
              if (token == T_CALIBRATE) {
                gyroBalanceQ = false;
                if (lastToken != T_CALIBRATE) {
                  setServoP(P_HARD);
                  hardServoQ = true;
                  strcpy(newCmd, "calib");
                  skill.loadFrame(newCmd);  //ensure it's correctly compared in load skill for protective shifts.
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
                // PT(token);
                // printList(target, 2);
              } else if (token == T_INDEXED_SEQUENTIAL_ASC) {
                transform(targetFrame, 1, 2);
              }
#ifdef T_SERVO_MICROSECOND
              else if (token == T_SERVO_MICROSECOND) {
                pwm.writeMicroseconds(eeprom(PWM_PIN, target[0]), target[1]);
              }
#endif
#ifdef T_TILT
              else if (token == T_TILT) {
                yprTilt[target[0]] = target[1];
              }
#endif
#ifdef T_MEOW
              else if (token == T_MEOW) {
                meow(random() % 3 + 1, (random() % 4 + 2) * 5);
              }
#endif
              else if (token == T_BEEP) {
                if (target[1])
                  beep(target[0], 1000 / target[1]);
              }

#ifdef T_TUNER
              else if (token == T_TUNER) {
                *par[target[0]] = target[1];
                PT(target[0]);
                PT('\t');
                PTL(target[1]);
              }
#endif
            } while (pch != NULL);
#ifdef T_TUNER
            if (token == T_TUNER) {
              for (byte p = 0; p < 6; p++) {
                PT(*par[p]);
                PT('\t');
              }
              PTL();
            }
#endif
            if ((token == T_INDEXED_SEQUENTIAL_ASC || token == T_INDEXED_SIMULTANEOUS_ASC) && (nonHeadJointQ || lastToken != T_SKILL)) {
              // PTL(token);
              transform(targetFrame, 1, 4);  // if (token == T_INDEXED_SEQUENTIAL_ASC) it will be useless
              skill.convertTargetToPosture(targetFrame);
            }
            delete[] pch;
          }
          break;
        }
      // this block handles array like arguments
      case T_INDEXED_SEQUENTIAL_BIN:
      case T_INDEXED_SIMULTANEOUS_BIN:
#ifdef GROVE_SERIAL_PASS_THROUGH
      case T_READ:
      case T_WRITE:
#endif
        {                  //indexed joint motions: joint0, angle0, joint1, angle1, ... (binary encoding)
          if (cmdLen < 2)  //the terminator of upper-case tokens is not '\n'. it may cause error when entered in the Arduino serial monitor
            manualHeadQ = false;
          else {
            int targetFrame[DOF];
            nonHeadJointQ = false;
            for (int i = 0; i < DOF; i++) {
              targetFrame[i] = currentAng[i] - (gyroBalanceQ ? currentAdjust[i] : 0);
            }
            // arrayNCPY(targetFrame, currentAng, DOF);
            byte group =
#ifdef GROVE_SERIAL_PASS_THROUGH
              token == T_WRITE ? 3 :
#endif
                               2;
            for (int i = 0; i < cmdLen; i += group) {
              if (newCmd[i] >= 0 && newCmd[i] < DOF) {
                targetFrame[newCmd[i]] = (int8_t)newCmd[i + 1];
                if (newCmd[i] < 4) {
                  targetHead[newCmd[i]] = (int8_t)newCmd[i + 1];
                  manualHeadQ = true;
                } else
                  nonHeadJointQ = true;
              }
              if (token == T_INDEXED_SEQUENTIAL_BIN) {
                transform(targetFrame, 1, 2);
              }
#ifdef GROVE_SERIAL_PASS_THROUGH
              else if (token == T_WRITE) {
                pinMode(newCmd[i + 1], OUTPUT);
                if (newCmd[i] == TYPE_ANALOG) {
                  analogWrite(newCmd[i + 1], uint8_t(newCmd[i + 2]));  //analog value can go up to 255.
                                                                       //the value was packed as unsigned byte by ardSerial
                                                                       //but casted by readSerial() as signed char and saved into newCmd.
                } else if (newCmd[i] == TYPE_DIGITAL)
                  digitalWrite(newCmd[i + 1], newCmd[i + 2]);
              } else if (token == T_READ) {
                PT('=');
                pinMode(newCmd[i + 1], INPUT);
                if (newCmd[i] == TYPE_ANALOG)  // Arduino Uno: A2->16, A3->17
                  PTL(analogRead(newCmd[i + 1]));
                else if (newCmd[i] == TYPE_DIGITAL)
                  PTL(digitalRead(newCmd[i + 1]));
              }
#endif
            }
            if (nonHeadJointQ || lastToken != T_SKILL) {
              // PTL(token);
              transform(targetFrame, 1, 4);  // if (token == T_INDEXED_SEQUENTIAL_BIN) it will be useless
              skill.convertTargetToPosture(targetFrame);
            }
          }
          break;
        }
#ifdef BINARY_COMMAND
      case T_LISTED_BIN:
        {
          // PTL(token);                                      //make real-time motion instructions more timely
          //list of all 16 joint: angle0, angle2,... angle15 (binary encoding)
          transform((int8_t *)newCmd, 1, transformSpeed);  //need to add angleDataRatio if the angles are large
          break;
        }
      case T_BEEP_BIN:
        {
          if (cmdLen < 2)  //toggle the melody on/off
                           //the terminator of upper-case tokens is not '\n'. it may cause error when entered in the Arduino serial monitor
            EEPROM.update(BOOTUP_SOUND_STATE, !eeprom(BOOTUP_SOUND_STATE));
          else {
            for (byte b = 0; b < cmdLen / 2; b++)
              beep(newCmd[2 * b], 1000 / newCmd[2 * b + 1]);
          }
          break;
        }
#ifdef T_TEMP
      case T_TEMP:
        {  //call the last skill data received from the serial port
          skill.loadDataFromI2cEeprom((unsigned int)EEPROMReadInt(SERIAL_BUFF));
          skill.loadFrameByDataBuffer();
          PTL(token);
          token = T_SKILL;
          strcpy(newCmd, "tmp");
          break;
        }
#endif
#ifdef T_SKILL_DATA
      case T_SKILL_DATA:
        {  //takes in the skill array from the serial port, load it as a regular skill object and run it locally without continuous communication with the master
          // unsigned int i2cEepromAddress = EEPROMReadInt(SERIAL_BUFF) + random() % (EEPROM_SIZE - EEPROMReadInt(SERIAL_BUFF)) - 500;
          // EEPROMWriteInt(SERIAL_BUFF_RAND, i2cEepromAddress);  //randomize the address of K data to protect the EEPROM. unnecessary when the address is fixed.
          unsigned int i2cEepromAddress = EEPROMReadInt(SERIAL_BUFF);
          skill.copyDataFromBufferToI2cEeprom(i2cEepromAddress, (int8_t *)newCmd);  //must be before the loading to set the period
          skill.loadFrameByDataBuffer();
          token = T_SKILL;
          strcpy(newCmd, "tmp");
          break;
        }
#endif
#endif

      case T_SKILL:
        {
          if (!strcmp("x", newCmd)        // x for random skill
              || strcmp(lastCmd, newCmd)  // won't transform for the same gait. it's better to compare skill->skillName and newCmd. but need more logics for non skill cmd in between
              || skill.period <= 1) {     // for repeating behaviors. if it's set < 1, won't repeat the last behavior

            skill.loadFrame(newCmd);
            // skill.info();
            // PTH("mem",freeMemory());

#ifdef DOUBLE_INFRARED_DISTANCE
            if (skill.period == 1)
              token = ' ';
#endif
          }
          break;
        }

        // case T_TASK_QUEUE:
        //   {
        //     tQueue->createTask();
        //     break;
        //   }

#endif
      case T_REST:
        {
          strcpy(newCmd, "rest");
          skill.loadFrame(newCmd);
          pwm.shutServos();
          gyroBalanceQ = false;
          manualHeadQ = false;
          PTL('g');
          break;
        }
      default:
        break;
    }
    if (token != T_SKILL || skill.period > 0) {
      PTL(token);  //postures, gaits, and other tokens can confirm completion by sending the token back
      if (lastToken == T_SKILL
          && (token == T_JOINTS || token == T_PAUSE || lowerToken == T_GYRO_FINENESS || lowerToken == T_INDEXED_SIMULTANEOUS_ASC || lowerToken == T_INDEXED_SEQUENTIAL_ASC
#ifdef T_PRINT_GYRO
              || lowerToken == T_PRINT_GYRO
#endif
#ifdef T_TILT
              || token == T_TILT
#endif
              ))
        token = T_SKILL;
    }
    resetCmd();
  }

#ifdef MAIN_SKETCH
  if (token == T_SKILL) {
    skill.perform();

#if defined NyBoard_V0_1 || defined NyBoard_V0_2 || !defined GYRO_PIN
    delayMicroseconds(850);  //Slow the robot down to smooth out motion; adjust delay PRN
#endif
    if (skill.period < 0) {  //behaviors
      {                      //use the last frame as a posture
        // newCmd[0] = '\0';
        // arrayNCPY(skill.dutyAngles, skill.dutyAngles + (abs(skill.period) - 1) * skill.frameSize, DOF);
        // skill.period = 1;
        // frame = 0;
        skill.convertTargetToPosture(currentAng);
      }
      for (int i = 0; i < DOF; i++)
        currentAdjust[i] = 0;
      // if (strcmp(newCmd, ""))
      //   skill.loadFrame(newCmd);
      PTL(token);  //behavior can confirm completion by sending the token back
      //      PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
    }
    if (exceptions && lastCmd[strlen(lastCmd) - 1] < 'L' && skill.lookupAddressByName(lastCmd) > 0) {  //lastToken == T_SKILL && lastSkill->period > 0) {
                                                                                                       // if the last command is not a behavior and not a turning gait. case wkF, wkL, wkR, rlL, rlR
      if (lastCmd[0] != '\0')
        skill.loadFrame(lastCmd);
      // read_IMU();
    }
  }
#endif
}
