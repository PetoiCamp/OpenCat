/* credit to:
   https://github.com/deviceai/arduino_sensors/blob/master/IRremote/ir_test_code.ino

   Go to the library manager of Arduino IDE (instruction: https://www.arduino.cc/en/Guide/Libraries),
   search and install IRremote.
   Rongzhong Li
   August 2017

  The MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#define SHORT_ENCODING// activating this line will use a shorter encoding of the HEX values
// the original value is formatted as address  code complement
//                                   2Bytes  1Byte   1Byte

#include "IRremote.h"
//you need to change the timer to //#define IR_USE_TIMER1   // tx = pin 9 in IRremoteBoardDefs.h

#define IR_RECEIVE 4 // Signal Pin of IR receiver to Arduino Pin 4
#define BUZZER 3 // the PWM pin the ACTIVE buzzer is attached to
#define BASE_PITCH 1046.50

/*-----( Declare objects )-----*/
IRrecv irrecv(IR_RECEIVE);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

void beep(float note, float duration = 100, int pause = 0, byte repeat = 1 ) {
  for (byte r = 0; r < repeat; r++) {
    if (note == 0)
      delay(duration);
    else
      tone(BUZZER, BASE_PITCH * pow(1.05946, note), duration);
    delay(pause);
  }
}

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn(); // Start the receiver

}/*--(end setup )---*/


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  if (irrecv.decode(&results)) // have we received an IR signal?

  {
    translateIR();
    irrecv.resume(); // receive the next value
  }
}/* --(end main loop )-- */

/*-----( Function )-----*/
void translateIR() // takes action based on IR code received

// describing Remote IR codes

{
#ifndef SHORT_ENCODING
  switch (results.value)
  {
    case 0xFF629D: Serial.println(" FORWARD"); break;
    case 0xFF22DD: Serial.println(" LEFT");    break;
    case 0xFF02FD: Serial.println(" -OK-");    break;
    case 0xFFC23D: Serial.println(" RIGHT");   break;
    case 0xFFA857: Serial.println(" REVERSE"); break;
    case 0xFF6897: Serial.println(" 1");    break; //1
    case 0xFF9867: Serial.println(" 2");    break; //2
    case 0xFFB04F: Serial.println(" 3");    break; //3
    case 0xFF30CF: Serial.println(" 4");    break; //4
    case 0xFF18E7: Serial.println(" 5");    break; //5
    case 0xFF7A85: Serial.println(" 6");    break; //6
    case 0xFF10EF: Serial.println(" 7");    break; //7
    case 0xFF38C7: Serial.println(" 8");    break; //1^
    case 0xFF5AA5: Serial.println(" 9");    break; //2^
    case 0xFF42BD: Serial.println(" *");    break; //3^
    case 0xFF4AB5: Serial.println(" 0");    break; //4^
    case 0xFF52AD: Serial.println(" #");    break; //5^
    case 0xFFFFFFFF: Serial.println(" REPEAT"); break;
#else
  uint8_t trimmed = (results.value >> 8);
  //  Serial.println(results.value, HEX);
  //  Serial.println(cut, HEX);
  switch (trimmed)
  {

    case 0xA2: /*PTLF(" CH-");   */   beep(2);    break;
    case 0x62: /*PTLF(" CH");  */     beep(3);    break;
    case 0xE2: /*PTLF(" CH+"); */     beep(4);    break;

    case 0x22: /*PTLF(" |<<"); */     beep(5);    break;
    case 0x02: /*PTLF(" >>|"); */     beep(6);    break;
    case 0xC2: /*PTLF(" >||"); */     beep(7);    break;

    case 0xE0: /*PTLF(" -");   */     beep(8);    break;
    case 0xA8: /*PTLF(" +");  */      beep(9);    break;
    case 0x90: /*PTLF(" EQ"); */      beep(10);    break;

    case 0x68: /*PTLF(" 0");  */      beep(11);    break; //1
    case 0x98: /*PTLF(" 100+"); */    beep(13);    break; //2
    case 0xB0: /*PTLF(" 200+"); */    beep(15);    break; //3

    case 0x30: /*PTLF(" 1");  */      beep(16);    break; //4
    case 0x18: /*PTLF(" 2");  */      beep(18);    break; //5
    case 0x7A: /*PTLF(" 3");  */      beep(20);    break; //6

    case 0x10: /*PTLF(" 4");  */      beep(22);    break; //7
    case 0x38: /*PTLF(" 5");  */      beep(23);    break; //1^
    case 0x5A: /*PTLF(" 6");  */      beep(25);    break; //2^

    case 0x42: /*PTLF(" 7");  */      beep(27);    break; //3^
    case 0x4A: /*PTLF(" 8");  */      beep(28);    break; //4^
    case 0x52: /*PTLF(" 9");  */      beep(30);    break; //5^

    case 0xFF: return (""); //Serial.println(" REPEAT");
#endif
    default:
      Serial.print("other button");

      Serial.print("\t");
      Serial.println(results.value, HEX);
  }// End Case

  delay(7); // Do not get immediate repeat


}
