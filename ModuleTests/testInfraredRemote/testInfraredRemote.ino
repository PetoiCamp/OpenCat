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

#include "IRremote.h"

//#include <IRremote.h>
//The included library is identical to the IRremote library by shirriff, version 2.6.1
//Source: https://github.com/Arduino-IRremote/Arduino-IRremote
//Here, we include the decoding functions in our folder only to make it more convenient for newbie users
//All rights belong to the original author, and we follow the MIT license.
//You no longer need to modify ~/Documents/Arduino/libraries/src/IRremote/IRremote.h as mentioned in our old manual.

#define IR_PIN 4
IRrecv irrecv(IR_PIN);
decode_results results;
//abbreviation   //gait/posture/function names
#define K00 "1"  //rest and shutdown all servos
#define K01 "2"  //forward
#define K02 "3"  //turn off gyro feedback to boost speed

#define K10 "4"  //left
#define K11 "5"  //neutral stand posture
#define K12 "6"  //right

#define K20 "7"  //pause motion and shut off all servos
#define K21 "8"  //backward
#define K22 "9"  //calibration mode with IMU turned off

#define K30 "10"  //stepping
#define K31 "11"  //crawl
#define K32 "12"  //walk

#define K40 "13"  //trot
#define K41 "14"  //sit
#define K42 "15"  //stretch

#define K50 "16"  //greeting
#define K51 "17"  //push up
#define K52 "18"  //standng with three legs

#define K60 "19"  //check around
#define K61 "20"  //scratch
#define K62 "21"  //call your customized Newbility saved to PROGMEM

#define SHORT_ENCODING  // activating this line will use a shorter encoding of the HEX values
String translateIR()    // takes action based on IR code received
// describing Remote IR codes.
{
#ifndef SHORT_ENCODING
  switch (results.value) {
    //IR signal    key on IR remote           //key mapping
    case 0xFFA25D: /*PTLF(" CH-");   */ return (F(K00));
    case 0xFF629D: /*PTLF(" CH");  */ return (F(K01));
    case 0xFFE21D: /*PTLF(" CH+"); */ return (F(K02));

    case 0xFF22DD: /*PTLF(" |<<"); */ return (F(K10));
    case 0xFF02FD: /*PTLF(" >>|"); */ return (F(K11));
    case 0xFFC23D: /*PTLF(" >||"); */ return (F(K12));

    case 0xFFE01F: /*PTLF(" -");   */ return (F(K20));
    case 0xFFA857: /*PTLF(" +");  */ return (F(K21));
    case 0xFF906F: /*PTLF(" EQ"); */ return (F(K22));

    case 0xFF6897: /*PTLF(" 0");  */ return (F(K30));
    case 0xFF9867: /*PTLF(" 100+"); */ return (F(K31));
    case 0xFFB04F: /*PTLF(" 200+"); */ return (F(K32));

    case 0xFF30CF: /*PTLF(" 1");  */ return (F(K40));
    case 0xFF18E7: /*PTLF(" 2");  */ return (F(K41));
    case 0xFF7A85: /*PTLF(" 3");  */ return (F(K42));

    case 0xFF10EF: /*PTLF(" 4");  */ return (F(K50));
    case 0xFF38C7: /*PTLF(" 5");  */ return (F(K51));
    case 0xFF5AA5: /*PTLF(" 6");  */ return (F(K52));

    case 0xFF42BD: /*PTLF(" 7");  */ return (F(K60));
    case 0xFF4AB5: /*PTLF(" 8");  */ return (F(K61));
    case 0xFF52AD: /*PTLF(" 9");  */ return (F(K62));

    case 0xFFFFFFFF: return "";  //PTL(" REPEAT");
#else
  uint8_t trimmed = (results.value >> 8);
  switch (trimmed) {
    //IR signal    key on IR remote           //key mapping
    case 0xA2: /*PTLF(" CH-");   */ return (F(K00));
    case 0x62: /*PTLF(" CH");  */ return (F(K01));
    case 0xE2: /*PTLF(" CH+"); */ return (F(K02));

    case 0x22: /*PTLF(" |<<"); */ return (F(K10));
    case 0x02: /*PTLF(" >>|"); */ return (F(K11));
    case 0xC2: /*PTLF(" >||"); */ return (F(K12));

    case 0xE0: /*PTLF(" -");   */ return (F(K20));
    case 0xA8: /*PTLF(" +");  */ return (F(K21));
    case 0x90: /*PTLF(" EQ"); */ return (F(K22));

    case 0x68: /*PTLF(" 0");  */ return (F(K30));
    case 0x98: /*PTLF(" 100+"); */ return (F(K31));
    case 0xB0: /*PTLF(" 200+"); */ return (F(K32));

    case 0x30: /*PTLF(" 1");  */ return (F(K40));
    case 0x18: /*PTLF(" 2");  */ return (F(K41));
    case 0x7A: /*PTLF(" 3");  */ return (F(K42));

    case 0x10: /*PTLF(" 4");  */ return (F(K50));
    case 0x38: /*PTLF(" 5");  */ return (F(K51));
    case 0x5A: /*PTLF(" 6");  */ return (F(K52));

    case 0x42: /*PTLF(" 7");  */ return (F(K60));
    case 0x4A: /*PTLF(" 8");  */ return (F(K61));
    case 0x52: /*PTLF(" 9");  */ return (F(K62));

    case 0xFF: return ("REPEAT");
#endif
    default:
      {
        Serial.println(results.value, HEX);
      }
      return "";  //PTL("null");
  }               // End Case
  //delay(100); // Do not get immediate repeat //no need because the main loop is slow

  // The control could be organized in another way, such as:
  // forward/backward to change the gaits corresponding to different speeds.
  // left/right key for turning left and right
  // number keys for different postures or behaviors
}

void read_infrared() {
  if (irrecv.decode(&results)) {
    String IRsig = translateIR();
    // PTL(IRsig);
    // if (IRsig != "") {
    Serial.println(IRsig);
    // }
    irrecv.resume();  // receive the next value
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("IR test start");
  irrecv.enableIRIn();  // Start the receiver
}

void loop() {
  read_infrared();
  delay(10);
}