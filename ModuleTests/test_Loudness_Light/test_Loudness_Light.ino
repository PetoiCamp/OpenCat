//connect the module to the analog grove socket

int val;
void setup()
{
    Serial.begin(115200);
}
 
void loop()
{
    val = analogRead(A2);
    Serial.println(val);//  sound/light level, output 0~1024
    delay(100);
}
