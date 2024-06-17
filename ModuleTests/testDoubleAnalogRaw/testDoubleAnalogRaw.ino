#define IN1 A2
#define IN2 A3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println("start");
}

void loop() {
  // put your main code here, to run repeatedly:
  //stats();

  Serial.print("0 1024 ");
  Serial.print(analogRead(IN1));
  delay(5);
  Serial.print('\t');
  Serial.println(analogRead(IN2));
  delay(5);
}
