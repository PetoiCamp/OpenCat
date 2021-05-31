//connect the module to the analog grove socket

int val;
int val2;
void setup()
{
    Serial.begin(115200);
}
 
void loop()
{ 
    Serial.print(1024);//reference for fixed scaling when plotting
    Serial.print('\t');
    val = analogRead(A2); //sound level, output 0~1024
    Serial.print(val);

    Serial.println();
    delay(10);
}
