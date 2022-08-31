#define SOUND1 A2
#define LIGHT1 A4
#define SOUND2 A3
#define LIGHT2 A5
#define BUZZER 5
#define READING_COUNT 100

#define SOUND_MAX 1024
#define SOUND_THRESHOLD SOUND_MAX*0.97


//#define IN1 LIGHT1
//#define IN2 LIGHT2
//
#define IN1 SOUND1
#define IN2 SOUND2



//S_n=S_n-1+(x_n - m_n-1)(x_n - m_n)

#define HISTORY 300
#define EDGE 200
int a[HISTORY], b[HISTORY], currentIndex = 0;


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
int offsetA_B1 = 0, offsetA_B2 = 0;
long meanLight1 = 0, meanLight2 = 0;
int correctedReading(int light) {
  return light - (offsetA_B2 +  (offsetA_B1 - offsetA_B2) / (meanLight1 - meanLight2) * (light - meanLight2));
  //  resetReading();
}

bool sensorConnectedQ(int n) {
  a[currentIndex] = correctedReading(analogRead(IN1));
  b[currentIndex] = correctedReading(analogRead(IN2));
//    a[currentIndex] = analogRead(IN1);
//    b[currentIndex] = analogRead(IN2);
  Serial.print(SOUND_MAX);
  Serial.print('\t');
  Serial.print(0);
  Serial.print('\t');
  //  Serial.print(SOUND_MAX - a[currentIndex]);
  //  Serial.print('\t');
  //  Serial.print(SOUND_MAX - b[currentIndex]);

  Serial.print(a[currentIndex]);
  Serial.print('\t');
  Serial.print(b[currentIndex]);


  Serial.println();

  currentIndex = (currentIndex + 1) % HISTORY;
  //  Serial.println(currentIndex);
  delayMicroseconds(10);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println("start");

  for (int i = 0; i < 50; i++) {
    int a = analogRead(IN1);
    int b = analogRead(IN2);
    offsetA_B1 += (a - b);
    meanLight1 += ((a + b) / 2 /50);
    delay(1);
  }
  offsetA_B1 /= 50; //m1
  Serial.println(offsetA_B1);
  Serial.println("Now cover the two sensors");
  delay(3000);
  for (int i = 0; i < 50; i++) {
    int a = analogRead(IN1);
    int b = analogRead(IN2);
    offsetA_B2 += (a - b);
    meanLight2 += ((a + b) / 2 /50);
    delay(1);
  }
  offsetA_B2 /= 50; //m2
  Serial.println(offsetA_B2);


}

void loop() {
  // put your main code here, to run repeatedly:
  //stats();
  //  sensorConnectedQ(READING_COUNT);
  bool con = sensorConnectedQ(READING_COUNT);
  //  Serial.println(con);
  //  con ? beep(10, 200) : beep(20, 200);
}
