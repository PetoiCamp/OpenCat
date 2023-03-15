#define LIGHT1 A2
#define LIGHT2 A3
#define READING_COUNT 30

#define MAX_READING 1024

int meanA = 0, meanB = 0, diffA_B = 0, actualOffset = 0, last = 0;
int tolerance = 20;


void doubleLightSetup() {
  // put your setup code here, to run once:
  for (int i = 0; i < READING_COUNT; i++) {
    int a = analogRead(LIGHT1);
    int b = analogRead(LIGHT2);
    meanA += (a / READING_COUNT);
    meanB += (b / READING_COUNT);
    diffA_B += (a - b) / READING_COUNT;
    delay(1);
  }
}

void read_doubleLight() {
  int a = analogRead(LIGHT1);
  int b = analogRead(LIGHT2);
  // PT(a);
  // PT('\t');
  // PT(meanA);
  // PT('\t');
  // PT(b);
  // PT('\t');
  // PT(meanB);
  // PT('\t');
  a -= meanA;
  b -= meanB;
  int offset = b - a;
  int maxL = max(a, b);
  int clippedOffset = min(max(offset, -60), 60);
  // PT(MAX_READING);
  // PT('\t');
  // PT(0);
  // PT("\ta");
  // PT(a);
  // PT("\tb");
  // PT(b);
  // PT("\tmax");
  // PT(maxL);
  // PT("\toffs ");
  // PT(offset);
  // PT("\tclip ");
  // PT(clippedOffset);
  // PT("\tact ");
  // PT(actualOffset);
  // PT("\tlast ");
  // PTL(last);

  if (maxL < -50) {
    tQueue->addTask('k', "bk", 2000);  // jigs when entering this case for the 2nd time. ???
    tQueue->addTask('k', "up");        // jigs when entering this case for the 2nd time. ???
    PTL(tQueue->size());
  } else if (maxL < 300) {
    actualOffset = (last + clippedOffset) / 2;
    actualOffset = max(min(actualOffset, 90), -90);
    token = T_INDEXED_SIMULTANEOUS_BIN;
    newCmd[0] = 0;
    newCmd[1] = actualOffset;
    last = actualOffset;
    cmdLen = 2;
    newCmdIdx = 5;
    PTL(actualOffset);
  } 
  // else if (maxL > 300 && abs(offset) > 30) {
  //   tQueue->addTask('k', (offset > 0 ? "vtR" : "vtL"), 1000);
  // }
}
