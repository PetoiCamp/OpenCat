// validate the bluetooth module by sending AT commands and reading "+OK"
// the basic test is setting the bluetooth pin to "0000"
// use: sendAT(String atCMD, String parameter, int ledIndex) for other commands
// the ledIndex will set the corresponding RGB LED. green indicates that the command gets a "+OK" response.

// validation: plug in the bluetooth module and the LED should blink green

#define BLUE_NAME "BITTLE"
#define BLUE_PIN "0000"
#define BLUE_CURRENT_BAUD 115200
#define BLUE_BAUD "8" //115200

#define NYBOARD
//#define PROMINI

#ifdef NYBOARD
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 10
#define NUMPIXELS 7
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#elif defined PROMINI
#define RED_LED 3
#define GREEN_LED 5
#define BLUE_LED 6
#endif
//4:9600
//5:19200
//6:38400
//7:57600
//8:115200
//9:128000

#define BUZZER 5
#define LIT_ON 30
void beep(int note, float duration = 10, int pause = 0, byte repeat = 1 ) {
  if (note == 0) {
    analogWrite(BUZZER, 0);
    delay(duration);
    return;
  }
  int freq = 220 * pow(1.059463, note - 1); // 220 is note A3
  //1.059463 comes from https://en.wikipedia.org/wiki/Twelfth_root_of_two
  float period = 1000000.0 / freq / 2.0;
  for (byte r = 0; r < repeat; r++) {
    for (float t = 0; t < duration * 1000; t += period * 2) {
      analogWrite(BUZZER, 150);      // Almost any value can be used except 0 and 255. it can be tuned as the amplitude of the note
      // experiment to get the best tone
      delayMicroseconds(period);          // wait for a delayms ms
      analogWrite(BUZZER, 0);       // 0 turns it off
      delayMicroseconds(period);          // wait for a delayms ms
    }
    delay(pause);
  }
}
void blinkColor(int led_pin) {
  analogWrite(led_pin, LIT_ON);
  delay(50);
  analogWrite(led_pin, 0);
  //delay(2);
}
void sendAT(String atCMD, String parameter, int ledIndex) {
  Serial.println(String("AT+") + atCMD + parameter);
  delay(2);
  String in = Serial.readString();
  if (in.substring(0, 3) == "+OK") {
#ifdef NYBOARD
    beep(10, 3);
    pixels.setPixelColor(ledIndex, pixels.Color( 0, LIT_ON, 0));
#elif defined PROMINI
    blinkColor(GREEN_LED);
#endif
  }
  else {
#ifdef NYBOARD
    beep(14, 10, 10, in.length());
    pixels.setPixelColor(ledIndex, pixels.Color(LIT_ON, 0, 0));
#elif defined PROMINI
    blinkColor(RED_LED);
#endif
  }
#ifdef NYBOARD
  pixels.show();   // Send the updated pixel colors to the hardware.
#endif
}

void setup() {
#ifdef NYBOARD
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  pixels.setPixelColor(0, pixels.Color(0, 0, LIT_ON));
  pixels.show();   // Send the updated pixel colors to the hardware.
  pinMode(BUZZER, OUTPUT);
  beep(4);
#elif defined PROMINI
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  blinkColor(BLUE_LED);
#endif
  Serial.begin(BLUE_CURRENT_BAUD);
  randomSeed(analogRead(A7));//use the fluctuation of voltage caused by servos as entropy pool

}

void loop() {
  // put your main code here, to run repeatedly:
#ifdef NYBOARD
  pixels.setPixelColor(1, pixels.Color(0, 0, 0));
  //pixels.setPixelColor(2, pixels.Color(0, 0, 0));
  //pixels.setPixelColor(3, pixels.Color(0, 0, 0));
  pixels.show();   // Send the updated pixel colors to the hardware.
#endif
  while (Serial.available() && Serial.read());//flush serial buffer
  sendAT("PIN", String(BLUE_PIN), 1);
  //sendAT("NAME", String(BLUE_NAME) + String(random(10000)), 2);
  //sendAT("BAUD", BLUE_BAUD, 3);
}
