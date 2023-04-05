#define SENSOR1 A2
#define SENSOR2 A3
#define READING_COUNT 30
#define SENSOR_DISPLACEMENT 3.7
#define MAX_READING 1024

int meanA = 0, meanB = 0, diffA_B = 0, actualDiff = 0, last = 0;
int tolerance = 20;
float currentX = 0;

void doubleInfraredDistanceSetup() {
  // put your setup code here, to run once:
  for (int i = 0; i < READING_COUNT; i++) {
    int a = analogRead(SENSOR1);
    int b = analogRead(SENSOR2);
    meanA += (a / READING_COUNT);
    meanB += (b / READING_COUNT);
    diffA_B += (a - b) / READING_COUNT;
    delay(1);
  }
}

void read_doubleInfraredDistance() {
  int rawL = analogRead(SENSOR2) - 24;
  int rawR = analogRead(SENSOR1) - 24;
  float dL = rawL < 30 ? rawL / 4.0 : 200.0 / sqrt(MAX_READING - rawL);
  float dR = rawR < 30 ? rawR / 4.0 : 200.0 / sqrt(MAX_READING - rawR);
  float minD = min(dL, dR);
  float diff = dR - dL;
  float offset = atan(diff / SENSOR_DISPLACEMENT) * degPerRad;

  if (periodGlobal == 1) {  //posture
    if (minD < 20 && abs(offset) > 5 - minD / 5) {
      // PT("rawLeft ");
      // PT(rawL);
      // PT("\trawRight ");
      // PT(rawR);
      // PT("\tdL ");
      // PT(dL);
      // PT("\tdR ");
      // PT(dR);
      // // // PT("\tmin ");
      // // // PT(minD);
      // PT("\tdiff ");
      // PT(diff);
      // PT("\toffset ");
      // PT(offset);
      // PT("\tcX ");
      // PT(currentX);
      currentX = min(90, max(-90, currentX + offset / 10));
      calibratedPWM(0, currentX, 0.2);
      // PT("\tcX2 ");
      // PT(currentX);
      // PTL();
      FPS();
    }
  } else if (periodGlobal > 1 && tQueue->empty()) {  //gait
    tQueue->addTask('i', "");
    if (dL > 30 && dR > 30) {  //free to run
      tQueue->addTask('k', "trF");
      // idleTimer = millis() + IDLE_TIME / 4;
    } else if (dL < 3 || dR < 3) {  //too close. retreat
      tQueue->addTask('k', "bk", 1000);
    } else {
      // idleTimer = millis() + IDLE_TIME/2;
      if (abs(dR - dL) > 2)  //one side has longer free distance
        tQueue->addTask('k', dR > dL ? "trR" : "trL", 1000);
      else if (dL < 12 || dR < 12) {
        tQueue->addTask('k', dL < dR ? "bkL" : "bkR", 1500);
        tQueue->addTask('k', dL < dR ? "trR" : "trL", 1500);
      }
    }
  }
}
