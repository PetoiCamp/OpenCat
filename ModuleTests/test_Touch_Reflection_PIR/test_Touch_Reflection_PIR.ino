//connects to the digital Grove socket with D8 and D9
#define DIGITAL1 8
#define DIGITAL2 9
int val;
void setup()
{
    Serial.begin(115200);
  Serial.setTimeout(2);
  pinMode(DIGITAL1, INPUT);
  pinMode(DIGITAL2, INPUT);
}
 
void loop()
{
  val = digitalRead(DIGITAL1);
  Serial.println(val); //prints 1 if there's human or touch event
                         //prints 0 if there's reflection within 4-16mm.
    delay(100);
}
