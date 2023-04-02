#define SENSOR1 A2
#define SENSOR2 A3
#define READING_COUNT 30
#define SENSOR_DISPLACEMENT 3.7
#define MAX_READING 1024

int meanA = 0, meanB = 0, diffA_B = 0, actualDiff = 0, last = 0;
int tolerance = 20;
int currentX = 0;

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
  int a = analogRead(SENSOR1) - 24;
  int b = analogRead(SENSOR2) - 24;
  PT("\ta ");
  PT(a);
  PT("\tb ");
  PT(b);
  float dL = a < 30 ? a / 4.0 : 200.0 / sqrt(MAX_READING - analogRead(SENSOR1));
  float dR = b < 30 ? b / 4.0 : 200.0 / sqrt(MAX_READING - analogRead(SENSOR2));
  // PT(a);
  // PT('\t');
  // PT(meanA);
  // PT('\t');
  // PT(b);
  // PT('\t');
  // PT(meanB);
  // PT('\t');
  // a -= meanA;
  // b -= meanB;
  float diff = dL - dR;
  float minL = min(dL, dR);
  float clippedDiff = min(max(diff, -60), 60);
  PT("\tdL ");
  PT(dL);
  PT("\tdR ");
  PT(dR);
  PT("\tmin ");
  PT(minL);
  PT("\tdiff ");
  PT(diff);
  PT("\toffset ");
  float offset = atan(diff / SENSOR_DISPLACEMENT) * degPerRad;
  PTL(offset);
  if (minL < 20 && abs(offset) > 5 - minL / 5) {
    currentX = min(90, max(-90, currentX + offset / 2));
    char mov[] = { 0, currentX, '~' };  //example using 'I' for Binary commands. it has to end up with '~' because regular 0 can be mistaken as '\0'.
    tQueue->addTask('I', mov);          //the movement starts before the music
  }
  // if (maxL < -50) {
  //   tQueue->addTask('k', "bk", 2000);  // jigs when entering this case for the 2nd time. ???
  //   tQueue->addTask('k', "up");        // jigs when entering this case for the 2nd time. ???
  //   PTL(tQueue->size());
  // } else if (maxL < 300) {
  //   actualDiff = (last + clippedDiff) / 2;
  //   actualDiff = max(min(actualDiff, 90), -90);
  //   token = T_INDEXED_SIMULTANEOUS_BIN;
  //   newCmd[0] = 0;
  //   newCmd[1] = actualDiff;
  //   last = actualDiff;
  //   cmdLen = 2;
  //   newCmdIdx = 5;
  //   PTL(actualDiff);
  // }
  // else if (maxL > 300 && abs(diff) > 30) {
  //   tQueue->addTask('k', (diff > 0 ? "vtR" : "vtL"), 1000);
  // }
}
