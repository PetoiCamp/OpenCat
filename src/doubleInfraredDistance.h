/*
Demo for the Petoi double infrared distance sensor

The robot will make different sound by the distance of the obstable in front of it.

When the robot is in static posture:
1. It will turn its head to the object in front of it.
2. When the view angle is larger than 75 degrees, the robot will turn its body to that direction.
3. If the object is too close, the robot will walk backward then walk forward, then sit. 

When the robot is walking:
1. It will trot forward when the front is clear.
2. It will turn when there's an obstacle within 30cm.
3. It will retreat and turn if the obstacle is within 12cm.
4. It will rotate when the obstacle is within 4cm.

Rongzhong Li
Petoi LLC
2023 April 17
*/
#define SENSOR1 A3
#define SENSOR2 A2
#define READING_COUNT 30
#define SENSOR_DISPLACEMENT 3.7
#define MAX_READING 1024

#ifdef NyBoard_V1_0
#define NEOPIXEL_PIN 10  //the code for NeoPixels have to be shrinked to fit in the board
#define NUMPIXELS 7
#elif defined NyBoard_V1_2
#define NEOPIXEL_PIN 10  //the code for NeoPixels have to be shrinked to fit in the board
#define NUMPIXELS 1
#endif

#ifdef NEOPIXEL_PIN
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

bool makeSound = true;



float kp = 0.5;        // Proportional gain
float ki = 0.1;        // Integral gain
float kd = 0.2;        // Derivative gain
float setpoint = 0;    // Target value
float error = 0;       // Difference between setpoint and actual value
float integral = 0;    // Running sum of errors over time
float derivative = 0;  // Rate of change of error over time
float last_error = 0;  // Error in the previous iteration
float currentX = 0;    // Control signal sent to the sensors

float d = SENSOR_DISPLACEMENT;  // Displacement of sensors on the x-axis
int rawL, rawR;
float dL, dR, meanD, maxD, minD;
int meanA = 0, meanB = 0, diffA_B = 0, actualDiff = 0, last = 0;

void resetPID() {
  error = 0;
  last_error = 0;
  integral = 0;
  derivative = 0;
}

void distanceNaive(float dLeft, float dRight) {  //a simple feedback loop without PID
  float minD = min(dLeft, dRight);
  float diff = dRight - dLeft;
  float offset = atan(diff / SENSOR_DISPLACEMENT) * degPerRad;

  if (periodGlobal == 1) {  //posture
    if (minD < 20 && abs(offset) > 5 - minD / 5) {
      // PT("\tmin ");
      // PT(minD);
      // PT("\tdiff ");
      // PT(diff);
      // PT("\toffset ");
      // PT(offset);
      currentX = min(90, max(-90, currentX + offset / 10));
      calibratedPWM(0, currentX, 0.2);
      // PTL();
      FPS();
    }
  }
}

void distancePID(float dLeft, float dRight) {
  // Read the current distances from the sensors
  if (min(dLeft, dRight) < 30) {
    // Calculate the error between the setpoint and the actual values, taking into account the x-axis displacement
    //error = atan((dLeft - dRight) / SENSOR_DISPLACEMENT) * degPerRad;
    error = dLeft - dRight - setpoint;
    // Calculate the integral and derivative terms
    integral = max(-900, min(900, integral + error));
    derivative = error - last_error;

    // Calculate the control signal using the PID formula
    currentX = -max(-90, min(90, kp * error + ki * integral + kd * derivative));

    // Send the control signal to the sensors to adjust their angles
    calibratedPWM(0, currentX, 0);
    // PT('\t');
    // PT(error);
    // PT('\t');
    // PT(integral);
    // PT('\t');
    // PT(derivative);
    // PT('\t');
    // PT(currentX);
    // Save the current error for use in the next iteration
    last_error = error;
  }
}

void doubleInfraredDistanceSetup() {
// put your setup code here, to run once:
#ifdef NEOPIXEL_PIN
  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)
#endif
}

void readDistancePins() {
  rawL = analogRead(SENSOR1) - 24;
  rawR = analogRead(SENSOR2) - 24;
  dL = rawL < 30 ? rawL / 4.0 : 200.0 / sqrt(MAX_READING - rawL);
  dR = rawR < 30 ? rawR / 4.0 : 200.0 / sqrt(MAX_READING - rawR);
  meanD = (dL + dR) / 2;
  maxD = max(dL, dR);
  minD = min(dL, dR);
  if (0) {
    PT("rL ");
    PT(rawL);
    PT("\trR ");
    PT(rawR);
    PT("\tdL ");
    PT(dL);
    PT("\tdR ");
    PT(dR);
    PT("\tmD ");
    PT(meanD);
    PTL();
  }
}

void read_doubleInfraredDistance() {
  readDistancePins();
  if (makeSound && minD > 10 && maxD < 25 && periodGlobal == 1)
    beep(35 - meanD, meanD / 4);
#ifdef NEOPIXEL_PIN
  strip.clear();
  for (int i = 0; i < min(8 - sqrt(dL) * 1.4, strip.numPixels()); i++) {                  // For each pixel in strip..
    strip.setPixelColor(i, strip.Color(255 - meanD * 6, meanD * 6, 128 + currentX * 2));  //  Set pixel's color (in RAM)
    strip.show();
  }
#endif
  if (dL < 1 || dR < 1) {
    readDistancePins();
    if (dL < 1 && dR < 1) {
      // makeSound = !makeSound;
      tQueue->addTask('k', "bk", 1500);
      tQueue->addTask('k', "wkF", 1500);
      tQueue->addTask('k', "sit");
      tQueue->addTask('i', "");
    }
  } else if (periodGlobal == 1) {
    distancePID(dL, dR);
    if (currentX < -75 || currentX > 75) {
      if (currentX < -75) {
        tQueue->addTask('k', "vtR", 2000);
      } else {
        tQueue->addTask('k', "vtL", 2000);
      }
      tQueue->addTask('k', "sit");
      currentX = 0;
      resetPID();
    }
  }
  // distanceNaive(dL, dR);
  else if (periodGlobal > 1 && tQueue->empty()) {  //gait
    tQueue->addTask('i', "");
    if (dL > 30 && dR > 30) {  //free to run
      tQueue->addTask('k', "trF");
      idleTimer = millis() + IDLE_TIME / 2;
      PTLF(" free");
    } else if (dL < 4 || dR < 4) {  //too close. retreat
      tQueue->addTask('k', dL < dR ? "vtR" : "vtL", 2000);
      PTLF(" too close");
    } else {
      idleTimer = millis() + IDLE_TIME;
      if (abs(dR - dL) > 2) {  //one side has longer free distance
        tQueue->addTask('k', dL < dR ? "trR" : "trL", 1000);
        PTLF("turn");
      } else if (dL < 12 || dR < 12) {
        tQueue->addTask('k', dL < dR ? "bkL" : "bkR", 1500);
        tQueue->addTask('k', dL < dR ? "trR" : "trL", 1500);
        PTLF(" retreat and turn");
      }
    }
  }
}
