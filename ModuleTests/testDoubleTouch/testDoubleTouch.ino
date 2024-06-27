#define TOUCH1 6
#define TOUCH2 7
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(2);
  pinMode(TOUCH1, INPUT);
  pinMode(TOUCH2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(TOUCH1));
  Serial.print('\t');
  Serial.print(digitalRead(TOUCH2));
  Serial.print('\t');

  //  Serial.print('\t');
  //  Serial.print(0);
  //  Serial.print('\t');
  //  Serial.print(4096);

  Serial.println();
  delay(2);
}
