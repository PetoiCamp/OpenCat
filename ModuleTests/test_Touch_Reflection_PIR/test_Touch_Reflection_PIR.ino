//connects to the digital Grove socket with D6 and D7
int val;
void setup()
{
    Serial.begin(115200);
}
 
void loop()
{
    val = digitalRead(6);
    Serial.println(val); //prints 1 if there's human or touch event
                         //prints 0 if there's reflection within 4-16mm.
    delay(100);
}
