/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  to calibrate the frequency of the oscillator clock of the PCA9685.

  CAUTION: DO NOT CONNECT ANY VOLTAGE HIGHER THAN THE BOARD LIMITS.
  For 3.3V boards, like the ESP8266, remove any 5V input. The setup will
  feed the voltage back into the board to measure the frequency.
  KABOEM, SMOKE if you use too much VOLTAGE.

  Connect the PCA9685 with I2C (Ground, VCC, SCL, SCA) and apply
  voltage on V+. See above not higher than board limits.
  Connect the signal (yellow pin, PWM) of the PCA9685 to your board:
  Default is pin 3, last of first block.
  Default is pin 14 (of your ESP8266).

  Formula for prescale to get the targetted frequency (=update_rate) is:
  prescale = round ( osc_clock / 4096 * update_rate) - 1
  rewritten: osc_clock = (prescale + 1) * 4096 * update_rate
  We will measure the real update_rate to assert the real osc_clock.

  ***************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

#if (defined(ESP8266) || defined(ESP32))

// Applied frequency in the test: can be changed to get the optimal
// oscillator calibration for your targetted frequency.
#define FREQUENCY             50

// CAUTION: ONLY CONNECT server and ESP WITHOUT 5V ON V+ or green breakout supply pins. Use 3.3V on V+
#define PIN_SERVO_FEEDBACK     3 // Connect Yellow PWM pin, 3 = last on first block
#define PIN_BOARD_FEEDBACK    14 // 14 => D5 on NodeMCU

uint8_t prescale = 0;
// loop
#define INTERVAL   1000  // 1 sec
int32_t lastEvaluation = 0;
uint16_t frozenCounter = 0;
uint16_t countDeviations = 0;

uint32_t totalCounter = 0;
uint32_t totalTime = 0;   // in millis
uint32_t realOsciFreq = 0;
uint32_t multiplier = 4096;

// interrupt
volatile uint16_t interruptCounter = 0;

ICACHE_RAM_ATTR void handleInterrupt() {
  interruptCounter++;
}

void setup() {
  Serial.begin(115200);
  Serial.println("PCA9685 Oscillator test");

  // set PCA9685
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);             // Set some frequency
  pwm.setPWM(PIN_SERVO_FEEDBACK,0,2048); // half of time high, half of time low
  prescale = pwm.readPrescale();         // read prescale
  Serial.printf("Target frequency: %u\n", FREQUENCY);
  Serial.printf("Applied prescale: %u\n", prescale);

  // prepare interrupt on ESP pin
  pinMode(PIN_BOARD_FEEDBACK, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BOARD_FEEDBACK), handleInterrupt, RISING);

  // take a breath and reset to zero
  delay(10);
  interruptCounter = 0;
  lastEvaluation = millis();
}

void loop() {
  if (millis() - lastEvaluation > INTERVAL)
  {
    // first freeze counters and adjust for new round
    frozenCounter = interruptCounter; // first freeze counter
    interruptCounter -= frozenCounter;
    lastEvaluation += INTERVAL;

    totalCounter += frozenCounter;
    totalTime += 1;

    // only print deviations from targetted frequency
    //if (frozenCounter != FREQUENCY)
    {
       multiplier = 4096;
       realOsciFreq = (prescale + 1) * totalCounter; // first part calcutlation
       // now follows an ugly hack to have maximum precision in 32 bits
       while (((realOsciFreq & 0x80000000) == 0) && (multiplier != 1))
       {
          realOsciFreq <<= 1;
          multiplier >>= 1;
       }
       realOsciFreq /= totalTime;
       if (multiplier) realOsciFreq *= multiplier;

       countDeviations++;
       Serial.printf("%4u", countDeviations);
       Serial.printf(" Timestamp: %4u ", totalTime);
       Serial.printf(" Freq: %4u ", frozenCounter);
       Serial.printf(" Counter: %6u ", totalCounter);
       Serial.printf(" calc.osci.freq: %9u\n",realOsciFreq);
    }
  }

}
#else

void setup() {
  Serial.begin(115200);
  Serial.println("PCA9685 Oscillator test");
  Serial.println("yet not available for your board."); // please help adapt the code!
}

void loop() {}

#endif // ESP8266/ESP32
