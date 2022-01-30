/* Write and read long data string to I2C EEPROM
   maximum is 65536 bit. i.e. address stops at 65535.
   Extra data will wrap over to address 0

   Rongzhong Li
   August 2018

   Copyright (c) 2018 Petoi LLC.

  The MIT license

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

#include <Wire.h>
#define DEVICE_ADDRESS 0x54    //Address of eeprom chip
#define WIRE_BUFFER 30 //Arduino wire allows 32 byte buffer, with 2 byte for address.
#define WIRE_LIMIT 16 //That leaves 30 bytes for data. use 16 to balance each writes
#define PAGE_LIMIT 32 //AT24C32D 32-byte Page Write Mode. Partial Page Writes Allowed

#define PT(s) Serial.print(s)  //makes life easier
#define PTL(s) Serial.println(s)
#define SIZE 65536/8
#define START 65536/8-200
//#define CLEAR
#define WRITE


void i2c_eeprom_write_byte( unsigned int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  delay(5);  // needs 5ms for write
}

void i2c_eeprom_read_buffer( unsigned int eeaddress, byte *buffer, int length ) {
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(DEVICE_ADDRESS, length);
  int c = 0;
  for ( c = 0; c < length; c++ ) {
    if (Wire.available()) buffer[c] = Wire.read();
    PT((char)buffer[c]);
  }
}
byte i2c_eeprom_read_byte( unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(DEVICE_ADDRESS, 1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void writeLong(unsigned int &eeAddress, char *data, int len) {
  //byte locationInPage = eeAddress % PAGE_LIMIT;
  i2c_eeprom_write_byte(eeAddress++, len);
  PTL("write " + String(len) + " bytes");
  int writtenToEE = 0;
  while (len > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)((eeAddress) >> 8));   // MSB
    Wire.write((int)((eeAddress) & 0xFF)); // LSB
    PT("* current address: ");
    PT((unsigned int)eeAddress % (SIZE));
    PTL("\t0 1 2 3 4 5 6 7 8 9 a b c d e f ");
    PT("\t\t\t\t");
    byte writtenToWire = 0;
    do
    {
      PT(data[writtenToEE]);
      PT(" ");
      Wire.write((byte)data[writtenToEE]);
      writtenToWire++;
      writtenToEE++;
      eeAddress++;
      len--;
    } while (len > 0 && (eeAddress  % PAGE_LIMIT ) && writtenToWire < WIRE_LIMIT);
    Wire.endTransmission();
    delay(6);  // needs 5ms for page write
    PTL();
    PTL("wrote " + String(writtenToWire) + " bytes.");
    if (eeAddress >= SIZE) {
      PTL("EEPROM overflow! Wrap around to the beginning of the address!\n");
    }
  }
  PTL("finish writing");
}

void readLong(unsigned int &eeAddress, char *data) {
  int len = i2c_eeprom_read_byte(eeAddress++);
  PTL("read " + String(len) + " bytes");
  int readFromEE = 0;
  int readToWire = 0;

  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write((int)((eeAddress) >> 8));   // MSB
  Wire.write((int)((eeAddress) & 0xFF)); // LSB
  Wire.endTransmission();
  while (len > 0) {
    Wire.requestFrom(DEVICE_ADDRESS, min(WIRE_BUFFER, len));
    readToWire = 0;
    do {
      if (Wire.available()) data[readFromEE] = Wire.read();
      PT( (char)data[readFromEE]);
      readFromEE++;
    } while (--len > 0 && ++readToWire < WIRE_BUFFER);
    PTL();
  }

  PTL("finish reading");
}

char data[] = "The quick brown fox jumps over the lazy dog. \
The five boxing wizards jump quickly. Pack my box with five dozen liquor jugs. \ 
D is a musical note a whole tone above C, and is known as Re within the fixed-Do solfege system. \
An enharmonic note is Cdouble sharp, which is a diatonic semitone below D. \
When calculated in equal temperament with a reference of A above middle C as 440 Hz, \
the frequency of middle D (D4) is approximately 293.665 Hz. See pitch for a discussion of historical variations in frequency."; // data to write
//observe how the overflow wrapping works

//char data[]={16,-3,5,7,9};
void setup()
{
  Wire.begin(); // initialise the connection
  Serial.begin(115200);
  delay(1);

  PTL("Start");
#ifdef CLEAR  //clear memory for a new write and read test
  PTL("Takes time. Wait for next message...");
  for (int i = 0; i < SIZE; i++)  {
    i2c_eeprom_write_byte( i, '#');
    delay(4);
  }
  PTL("Done clearing!");
#else
  unsigned int eeAddress = START;
  //  data[0] = strlen(data);
#ifdef WRITE
  writeLong(eeAddress, data, strlen(data));
  PTL("current EE address: " + String(eeAddress));
  PTL();
#endif
  eeAddress = START;
  int len = i2c_eeprom_read_byte(eeAddress);
  char *buffer = new char[len];
  readLong(eeAddress, buffer);
  PTL();
  PTL("the buffer is: ");
  for (int i = 0; i < len; i++)
    PT(buffer[i]);
  PTL();
  PTL();
#endif
  PTL("read by individual bytes for checking:");
  int c = 0;
  for (long r = START; r < START + strlen(data); r++) {
    PT((char)i2c_eeprom_read_byte(r + 1));//the first character will be the length, skipped.
    if (++c == 16) {
      PTL();
      c = 0;
    }
    else
      PT(' ');
  }
  PTL();
  PTL("read from address 0"); 
  c = 0;
  for (long r = 0; r < strlen(data); r++) {
    PT((char)i2c_eeprom_read_byte(r));
    if (++c == 16) {
      PTL();
      c = 0;
    }
    else
      PT(' ');
  }
}

void loop() {
}
