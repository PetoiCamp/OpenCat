#define SOUND A2
#define LIGHT A3
#define BUZZER 5
#define READING_COUNT 100

//S_n=S_n-1+(x_n - m_n-1)(x_n - m_n)

void beep(int8_t note, float duration = 10, int pause = 0, byte repeat = 1 ) {
  if (note == 0) {//rest note
    analogWrite(BUZZER, 0);
    delay(duration);
    return;
  }
  int freq = 220 * pow(1.059463, note - 1); // 1.059463 comes from https://en.wikipedia.org/wiki/Twelfth_root_of_two
  float period = 1000000.0 / freq;
  for (byte r = 0; r < repeat; r++) {
    for (float t = 0; t < duration * 1000; t += period) {
      analogWrite(BUZZER, 150);      // Almost any value can be used except 0 and 255
      // experiment to get the best tone
      delayMicroseconds(period / 2);        // rise for half period
      analogWrite(BUZZER, 0);       // 0 turns it off
      delayMicroseconds(period / 2);        // down for half period
    }
    delay(pause);
  }
}

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

bool sensorConnectedQ(int n) {
  float mean = 0;
  float bLag = analogRead(A3);

  for (int i = 0; i < READING_COUNT; i++) {
    int a, b;
    a = analogRead(A2);
    b = analogRead(A3);
    mean = mean + ((a - b) * (a - b) - mean) / (i + 1);
    //    Serial.print(a);
    //    Serial.print('\t');
    //    Serial.print(b);
    //    Serial.print('\t');
    //    Serial.print(mean);
    //    Serial.println('\t');

    if (abs(a - b) > 50 && abs(b - bLag) < 5) {
      //      Serial.print(a);
      //      Serial.print('\t');
      //      Serial.print(b);
      return true;
    }

    bLag = b;
    delay(1);
  }
  //Serial.println(sqrt(sum));

  return sqrt(mean) > 20 ? true : false;

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
  for (int i = 0; i < 50; i++) {
    analogRead(A2);
    analogRead(A3);
    delay(1);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  //stats();
  //  sensorConnectedQ(READING_COUNT);
  bool con = sensorConnectedQ(READING_COUNT);
  Serial.println(con);
  //  con ? beep(10, 200) : beep(20, 200);
}
