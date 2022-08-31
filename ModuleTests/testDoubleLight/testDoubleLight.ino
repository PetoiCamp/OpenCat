#define LIGHT1 A2
#define LIGHT2 A3
#define BUZZER 5
#define READING_COUNT 30

#define IN1 LIGHT1
#define IN2 LIGHT2
#define MAX_READING 1024

int meanA = 0, meanB = 0, diffA_B = 0;
int tolerance = 20;

float mean(int *a, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++)
    sum += a[i];
  return sum / n;
}

float sDev(int *a, float m, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++)
    sum += (a[i] - m) * (a[i] - m);
  return sqrt(sum / n);

}

float correlation(int *a, int *b, int n) {
  double sum = 0;
  for (int i = 0; i < n; i++)
    sum += (a[i] - b[i]) * (a[i] - b[i]);
  return sqrt(sum / n);
}

template <typename T> int8_t sign(T val) {
  return (T(0) < val) - (val < T(0));
}

bool readSensor(int n) {
  int a = analogRead(IN1) - meanA;
  int b = analogRead(IN2) - meanB;
  int offset = a-b;//max(abs(a - b) - tolerance, 0) * sign(a - b);

  Serial.print(MAX_READING);
  Serial.print('\t');
  Serial.print(0);
  Serial.print('\t');
  Serial.print(a);
  Serial.print('\t');
  Serial.print(b);
  Serial.print('\t');
  Serial.print(offset);
  Serial.println();

  delayMicroseconds(50);
}
void stats() {
  //  int sound[READING_COUNT];
  //  int light[READING_COUNT];
  //  float soundMean = 0;
  //  float lightMean = 0;

  //  for (int i = 0; i < READING_COUNT; i++) {
  //    sound[i] = analogRead(A2);
  //    light[i] = analogRead(A3);
  //    //    soundMean += sound[i];
  //    //    lightMean += light[i];
  //    //    Serial.print(sound[i]);
  //    //    Serial.print('\t');
  //    //    Serial.println(light[i]);
  //    delay(1);
  //  }
  //  soundMean /= READING_COUNT;
  //  lightMean /= READING_COUNT;

  //  float soundDev = sDev(sound, soundMean, READING_COUNT);
  //  float lightDev = sDev(light, lightMean, READING_COUNT);
  //  float cor = correlation(sound, light, READING_COUNT);
  //  Serial.println("sound\tlight");
  //  Serial.print(soundMean);
  //  Serial.print('\t');
  //  Serial.print(lightMean);
  //  Serial.println("\tmean");

  //  Serial.print(soundDev);
  //  Serial.print('\t');
  //  Serial.print(lightDev);
  //  Serial.println("\tstdDev");

  //  Serial.print("    ");
  //  Serial.print(cor);
  //  Serial.println("\tcorrelation");
  //  Serial.print(soundDev < 50 ? "Yes" : "Null");
  //  Serial.print('\t');
  //  Serial.print(lightDev < 20 ? "Yes" : "Null");
  //  Serial.print('\t');
  //  Serial.println(cor > 20 ? "connected" : "Null");

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println("start");
  for (int i = 0; i < READING_COUNT; i++) {
    int a = analogRead(IN1);
    int b = analogRead(IN2);
    meanA += (a / READING_COUNT);
    meanB += (b / READING_COUNT);
    diffA_B += (a - b) / READING_COUNT;
    delay(1);
  }

  Serial.print(meanA);
  Serial.print('\t');
  Serial.print(meanB);
  Serial.print('\t');
  Serial.println(diffA_B);
  delay(2000);
  //  resetReading();
}

void loop() {

  bool con = readSensor(READING_COUNT);
  //  Serial.println(con);
  //  con ? beep(10, 200) : beep(20, 200);
}
