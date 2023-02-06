#define LIGHT1 A3
#define LIGHT2 A2
#define BUZZER 5
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
  // Serial.print(a);
  // Serial.print('\t');
  // Serial.print(meanA);
  // Serial.print('\t');
  // Serial.print(b);
  // Serial.print('\t');
  // Serial.print(meanB);
  // Serial.print('\t');
  a -= meanA;
  b -= meanB;
  int offset = b - a;
  int maxL = max(a, b);
  int clippedOffset = min(max(offset, -60), 60);
  // Serial.print(MAX_READING);
  // Serial.print('\t');
  // Serial.print(0);
  // Serial.print("\ta");
  // Serial.print(a);
  // Serial.print("\tb");
  // Serial.print(b);
  // Serial.print("\tmax");
  // Serial.print(maxL);
  // Serial.print("\toffs ");
  // Serial.print(offset);
  // Serial.print("\tclip ");
  // PT(clippedOffset);
  // PT("\tact ");
  // PT(actualOffset);
  // PT("\tlast ");
  // PTL(last);

  if (maxL < -50) {
    tQueue->push_back(new Task('k', "bk", 1000));
    PTL(tQueue->size());
  } else if (maxL < 300) {
    actualOffset = (last + clippedOffset) / 2;
    actualOffset = max(min(actualOffset, 90), -90);
    token = T_INDEXED_SIMULTANEOUS_ASC;
    char temp[7] = "0 ";
    strcat(temp, String(actualOffset).c_str());
    strcpy((char*)dataBuffer, temp);
    last = actualOffset;
    newCmdIdx = 5;
  } else if (maxL > 300 && abs(offset) > 30) {
    tQueue->push_back(new Task('k', (offset > 0 ? "vtR" : "vtL"), 1000));
  }
}
