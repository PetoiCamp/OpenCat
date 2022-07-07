#include "ir/IRremote.h"

//#include <IRremote.h>
//The included library is identical to the IRremote library by shirriff, version 2.6.1
//Source: https://github.com/Arduino-IRremote/Arduino-IRremote
//Here, we include the decoding functions in our folder only to make it more convenient for newbie users
//All rights belong to the original author, and we follow the MIT license.
//You no longer need to modify ~/Documents/Arduino/libraries/src/IRremote/IRremote.h as mentioned in our old manual.

IRrecv irrecv(IR_PIN);
decode_results results;
//abbreviation //gait/posture/function names
#define K00 "d"       //rest and shutdown all servos
#define K01 "F"       //forward
#define K02 "g"       //turn off gyro feedback to boost speed

#define K10 "L"       //left
#define K11 "balance" //neutral stand up posture
#define K12 "R"       //right

#define K20 "p"       //pause motion and shut off all servos
#define K21 "B"       //backward
#define K22 "c"       //calibration mode with IMU turned off

#define K30 "vt"      //stepping
#define K31 "cr"      //crawl
#define K32 "wk"      //walk

#define K40 "tr"      //trot
#define K41 "sit"     //sit
#define K42 "str"     //stretch

#define K50 "hi"      //greeting
#define K51 "pu"      //push up
#define K52 "pee"     //standng with three legs

#ifdef NYBBLE
#define K60 "stand"//"lu"      //look up
#define K61 "buttUp"    //butt up
#elif defined BITTLE
#define K60 "ck"      //check around
#define K61 "jy"      //joy
#else
#define K60 "fd"      //fold
#define K61 "rt"      //
#endif

#define K62 "z"  //turn on/off the random behaviors
//#define K62 "zero" //call your customized Newbility saved to PROGMEM

//#define K62 "T"    //call the last skill data received from the serial port

#define SHORT_ENCODING // activating this line will use a shorter encoding of the HEX values
String translateIR() // takes action based on IR code received
// describing Remote IR codes.
{
#ifndef SHORT_ENCODING
  switch (results.value) {
    //IR signal    key on IR remote           //key mapping
    case 0xFFA25D: /*PTLF(" CH-");   */       return (F(K00));
    case 0xFF629D: /*PTLF(" CH");  */         return (F(K01));
    case 0xFFE21D: /*PTLF(" CH+"); */         return (F(K02));

    case 0xFF22DD: /*PTLF(" |<<"); */         return (F(K10));
    case 0xFF02FD: /*PTLF(" >>|"); */         return (F(K11));
    case 0xFFC23D: /*PTLF(" >||"); */         return (F(K12));

    case 0xFFE01F: /*PTLF(" -");   */         return (F(K20));
    case 0xFFA857: /*PTLF(" +");  */          return (F(K21));
    case 0xFF906F: /*PTLF(" EQ"); */          return (F(K22));

    case 0xFF6897: /*PTLF(" 0");  */          return (F(K30));
    case 0xFF9867: /*PTLF(" 100+"); */        return (F(K31));
    case 0xFFB04F: /*PTLF(" 200+"); */        return (F(K32));

    case 0xFF30CF: /*PTLF(" 1");  */          return (F(K40));
    case 0xFF18E7: /*PTLF(" 2");  */          return (F(K41));
    case 0xFF7A85: /*PTLF(" 3");  */          return (F(K42));

    case 0xFF10EF: /*PTLF(" 4");  */          return (F(K50));
    case 0xFF38C7: /*PTLF(" 5");  */          return (F(K51));
    case 0xFF5AA5: /*PTLF(" 6");  */          return (F(K52));

    case 0xFF42BD: /*PTLF(" 7");  */          return (F(K60));
    case 0xFF4AB5: /*PTLF(" 8");  */          return (F(K61));
    case 0xFF52AD: /*PTLF(" 9");  */          return (F(K62));

    case 0xFFFFFFFF: return ""; //Serial.println(" REPEAT");
#else
  uint8_t trimmed = (results.value >> 8);
  switch (trimmed) {
    //IR signal    key on IR remote           //key mapping
    case 0xA2: /*PTLF(" CH-");   */       return (F(K00));
    case 0x62: /*PTLF(" CH");  */         return (F(K01));
    case 0xE2: /*PTLF(" CH+"); */         return (F(K02));

    case 0x22: /*PTLF(" |<<"); */         return (F(K10));
    case 0x02: /*PTLF(" >>|"); */         return (F(K11));
    case 0xC2: /*PTLF(" >||"); */         return (F(K12));

    case 0xE0: /*PTLF(" -");   */         return (F(K20));
    case 0xA8: /*PTLF(" +");  */          return (F(K21));
    case 0x90: /*PTLF(" EQ"); */          return (F(K22));

    case 0x68: /*PTLF(" 0");  */          return (F(K30));
    case 0x98: /*PTLF(" 100+"); */        return (F(K31));
    case 0xB0: /*PTLF(" 200+"); */        return (F(K32));

    case 0x30: /*PTLF(" 1");  */          return (F(K40));
    case 0x18: /*PTLF(" 2");  */          return (F(K41));
    case 0x7A: /*PTLF(" 3");  */          return (F(K42));

    case 0x10: /*PTLF(" 4");  */          return (F(K50));
    case 0x38: /*PTLF(" 5");  */          return (F(K51));
    case 0x5A: /*PTLF(" 6");  */          return (F(K52));

    case 0x42: /*PTLF(" 7");  */          return (F(K60));
    case 0x4A: /*PTLF(" 8");  */          return (F(K61));
    case 0x52: /*PTLF(" 9");  */          return (F(K62));

    case 0xFF: return ""; //Serial.println(" REPEAT");
#endif
    default: {
        //Serial.println(results.value, HEX);
      }
      return "";                      //Serial.println("null");
  }// End Case
  //delay(100); // Do not get immediate repeat //no need because the main loop is slow

  // The control could be organized in another way, such as:
  // forward/backward to change the gaits corresponding to different speeds.
  // left/right key for turning left and right
  // number keys for different postures or behaviors
}

String gait = "tr";
char direct = 'F';

String irParser(String raw) {
  //  PT(raw); PT('\t'); PT(gait); PT('\t'); PTL(direct);
  if (raw == "B") {
    gait = "bk";//(gait == "vt") ? "bk" : "vt";
    return gait;
  }
  if (raw == "F" || raw == "L" || raw == "R") {
    direct = raw[0];
    if (direct == 'F' && gait == "bk") {
      //      if (gait == "bk")
      //        gait = "vt";
      //      else if (gait == "vt")
      gait = "tr";
    }
    return gait + direct;
  }
  else if (raw == "vt" || raw == "cr" || raw == "wk" || raw == "tr" || raw == "ph") {
    gait = raw;
    return gait + direct;
  }
  else {
    return raw;
  }
}

void read_infrared() {
  if (irrecv.decode(&results)) {
    String IRsig = irParser(translateIR());
    //PTL(IRsig);
    if (IRsig != "") {
      //      delete [] newCmd;
      //      newCmd = new char[IRsig.length() + 1];
      strcpy(newCmd, IRsig.c_str());
      if (strlen(newCmd) == 1)
        token = newCmd[0];
      else
        token = T_SKILL;
      newCmdIdx = 1;
    }
    irrecv.resume(); // receive the next value
  }
}
