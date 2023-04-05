#define SENSOR1 A2
#define SENSOR2 A3
#define READING_COUNT 30
#define SENSOR_DISPLACEMENT 3.7
#define MAX_READING 1024

#define PT(s) Serial.print(s)                      // abbreviate print commands
#define PT_FMT(s, format) Serial.print(s, format)  // abbreviate print commands
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))  // trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))
#define PTH(head, value) \
  { \
    PT(head); \
    PT('\t'); \
    PTL(value); \
  }

int meanA = 0, meanB = 0, diffA_B = 0, actualDiff = 0, last = 0;

void read_doubleInfraredDistance() {
  int rawL = analogRead(SENSOR2) - 24;
  int rawR = analogRead(SENSOR1) - 24;
  float dL = rawL < 30 ? rawL / 4.0 : 200.0 / sqrt(MAX_READING - rawL);
  float dR = rawR < 30 ? rawR / 4.0 : 200.0 / sqrt(MAX_READING - rawR);
  PT("rawLeft ");
  PT(rawL);
  PT("\trawRight ");
  PT(rawR);
  PT("\tdL ");
  PT(dL);
  PT("\tdR ");
  PTL(dR);
}

void setup() {
  Serial.begin(115200);
}
void loop() {
  read_doubleInfraredDistance();
}