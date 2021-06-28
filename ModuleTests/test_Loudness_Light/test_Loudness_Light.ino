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
    Serial.print(0);//reference for fixed scaling when plotting
    Serial.print('\t');
    val = analogRead(A2); //analog input level, output 0~1024
    Serial.print(val);
    Serial.print('\t');
    val2 = analogRead(A3); //analog input level2, output 0~1024
    Serial.print(val2);
    
    Serial.println();
    delay(2);
}
