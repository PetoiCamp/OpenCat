/*
  This file is part of the Arduino_APDS9960 library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino_APDS9960.h"

APDS9960::APDS9960(TwoWire& wire, int intPin) :
  _wire(wire),
  _intPin(intPin),
  _gestureEnabled(false),
  _proximityEnabled(false),
  _colorEnabled(false),
  _gestureIn(false),
  _gestureDirectionX(0),
  _gestureDirectionY(0),
  _gestureDirInX(0),
  _gestureDirInY(0),
  _gestureSensitivity(20),
  _detectedGesture(GESTURE_NONE)
{
}

APDS9960::~APDS9960()
{
}

bool APDS9960::begin() {
  _wire.begin();
    
  // Check ID register
  uint8_t id;
  if (!getID(&id)) return false;
  if (id!=0xA8 && id!=0xAB) return false;
    
  // Disable everything
  if (!setENABLE(0x00)) return false;
  if (!setWTIME(0xFF)) return false;
  if (!setGPULSE(0x8F)) return false; // 16us, 16 pulses // default is: 0x40 = 8us, 1 pulse
  if (!setPPULSE(0x8F)) return false; // 16us, 16 pulses // default is: 0x40 = 8us, 1 pulse
  if (!setGestureIntEnable(true)) return false;
  if (!setGestureMode(true)) return false;
  if (!enablePower()) return false;
  if (!enableWait()) return false;
  // set ADC integration time to 10 ms
  if (!setATIME(256 - (10 / 2.78))) return false;
  // set ADC gain 4x (0x00 => 1x, 0x01 => 4x, 0x02 => 16x, 0x03 => 64x)
  if (!setCONTROL(0x02)) return false;
  delay(10);
  // enable power
  if (!enablePower()) return false;

  if (_intPin > -1) {
    pinMode(_intPin, INPUT);
  }

  return true;
}

void APDS9960::end() {
  // Disable everything
  setENABLE(0x00);

  _gestureEnabled = false;

  _wire.end();
}

// Sets the LED current boost value:
// 0=100%, 1=150%, 2=200%, 3=300%
bool APDS9960::setLEDBoost(uint8_t boost) {
  uint8_t r;
  if (!getCONFIG2(&r)) return false;
  r &= 0b11001111;
  r |= (boost << 4) & 0b00110000;
  return setCONFIG2(r);
}

void APDS9960::setGestureSensitivity(uint8_t sensitivity) {
  if (sensitivity > 100) sensitivity = 100;
  _gestureSensitivity = 100 - sensitivity;
}

void APDS9960::setInterruptPin(int pin) {
  _intPin = pin;
}

bool APDS9960::setGestureIntEnable(bool en) {
    uint8_t r;
    if (!getGCONF4(&r)) return false;
    if (en) {
      r |= 0b00000010;
    } else {
      r &= 0b11111101;
    }
    return setGCONF4(r);
}

bool APDS9960::setGestureMode(bool en)
{
    uint8_t r;
    if (!getGCONF4(&r)) return false;
    if (en) {
      r |= 0b00000001;
    } else {
      r &= 0b11111110;
    }
    return setGCONF4(r);
}

bool APDS9960::enablePower() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00000001) != 0) return true;
  r |= 0b00000001;
  return setENABLE(r);
}

bool APDS9960::disablePower() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00000001) == 0) return true;
  r &= 0b11111110;
  return setENABLE(r);
}

bool APDS9960::enableColor() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00000010) != 0) {
    _colorEnabled = true;
    return true;
  }
  r |= 0b00000010;
  bool res = setENABLE(r);
  _colorEnabled = res;
  return res;
}

bool APDS9960::disableColor() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00000010) == 0) {
    _colorEnabled = false;
    return true;
  }
  r &= 0b11111101;
  bool res = setENABLE(r);
  _colorEnabled = !res; // (res == true) if successfully disabled
  return res;
}

bool APDS9960::enableProximity() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00000100) != 0) {
    _proximityEnabled = true;
    return true;
  }
  r |= 0b00000100;
  bool res = setENABLE(r);
  _proximityEnabled = res;
  return res;
}

bool APDS9960::disableProximity() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00000100) == 0) {
    _proximityEnabled = false;
    return true;
  }
  r &= 0b11111011;
  bool res = setENABLE(r);
  _proximityEnabled = !res; // (res == true) if successfully disabled
  return res;
}

bool APDS9960::enableWait() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00001000) != 0) return true;
  r |= 0b00001000;
  return setENABLE(r);
}

bool APDS9960::disableWait() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b00001000) == 0) return true;
  r &= 0b11110111;
  return setENABLE(r);
}

bool APDS9960::enableGesture() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b01000000) != 0) {
    _gestureEnabled = true;
    return true;
  }
  r |= 0b01000000;
  bool res = setENABLE(r);
  _gestureEnabled = res;
  return res;
}

bool APDS9960::disableGesture() {
  uint8_t r;
  if (!getENABLE(&r)) return false;
  if ((r & 0b01000000) == 0) {
    _gestureEnabled = false;
    return true;
  }
  r &= 0b10111111;
  bool res = setENABLE(r);
  _gestureEnabled = !res; // (res == true) if successfully disabled
  return res;
}

#define APDS9960_ADDR 0x39

bool APDS9960::write(uint8_t val) {
  _wire.beginTransmission(APDS9960_ADDR);
  _wire.write(val);
  return _wire.endTransmission() == 0;
}

bool APDS9960::write(uint8_t reg, uint8_t val) {
  _wire.beginTransmission(APDS9960_ADDR);
  _wire.write(reg);
  _wire.write(val);
  return _wire.endTransmission() == 0;
}

bool APDS9960::read(uint8_t reg, uint8_t *val) {
  if (!write(reg)) return false;
  _wire.requestFrom(APDS9960_ADDR, 1);
  if (!_wire.available()) return false;
  *val = _wire.read();
  return true;
}

size_t APDS9960::readBlock(uint8_t reg, uint8_t *val, unsigned int len) {
    size_t i = 0;
    if (!write(reg)) return 0;
    _wire.requestFrom((uint8_t)APDS9960_ADDR, len);
    while (_wire.available()) {
      if (i == len) return 0;
      val[i++] = _wire.read();
    }
    return i;
}

int APDS9960::gestureFIFOAvailable() {
  uint8_t r;
  if (!getGSTATUS(&r)) return -1;
  if ((r & 0x01) == 0x00) return -2;
  if (!getGFLVL(&r)) return -3;
  return r;
}

int APDS9960::handleGesture() {
  const int gestureThreshold = 30;
  while (true) {
    int available = gestureFIFOAvailable();
    if (available <= 0) return 0;

    uint8_t fifo_data[128];
    uint8_t bytes_read = readGFIFO_U(fifo_data, available * 4);
    if (bytes_read == 0) return 0;

    for (int i = 0; i+3 < bytes_read; i+=4) {
      uint8_t u,d,l,r;
      u = fifo_data[i];
      d = fifo_data[i+1];
      l = fifo_data[i+2];
      r = fifo_data[i+3];
      // Serial.print(u);
      // Serial.print(",");
      // Serial.print(d);
      // Serial.print(",");
      // Serial.print(l);
      // Serial.print(",");
      // Serial.println(r);

      if (u<gestureThreshold && d<gestureThreshold && l<gestureThreshold && r<gestureThreshold) {
        _gestureIn = true;
        if (_gestureDirInX != 0 || _gestureDirInY != 0) {
          int totalX = _gestureDirInX - _gestureDirectionX;
          int totalY = _gestureDirInY - _gestureDirectionY;
          // Serial.print("OUT ");
          // Serial.print(totalX);
          // Serial.print(",");
          // Serial.println(totalY);
          if (totalX < -_gestureSensitivity) { _detectedGesture = GESTURE_LEFT; }
          if (totalX > _gestureSensitivity) { _detectedGesture = GESTURE_RIGHT; }
          if (totalY < -_gestureSensitivity) { _detectedGesture = GESTURE_DOWN; }
          if (totalY > _gestureSensitivity) { _detectedGesture = GESTURE_UP; }
          _gestureDirectionX = 0;
          _gestureDirectionY = 0;
          _gestureDirInX = 0;
          _gestureDirInY = 0;
        }
        continue;
      }

      _gestureDirectionX = r - l;
      _gestureDirectionY = u - d;
      if (_gestureIn) {
        _gestureIn = false;
        _gestureDirInX = _gestureDirectionX;
        _gestureDirInY = _gestureDirectionY;
        // Serial.print("IN ");
        // Serial.print(_gestureDirInX);
        // Serial.print(",");
        // Serial.print(_gestureDirInY);
        // Serial.print(" ");
      }
    }
  }
}

int APDS9960::gestureAvailable() {
  if (!_gestureEnabled) enableGesture();

  if (_intPin > -1) {
    if (digitalRead(_intPin) != LOW) {
      return 0;
    }
  } else if (gestureFIFOAvailable() <= 0) {
    return 0;
  }

  handleGesture();
  if (_proximityEnabled) {
    setGestureMode(false);
  }
  return (_detectedGesture == GESTURE_NONE) ? 0 : 1;
}

int APDS9960::readGesture() {
  int gesture = _detectedGesture;

  _detectedGesture = GESTURE_NONE;

  return gesture;
}

int APDS9960::colorAvailable() {
  uint8_t r;

  enableColor();

  if (!getSTATUS(&r)) {
    return 0;
  }

  if (r & 0b00000001) {
    return 1;
  }

  return 0;
}

bool APDS9960::readColor(int& r, int& g, int& b) {
  int c;

  return readColor(r, g, b, c);
}

bool APDS9960::readColor(int& r, int& g, int& b, int& c) {
  uint16_t colors[4];

  if (!readCDATAL((uint8_t *)colors, sizeof(colors))) {
    r = -1;
    g = -1;
    b = -1;
    c = -1;

    return false;
  }

  c = colors[0];
  r = colors[1];
  g = colors[2];
  b = colors[3];

  disableColor();

  return true;
}

int APDS9960::proximityAvailable() {
  uint8_t r;

  enableProximity();

  if (!getSTATUS(&r)) {
    return 0;
  }

  if (r & 0b00000010) {
    return 1;
  }

  return 0;
}

int APDS9960::readProximity() {
  uint8_t r;

  if (!getPDATA(&r)) {
    return -1;
  }

  disableProximity();

  return (255 - r);
}

#if defined(APDS9960_INT_PIN)
APDS9960 APDS(APDS9960_WIRE_INSTANCE, APDS9960_INT_PIN);
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
APDS9960 APDS(Wire1, PIN_INT_APDS);
#else
APDS9960 APDS(Wire, -1);
#endif
