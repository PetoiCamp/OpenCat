
void dealWithExceptions() {
  //  soundFallOver();
  //  for (int m = 0; m < 2; m++)
  //    meow(30 - m * 12, 42 - m * 12, 20);
  //  beep(15, 100, 100, 2);
  meow();
  token = T_SKILL;
  strcpy(newCmd, "rc");
  newCmdIdx = -1;
}

#ifdef VOLTAGE_DETECTION_PIN
bool lowBattery() {
  float voltage = analogRead(VOLTAGE_DETECTION_PIN);
  if (voltage < LOW_VOLTAGE) { //if battery voltage < threshold, it needs to be recharged
    //give the robot a break when voltage drops after sprint
    //adjust the thresholds according to your batteries' voltage
    //if set too high, the robot will stop working when the battery still has power.
    //If too low, the robot may not alarm before the battery shuts off
    PTF("Low power:");
    PT(voltage / 99);
    PTL('V');
    playMelody(MELODY_LOW_BATTERY);
    int8_t bStep = 1;
    for (byte brightness = 1; brightness > 0; brightness += bStep) {
#ifdef NEOPIXEL_PIN
      pixel.setPixelColor(0, pixel.Color(brightness, 0, 0));
      pixel.show();
#elif defined LED_PIN
      analogWrite(LED_PIN, 255 - brightness);
#endif
      if (brightness == 255)
        bStep = -1;
      delay(10);
    }
    return true;
  }
  return false;
}
#endif

void resetCmd() {
  //  PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  if (strcmp(newCmd, "rc") && token != T_INDEXED_SIMULTANEOUS_BIN && token != T_LISTED_BIN && token != T_MOVE_BIN && token != T_SKILL_DATA) {
    delete [] lastCmd;
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
    if (newCmdIdx < 5 && token != T_BEEP && token != T_MEOW && token != T_LISTED_BIN && token != T_INDEXED_SIMULTANEOUS_BIN && token != T_TILT)
      beep(10 + newCmdIdx * 2); //ToDo: check the muted sound when newCmdIdx = -1
    if ((lastToken == T_CALIBRATE || lastToken == T_REST) && token != T_CALIBRATE) {
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
#ifdef GYRO
      case T_GYRO:
        //      case T_PRINT_GYRO:
        //      case T_VERBOSELY_PRINT_GYRO:
#endif
      case T_RANDOM_MIND:
        //      case T_RAMP:
        {
#ifdef GYRO_PIN
          if (token == T_GYRO) {
            checkGyro = !checkGyro;
            token = checkGyro ? 'G' : 'g';  //G for activated gyro
          }
          //          else if (token == T_PRINT_GYRO) {
          //            print6Axis();
          //          }
          //          else if (token == T_VERBOSELY_PRINT_GYRO) {
          //            printGyro = !printGyro;
          //            token = printGyro ? 'V' : 'v';  //V for verbosely print gyro data
          //          }
          else
#endif
#ifdef RANDOM_MIND
            if (token == T_RANDOM_MIND) {
              autoSwitch = !autoSwitch;
              token = autoSwitch ? 'Z' : 'z';  //Z for active random mind
            }
#endif
          //          else if (token == T_RAMP) {//reverse the adjustment direction
          //            ramp = -ramp;
          //            token = ramp > 0 ? 'R' : 'r';
          //          }
          //          PTL(token);
          //          if (lastToken == T_SKILL)
          //            token = T_SKILL;
          break;
        }
      case T_PAUSE: {
          tStep = !tStep; //tStep can be -1
          token = tStep ? 'p' : 'P';      //P for pause activated
          if (!tStep)
            pwm.shutServos();
          break;
        }
      case T_JOINTS: { //show the list of current joint anles
          printTable(currentAng);
          break;
        }
      case T_MELODY: {
          playMelody(MELODY_1);
          break;
        }
      case T_SAVE: {
          PTLF("saved");
          saveCalib(servoCalib);
          break;
        }
      case T_ABORT: {
          PTLF("aborted");
          for (byte i = 0; i < DOF; i++) {
            servoCalib[i] = eeprom(CALIB, i);
          }
          break;
        }
      case T_CALIBRATE: //calibration
      case T_MOVE_ASC: //move multiple indexed joints to angles once at a time (ASCII format entered in the serial monitor)
      case T_INDEXED_SIMULTANEOUS_ASC: //move multiple indexed joints to angles simultaneously (ASCII format entered in the serial monitor)
      case T_TILT:  //tilt the robot, format: t axis angle. 0:yaw, 1:pitch, 2:roll
      case T_MEOW: //meow
      case T_BEEP: //beep(tone, duration): tone 0 is pause, duration range is 0~255
        {
          int targetFrame[DOF];
          //          for (int i = 0; i < DOF; i += 1) {
          //            targetFrame[i] = currentAng[i];
          //          }
          arrayNCPY(targetFrame, currentAng, DOF);
          char *pch;
          char *input = (token == T_TILT) ? newCmd : (char*)dataBuffer;
          pch = strtok ((char*)input, " ,");
          do {  //it supports combining multiple commands at one time
            //for example: "m8 40 m8 -35 m 0 50" can be written as "m8 40 8 -35 0 50"
            //the combined commands should be less than four. string len <=30 to be exact.
            int target[2] = {};
            byte inLen = 0;
            for (byte b = 0; b < 2 && pch != NULL; b++) {
              target[b] = atoi(pch);
              pch = strtok (NULL, " ,\t");
              inLen++;
            }
            targetFrame[target[0]] = target[1];
            int angleStep = 0;
            if (token == T_CALIBRATE) {
              checkGyro = false;
              autoSwitch = false;
              if (lastToken != T_CALIBRATE) {
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
              int duty = EEPROMReadInt(ZERO_POSITIONS + target[0] * 2) + float(servoCalib[target[0]])  * eeprom(ROTATION_DIRECTION, target[0]);
              pwm.writeAngle(target[0], duty);
              printTable(servoCalib);
              //              PT(token);
              //              printList(target, 2);
            }
            else if (token == T_MOVE_ASC) {
              transform(targetFrame, 1, 2);
            }
            else if (token == T_TILT) {
              yprTilt[target[0]] = target[1];
            }
            else if (token == T_MEOW) {
              meow(rand() % 2 + 1, (rand() % 4 + 2) * 5);
            }
            else if (token == T_BEEP) {
              beep(target[0], target[1], 50);
            }
          } while (pch != NULL);
          if (token == T_INDEXED_SIMULTANEOUS_ASC) {
            transform(targetFrame, 1, 2);
          }
          delete []pch;
          break;
        }

      // this block handles array like arguments
      case T_MOVE_BIN:
      case T_INDEXED_SIMULTANEOUS_BIN: {//indexed joint motions: joint0, angle0, joint1, angle1, ... (binary encoding)
          int targetFrame[DOF];
          //          for (int i = 0; i < DOF; i++) {
          //            targetFrame[i] = currentAng[i];
          //          }
          arrayNCPY(targetFrame, currentAng, DOF);
          for (int i = 0; i < cmdLen; i += 2) {
            targetFrame[dataBuffer[i]] = dataBuffer[i + 1];
            if (token == T_MOVE_BIN) {
              transform(targetFrame, 1, 2);
              //              delay(10);
            }
          }
          if (token == T_INDEXED_SIMULTANEOUS_BIN)
            transform(targetFrame, 1, transformSpeed);
          break;
        }
      case T_LISTED_BIN: {//list of all 16 joint: angle0, angle2,... angle15 (binary encoding)
          transform(dataBuffer, 1, transformSpeed); //need to add angleDataRatio if the angles are large
          break;
        }
      case T_BEEP_BIN: {
          for (byte b = 0; b < cmdLen / 2; b++)
            beep(dataBuffer[2 * b], 1000 / dataBuffer[2 * b + 1]);
          break;
        }
      case T_TEMP: {//call the last skill data received from the serial port
          skill.loadDataFromI2cEeprom((unsigned int)EEPROMReadInt(SERIAL_BUFF));
          skill.loadFrameByDataBuffer();
          PTL(token);
          token = T_SKILL;
          break;
        }
#ifdef  T_SKILL_DATA
      case T_SKILL_DATA: {//takes in the skill array from the serial port, load it as a regular skill object and run it locally without continuous communication with the master
          skill.copyDataFromBufferToI2cEeprom((unsigned int)EEPROMReadInt(SERIAL_BUFF), dataBuffer);
          skill.loadFrameByDataBuffer();
          token = T_SKILL;
          break;
        }
#endif
      case T_SKILL: {
          if (strcmp(lastCmd, newCmd) //won't transform for the same gait. it's better to compare skill->skillName and newCmd. but need more logics for non skill cmd in between
              || skill.period <= 1) { // for repeating behaviors
            skill.loadFrame(newCmd);
          }
          break;
        }
#endif
      case T_REST: {
          strcpy(newCmd, "rest");
          skill.loadFrame(newCmd);
          pwm.shutServos();
          checkGyro = false;
          PTL(T_GYRO);
          break;
        }
    }
    if (token != T_SKILL || skill.period > 0) {
      PTL(token);//postures, gaits, and other tokens can confirm completion by sending the token back
      char lowerToken = tolower(token);
      if ((lowerToken == T_GYRO || lowerToken == T_PRINT_GYRO || token == T_JOINTS) && lastToken == T_SKILL || token == T_PAUSE || token == T_TILT)
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
    delayMicroseconds(850); //Slow the robot down to smooth out motion; adjust delay time PRN
  }
  if (skill.period < 0 ) {
    if (exceptions && skill.lookupAddressByName(lastCmd) > 0) { //lastToken == T_SKILL && lastSkill->period > 0) {
      strcpy(newCmd, lastCmd);
      for (int i = 0; i < DOF; i++)
        currentAdjust[i] = 0;
    }
    else {
      strcpy(newCmd, "balance");
    }
    if (strcmp(newCmd, ""))
      skill.loadFrame(newCmd);
    PTL(token);//behavior can confirm completion by sending the token back
    //      PTL("lastT:" + String(lastToken) + "\tT:" + String(token) + "\tLastCmd:" + String(lastCmd) + "\tCmd:" + String(newCmd));
  }
#endif
}
