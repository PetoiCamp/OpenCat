/*
    SeeedOLED.h
    SSD130x OLED Driver Library

    Copyright (c) 2011 seeed technology inc.
    Author        :   Visweswara R
    Create Time   :   Dec 2011
    Change Log    :

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _SSD1315_H_
#define _SSD1315_H_

// SeeedOLED Instruction set addresses

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <WProgram.h>
    #include "Wire.h"
#endif

#define SeeedOLED_Max_X                 127 //128 Pixels
#define SeeedOLED_Max_Y                 63  //64  Pixels

#define PAGE_MODE                       01
#define HORIZONTAL_MODE                 02


#define SeeedOLED_Address               0x3c
#define SeeedOLED_Command_Mode          0x80
#define SeeedOLED_Data_Mode             0x40
#define SeeedOLED_Display_Off_Cmd       0xAE
#define SeeedOLED_Display_On_Cmd        0xAF
#define SeeedOLED_Normal_Display_Cmd    0xA6
#define SeeedOLED_Inverse_Display_Cmd   0xA7
#define SSD1315_SET_MULTI_PLEX          0xA8
#define SeeedOLED_Activate_Scroll_Cmd   0x2F
#define SeeedOLED_Dectivate_Scroll_Cmd  0x2E
#define SSD1315_SET_DISPLAY_CLK_DIV     0xD5
#define SeeedOLED_Set_Brightness_Cmd    0x81
#define SSD1315_CHARGE_PUMP             0x8D

#define Scroll_Left             0x00
#define Scroll_Right            0x01

#define Scroll_2Frames          0x7
#define Scroll_3Frames          0x4
#define Scroll_4Frames          0x5
#define Scroll_5Frames          0x0
#define Scroll_25Frames         0x6
#define Scroll_64Frames         0x1
#define Scroll_128Frames        0x2
#define Scroll_256Frames        0x3


class SeeedOLED {

  public:

    char addressingMode;


    void init(void);

    void setNormalDisplay();
    void setInverseDisplay();

    void sendCommand(unsigned char command);
    void sendData(unsigned char Data);

    void setPageMode();
    void setHorizontalMode();

    void setTextXY(unsigned char Row, unsigned char Column);
    void clearDisplay();
    void setBrightness(unsigned char Brightness);
    void putChar(unsigned char c);
    void putString(const char* String);
    unsigned char putNumber(long n);
    unsigned char putFloat(float floatNumber, unsigned char decimal);
    unsigned char putFloat(float floatNumber);
    void drawBitmap(unsigned char* bitmaparray, int bytes);

    void setHorizontalScrollProperties(unsigned char direction, unsigned char startPage, unsigned char endPage,
                                       unsigned char scrollSpeed);
    void activateScroll();
    void deactivateScroll();

};

extern SeeedOLED SeeedOled;  // SeeedOLED object

#endif
