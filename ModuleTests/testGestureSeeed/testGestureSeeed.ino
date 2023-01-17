#include <Wire.h>
#include "paj7620.h" //install the PAJ762 by Seeed through Arduino IDE's library manager
 
void setup()
{
  paj7620Init();
  Serial.begin(115200);
}
 
void loop()
{
    uint8_t data = 0;  // Read Bank_0_Reg_0x43/0x44 for gesture result.
 
    paj7620ReadReg(0x43, 1, &data);  
    
    if (data == GES_UP_FLAG){
      Serial.println("GES_UP_FLAG");        
    }
    else if (data == GES_DOWN_FLAG){
      Serial.println("GES_DOWN_FLAG");      
    }
    else if (data == GES_LEFT_FLAG){
      Serial.println("GES_LEFT_FLAG");      
    }
    else if (data == GES_RIGHT_FLAG){
      Serial.println("GES_RIGHT_FLAG");      
    } 
    else if (data == GES_FORWARD_FLAG){
      Serial.println("GES_FORWARD_FLAG");      
    } 
    else if (data == GES_BACKWARD_FLAG){
      Serial.println("GES_BACKWARD_FLAG");      
    } 
    else if (data == GES_CLOCKWISE_FLAG){
      Serial.println("GES_CLOCKWISE_FLAG");      
    }  
    else if (data == GES_COUNT_CLOCKWISE_FLAG){
      Serial.println("GES_COUNT_CLOCKWISE_FLAG");      
    } 
    else if (data == GES_WAVE_FLAG){
      Serial.println("GES_WAVE_FLAG");      
    }  
}
