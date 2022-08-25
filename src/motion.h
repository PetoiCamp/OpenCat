

// balancing parameters
#define ROLL_LEVEL_TOLERANCE 5//the body is still considered as level, no angle adjustment
#define PITCH_LEVEL_TOLERANCE 3
int8_t levelTolerance[2] = {ROLL_LEVEL_TOLERANCE, PITCH_LEVEL_TOLERANCE}; //the body is still considered as level, no angle adjustment

//the following coefficients will be divided by radPerDeg in the adjust() function. so (float) 0.1 can be saved as (int8_t) 1
//this trick allows using int8_t array insead of float array, saving 96 bytes and allows storage on EEPROM
#define panF 60
#define tiltF 60
#define sRF 50    //shoulder roll factor
#define sPF 12    //shoulder pitch factor
#define uRF 60    //upper leg roll factor
#define uPF 30    //upper leg pitch factor
#define lRF (-1.5*uRF)  //lower leg roll factor 
#define lPF (-1.5*uPF)  //lower leg pitch factor
#define LEFT_RIGHT_FACTOR 1.2
#define FRONT_BACK_FACTOR 1.2
#define POSTURE_WALKING_FACTOR 0.5
#ifdef POSTURE_WALKING_FACTOR
float postureOrWalkingFactor;
#endif

#ifdef X_LEG  // >< leg
float adaptiveParameterArray[16][2] = { // they will be saved as int8_t in EEPROM
  { -panF, 0}, { -panF, -tiltF}, { -2 * panF, 0}, {0, 0},
  {sRF, -sPF}, { -sRF, -sPF}, { -sRF, sPF}, {sRF, sPF},
  {uRF, uPF}, {uRF, uPF}, { -uRF, uPF}, { -uRF, uPF},
  {lRF, lPF}, {lRF, lPF}, { -lRF, lPF}, { -lRF, lPF}
};
#else         // >> leg
float adaptiveParameterArray[16][2] = { // they will be saved as int8_t in EEPROM
  { -panF, 0}, { -panF / 2, -tiltF}, { -2 * panF, 0}, {0, 0},
  {sRF, -sPF}, { -sRF, -sPF}, { -sRF, sPF}, {sRF, sPF},
  {uRF, uPF}, {uRF, uPF}, {uRF, uPF}, {uRF, uPF},
  {lRF, -0.5 * lPF}, {lRF, -0.5 * lPF}, {lRF, 0.5 * lPF}, {lRF, 0.5 * lPF}
};
#endif

float RollPitchDeviation[2];
int8_t ramp = 1;


float adjust(byte i) {
  float rollAdj, pitchAdj, adj;
  if (i == 1 || i > 3)  {//check idx = 1
    bool leftQ = (i - 1 ) % 4 > 1 ? true : false;
    //bool frontQ = i % 4 < 2 ? true : false;
    //bool upperQ = i / 4 < 3 ? true : false;
    float leftRightFactor = 1;
    if ((leftQ && ramp * RollPitchDeviation[0] > 0 )// operator * is higher than &&
        || ( !leftQ && ramp * RollPitchDeviation[0] < 0))
      leftRightFactor = LEFT_RIGHT_FACTOR * abs(ramp);
    rollAdj = fabs(RollPitchDeviation[0]) * eeprom(ADAPT_PARAM, i, 0, 2) * leftRightFactor;

  }
  else
    rollAdj = RollPitchDeviation[0] * eeprom(ADAPT_PARAM, i, 0, 2) ;
  currentAdjust[i] = radPerDeg * (
#ifdef POSTURE_WALKING_FACTOR
                       (i > 3 ? postureOrWalkingFactor : 1) *
#endif

                       rollAdj - /*ramp * */ eeprom(ADAPT_PARAM, i, 1, 2) * ((i % 4 < 2) ? ( RollPitchDeviation[1]) : RollPitchDeviation[1]));

  return currentAdjust[i];
}


void calibratedPWM(byte i, float angle, float speedRatio = 0) {
  /*float angle = max(-SERVO_ANG_RANGE/2, min(SERVO_ANG_RANGE/2, angle));
    if (i > 3 && i < 8)
    angle = max(-5, angle);*/
  angle = max(float(loadAngleLimit(i, 0)), min(float(loadAngleLimit(i, 1)), angle));
  int duty0 = EEPROMReadInt(CALIBRATED_ZERO_POSITIONS + i * 2) + currentAng[i] * eeprom(ROTATION_DIRECTION, i);
  currentAng[i] = angle;
  int duty = EEPROMReadInt(CALIBRATED_ZERO_POSITIONS + i * 2) + angle * eeprom(ROTATION_DIRECTION, i);
  int steps = speedRatio > 0 ? int(round(abs(duty - duty0) / 1.0/*degreeStep*/ / speedRatio)) : 0;
  //if default speed is 0, no interpolation will be used
  //otherwise the speed ratio is compared to 1 degree per second.
  for (int s = 0; s <= steps; s++) {
    pwm.writeAngle(i, duty + (steps == 0 ? 0 : (1 + cos(M_PI * s / steps)) / 2 * (duty0 - duty)));
  }
}

template <typename T> void allCalibratedPWM(T * dutyAng, byte offset = 0) {
  for (int8_t i = DOF - 1; i >= offset; i--) {
    calibratedPWM(i, dutyAng[i]);
  }
}

template <typename T> void transform( T * target, byte angleDataRatio = 1, float speedRatio = 2, byte offset = 0) {
  if (speedRatio <= 0
#ifdef SERVO_SLOW_BOOT
      || servoOff
#endif
     ) { // the speed ratio should be >0, if the user enters some large numbers, it will become negative
    allCalibratedPWM(target, offset);
#ifdef SERVO_SLOW_BOOT
    if (servoOff) { //slow boot up for servos
      delay(500);
      servoOff = false;
    }
#endif
  }
  else {
    int *diff = new int [DOF - offset], maxDiff = 0;
    for (byte i = offset; i < DOF; i++) {
      diff[i - offset] =   currentAng[i] - target[i - offset] * angleDataRatio;
      maxDiff = max(maxDiff, abs( diff[i - offset]));
    }

    int steps = int(round(maxDiff / 1.0/*degreeStep*/ / speedRatio));//default speed is 1 degree per step

    for (int s = 0; s <= steps; s++) {
      for (byte i = offset; i < DOF; i++) {
        float dutyAng = (target[i - offset] * angleDataRatio + (steps == 0 ? 0 : (1 + cos(M_PI * s / steps)) / 2 * diff[i - offset]));
        calibratedPWM(i,  dutyAng);
      }
    }
    delete [] diff;
  }
}
