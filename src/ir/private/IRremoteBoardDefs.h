/**
 * @file IRremoteBoardDefs.h
 *
 * @brief All board specific information should be contained in this file.
 * It defines a number of macros, depending on the board, as determined by
 * pre-proccesor symbols.
 * It was previously contained within IRremoteInt.h.
 */
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
// Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// Whynter A/C ARC-110WD added by Francesco Meschia
// Sparkfun Pro Micro support by Alastair McCormack
//******************************************************************************
#ifndef IRremoteBoardDefs_h
#define IRremoteBoardDefs_h

// Define some defaults, that some boards may like to override
// (This is to avoid negative logic, ! DONT_... is just awkward.)

/**
 * Defined if the standard enableIRIn function should be used.
 * Undefine for boards supplying their own.
 */
#define USE_DEFAULT_ENABLE_IR_IN

/**
 * Define if the current board supports sending.
 * Currently not used.
 */
#define SENDING_SUPPORTED

/**
 * Defined if the standard enableIROut function should be used.
 * Undefine for boards supplying their own.
 */
#define USE_DEFAULT_ENABLE_IR_OUT

/**
 * Duty cycle in percent for sent signals.
 */
#if ! defined(DUTY_CYCLE)
#define DUTY_CYCLE 30 // 30 saves power and is compatible to the old existing code
#endif

/**
 * If USE_SOFT_CARRIER or USE_NO_CARRIER, this amount (in micro seconds) is subtracted from the
 * on-time of the pulses.
 */
#define PULSE_CORRECTION 3

//------------------------------------------------------------------------------
// This first #ifdef statement contains defines for blinking the LED,
// as well as all other board specific information, with the exception of
// timers and the sending pin (IR_SEND_PIN).

#ifdef DOXYGEN
/**
 * If defined, denotes pin number of LED that should be blinked during IR reception.
 * Leave undefined to disable blinking.
 */
#define BLINKLED        LED_BUILTIN

/**
 * Board dependent macro to turn BLINKLED on.
 */
#define BLINKLED_ON()   digitalWrite(BLINKLED, HIGH)

/**
 * Board dependent macro to turn BLINKLED off.
 */
#define BLINKLED_OFF()  digitalWrite(BLINKLED, LOW)

/**
 * Define to use no carrier PWM, just simulate a receiver signal.
 */
#define USE_NO_CARRIER

/**
 * Define to use carrier generation in software, instead of hardware PWM.
 */
#define USE_SOFT_CARRIER

/**
 * Define to use spin wait instead of delayMicros() for USE_SOFT_CARRIER.
 */
#define USE_SPIN_WAIT

#elif ! defined(ARDUINO)
// Assume that we compile a test version, to be executed on the host, not on a board.

// Do not define anything.

#elif defined(CORE_LED0_PIN)
#define BLINKLED        CORE_LED0_PIN
#define BLINKLED_ON()   (digitalWrite(CORE_LED0_PIN, HIGH))
#define BLINKLED_OFF()  (digitalWrite(CORE_LED0_PIN, LOW))

// Arduino Uno, Nano etc (previously default clause)
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega32U4__)
#define BLINKLED        LED_BUILTIN
#define BLINKLED_ON()  (PORTB |= B00100000)
#define BLINKLED_OFF()  (PORTB &= B11011111)

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BLINKLED        13
#define BLINKLED_ON()   (PORTB |= B10000000)
#define BLINKLED_OFF()  (PORTB &= B01111111)

#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define BLINKLED        0
#define BLINKLED_ON()   (PORTD |= B00000001)
#define BLINKLED_OFF()  (PORTD &= B11111110)

// Nano Every, Uno WiFi Rev2
#elif defined(__AVR_ATmega4809__)
#define BLINKLED        LED_BUILTIN
#define BLINKLED_ON()   (digitalWrite(BLINKLED, HIGH))
#define BLINKLED_OFF()  (digitalWrite(BLINKLED, LOW))

#elif defined(ARDUINO_ARCH_SAMD)
#define BLINKLED        LED_BUILTIN
#define BLINKLED_ON()   (digitalWrite(LED_BUILTIN, HIGH))
#define BLINKLED_OFF()  (digitalWrite(LED_BUILTIN, LOW))

#define USE_SOFT_CARRIER
// Define to use spin wait instead of delayMicros()
//#define USE_SPIN_WAIT
// Supply own enableIRIn()
#undef USE_DEFAULT_ENABLE_IR_IN

#elif defined(ESP32)
// No system LED on ESP32, disable blinking by NOT defining BLINKLED

// Supply own enableIRIn() and enableIROut()
#undef USE_DEFAULT_ENABLE_IR_IN
#undef USE_DEFAULT_ENABLE_IR_OUT

#else
#warning No blinking definition found. Check IRremoteBoardDefs.h.
#ifdef LED_BUILTIN
#define BLINKLED        LED_BUILTIN
#define BLINKLED_ON()   digitalWrite(BLINKLED, HIGH)
#define BLINKLED_OFF()  digitalWrite(BLINKLED, LOW)
#endif
#endif

//------------------------------------------------------------------------------
// microseconds per clock interrupt tick
#define MICROS_PER_TICK    50

//------------------------------------------------------------------------------
// Define which timer to use
//
// Uncomment the timer you wish to use on your board.
// If you are using another library which uses timer2, you have options to
//   switch IRremote to use a different timer.
//

#ifndef ARDUINO
// Assume that we compile a test version, to be executed on the host,
// not on a board.

// Do not define any timer.

/*********************
 * ARDUINO Boards
 *********************/
// Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, Nano, etc
// ATmega48, ATmega88, ATmega168, ATmega328
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) // old default clause
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER2)
//#define IR_USE_TIMER1   // tx = pin 9
#define IR_USE_TIMER2     // tx = pin 3
#  endif

// Arduino Mega
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER2) && !defined(IR_USE_TIMER3) && !defined(IR_USE_TIMER4) && !defined(IR_USE_TIMER5)
//#define IR_USE_TIMER1   // tx = pin 11
#define IR_USE_TIMER2     // tx = pin 9
//#define IR_USE_TIMER3   // tx = pin 5
//#define IR_USE_TIMER4   // tx = pin 6
//#define IR_USE_TIMER5   // tx = pin 46
#  endif

// Leonardo
#elif defined(__AVR_ATmega32U4__) && ! defined(TEENSYDUINO) && ! defined(ARDUINO_AVR_PROMICRO)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER3) && !defined(IR_USE_TIMER4_HS)
//#define IR_USE_TIMER1     // tx = pin 9
#define IR_USE_TIMER3       // tx = pin 5
//#define IR_USE_TIMER4_HS  // tx = pin 13
#  endif

// Nano Every, Uno WiFi Rev2
#elif defined(__AVR_ATmega4809__)
#  if !defined(IR_USE_TIMER_4809_1) && !defined(IR_USE_TIMER_4809_2)
#define IR_USE_TIMER_4809_1     //  tx = pin 24
//#define IR_USE_TIMER_4809_2     // TODO tx = pin 21
#  endif

/*********************
 * Plain AVR CPU's
 *********************/
// ATmega8u2, ATmega16U2, ATmega32U2
#elif defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__)  || defined(__AVR_ATmega32U2__)
#  if !defined(IR_USE_TIMER1)
    #define IR_USE_TIMER1     // tx = pin C6
#endif

// Atmega8
#elif defined(__AVR_ATmega8__)
#  if !defined(IR_USE_TIMER1)
#define IR_USE_TIMER1     // tx = pin 9
#  endif


// ATtiny84
#elif defined(__AVR_ATtiny84__)
#  if !defined(IR_USE_TIMER1)
#define IR_USE_TIMER1     // tx = pin 6
#  endif

//ATtiny85
#elif defined(__AVR_ATtiny85__)
#  if !defined(IR_USE_TIMER_TINY0)
#define IR_USE_TIMER_TINY0   // tx = pin 1
#  endif

/*********************
 * SPARKFUN Boards
 *********************/
// Sparkfun Pro Micro
#elif defined(ARDUINO_AVR_PROMICRO)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER3) && !defined(IR_USE_TIMER4_HS)
//#define IR_USE_TIMER1     // tx = pin 9
#define IR_USE_TIMER3       // tx = pin 5
//#define IR_USE_TIMER4_HS  // tx = pin 13
#  endif

/*********************
 * TEENSY Boards
 *********************/
// Teensy 1.0
#elif defined(__AVR_AT90USB162__)
#  if !defined(IR_USE_TIMER1)
#define IR_USE_TIMER1     // tx = pin 17
#  endif

// Teensy 2.0
#elif defined(__AVR_ATmega32U4__) && defined(TEENSYDUINO)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER3) && !defined(IR_USE_TIMER4_HS)
//#define IR_USE_TIMER1     // tx = pin 14 (Teensy 2.0 - physical pin: B5)
#define IR_USE_TIMER3       // tx = pin 9  (Teensy 2.0 - physical pin: C6)
//#define IR_USE_TIMER4_HS  // tx = pin 10 (Teensy 2.0 - physical pin: C7)
#  endif

// Teensy 3.0 / Teensy 3.1
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#  if !defined(IR_USE_TIMER_CMT)
#define IR_USE_TIMER_CMT    // tx = pin 5
#  endif

// Teensy-LC
#elif defined(__MKL26Z64__)
#  if !defined(IR_USE_TIMER_TPM1)
#define IR_USE_TIMER_TPM1 // tx = pin 16
#  endif

// Teensy++ 1.0 & 2.0
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER2) && !defined(IR_USE_TIMER3)
//#define IR_USE_TIMER1   // tx = pin 25
#define IR_USE_TIMER2     // tx = pin 1
//#define IR_USE_TIMER3   // tx = pin 16
#  endif

/*********************
 * CPU's with MegaCore
 *********************/
// MegaCore - ATmega64, ATmega128
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__)
#  if !defined(IR_USE_TIMER1)
 #define IR_USE_TIMER1     // tx = pin 13
#  endif

/*********************
 * CPU's with MajorCore
 *********************/
#elif defined(__AVR_ATmega8515__) || defined(__AVR_ATmega162__)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER3)
    #define IR_USE_TIMER1     // tx = pin 13
    //#define IR_USE_TIMER3   // tx = pin 12 - ATmega162 only
#endif

/*********************
 * CPU's with MightyCore
 *********************/
// MightyCore - ATmega1284
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER2) && !defined(IR_USE_TIMER3)
//#define IR_USE_TIMER1   // tx = pin 13
#define IR_USE_TIMER2     // tx = pin 14
//#define IR_USE_TIMER3   // tx = pin 6
#  endif

// MightyCore - ATmega164, ATmega324, ATmega644
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) \
|| defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) \
|| defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega164A__) \
|| defined(__AVR_ATmega164P__)
#  if !defined(IR_USE_TIMER1) && !defined(IR_USE_TIMER2)
//#define IR_USE_TIMER1   // tx = pin 13
#define IR_USE_TIMER2     // tx = pin 14
#  endif

// MightyCore - ATmega8535, ATmega16, ATmega32
#elif defined(__AVR_ATmega8535__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
#  if !defined(IR_USE_TIMER1)
#define IR_USE_TIMER1     // tx = pin 13
#  endif

/*********************
 * OTHER CPU's
 *********************/
#elif defined(ESP32)
#  if !defined(IR_TIMER_USE_ESP32)
#define IR_TIMER_USE_ESP32
#  endif

#elif defined(ARDUINO_ARCH_SAMD)
#define TIMER_PRESCALER_DIV 64

#else
// Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, Nano, etc
// ATmega48, ATmega88, ATmega168, ATmega328
//#define IR_USE_TIMER1   // tx = pin 9
#error Board could not be identified from pre-processor symbols. Please extend IRremoteBoardDefs.h.
#endif

// Provide default definitions, portable but possibly slower than necessary.
// digitalWrite is supposed to be slow. If this is an issue, define faster,
// board-dependent versions of these macros SENDPIN_ON(pin) and SENDPIN_OFF(pin).
// Portable, possibly slow, default definitions are given at the end of this file.
// If defining new versions, feel free to ignore the pin argument if it
// is not configurable on the current board.

#ifndef SENDPIN_ON
/** Board dependent macro to turn on the pin given as argument. */
#define SENDPIN_ON(pin)  digitalWrite(pin, HIGH)
#endif

#ifndef SENDPIN_OFF
/**
 * Board dependent macro to turn off the pin given as argument.
 */
#define SENDPIN_OFF(pin) digitalWrite(pin, LOW)
#endif

//------------------------------------------------------------------------------
// CPU Frequency
//
#if !defined(SYSCLOCK) && defined(ARDUINO) // allow for processor specific code to define SYSCLOCK
#ifndef F_CPU
#error SYSCLOCK cannot be determined. Define it for your board in IRremoteBoardDefs.h.
#endif // ! F_CPU
/**
 * Clock frequency to be used for timing.
 */
#define SYSCLOCK F_CPU // main Arduino clock
#endif // ! SYSCLOCK

//------------------------------------------------------------------------------
// Defines for Timer

//---------------------------------------------------------
#ifdef DOXYGEN
/**
 * If applicable, pin number for sending IR. Note that in most cases, this is not
 * used and ignored if set. Instead, the sending pin is determined by the timer
 * deployed.
 */
#define IR_SEND_PIN

/**
 * Interrupt service routine. Called as interrupt routine to collect read IR data.
 */
#define  ISR

#elif ! defined(ARDUINO)
// Assume that we compile a test version, to be executed on the host,
// not on a board.
// Do nothing.
#ifdef ISR
#undef ISR
#endif
#define  ISR(f)  void do_not_use__(void)
#define TIMER_RESET

//---------------------------------------------------------
// Timer2 (8 bits)
//
#elif defined(IR_USE_TIMER2)

#define TIMER_RESET
#define TIMER_ENABLE_PWM    (TCCR2A |= _BV(COM2B1))
#define TIMER_DISABLE_PWM   (TCCR2A &= ~(_BV(COM2B1)))
#define TIMER_ENABLE_INTR   (TIMSK2 = _BV(OCIE2A))
#define TIMER_DISABLE_INTR  (TIMSK2 = 0)
#define TIMER_INTR_NAME     TIMER2_COMPA_vect
// COM2A = 00: disconnect OC2A
// COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
// WGM2 = 101: phase-correct PWM with OCRA as top
// CS2  = 000: no prescaling
// The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
#pragma GCC diagnostic ignored "-Wunused-function"
static void timerConfigkHz(uint16_t frequency) {
    const uint16_t pwmval = (SYSCLOCK / 2000) / (frequency);
    TCCR2A = _BV(WGM20);
    TCCR2B = _BV(WGM22) | _BV(CS20);
    OCR2A = pwmval;
    OCR2B = pwmval * DUTY_CYCLE / 100;
}

#define TIMER_COUNT_TOP  (SYSCLOCK * MICROS_PER_TICK / 1000000)
static void timerConfigNormal() {
#if (TIMER_COUNT_TOP < 256)
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS20);
    OCR2A  = TIMER_COUNT_TOP;
    TCNT2  = 0;
#else
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS21);
    OCR2A = TIMER_COUNT_TOP / 8;
    TCNT2 = 0;
#endif
}

//-----------------
#if defined(CORE_OC2B_PIN)
#define IR_SEND_PIN  CORE_OC2B_PIN  // Teensy

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define IR_SEND_PIN  9              // Arduino Mega

#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) \
|| defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) \
|| defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) \
|| defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega164A__) \
|| defined(__AVR_ATmega164P__)
#define IR_SEND_PIN  14             // MightyCore, MegaCore

#else
#define IR_SEND_PIN  3              // Arduino Duemilanove, Diecimila, LilyPad, etc
#endif // defined(CORE_OC2B_PIN)

//---------------------------------------------------------
// Timer1 (16 bits)
//
#elif defined(IR_USE_TIMER1)

#define TIMER_RESET
#define TIMER_ENABLE_PWM   (TCCR1A |= _BV(COM1A1))
#define TIMER_DISABLE_PWM  (TCCR1A &= ~(_BV(COM1A1)))

//-----------------
#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega8515__) \
|| defined(__AVR_ATmega8535__) || defined(__AVR_ATmega16__) \
|| defined(__AVR_ATmega32__) || defined(__AVR_ATmega64__) \
|| defined(__AVR_ATmega128__) || defined(__AVR_ATmega162__)
#define TIMER_ENABLE_INTR   (TIMSK |= _BV(OCIE1A))
#define TIMER_DISABLE_INTR  (TIMSK &= ~_BV(OCIE1A))
#else
#define TIMER_ENABLE_INTR   (TIMSK1 = _BV(OCIE1A))
#define TIMER_DISABLE_INTR  (TIMSK1 = 0)
#endif

//-----------------
#define TIMER_INTR_NAME       TIMER1_COMPA_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint32_t pwmval = SYSCLOCK / 2000 / (frequency);
    TCCR1A = _BV(WGM11);
    TCCR1B = _BV(WGM13) | _BV(CS10);
    ICR1 = pwmval;
    OCR1A = pwmval * DUTY_CYCLE / 100;
}

static void timerConfigNormal() {
    TCCR1A = 0;
    TCCR1B = _BV(WGM12) | _BV(CS10);
    OCR1A = SYSCLOCK * MICROS_PER_TICK / 1000000;
    TCNT1 = 0;
}

//-----------------
#if defined(CORE_OC1A_PIN)
#define IR_SEND_PIN  CORE_OC1A_PIN  // Teensy

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define IR_SEND_PIN  11             // Arduino Mega

#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) \
|| defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) \
|| defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) \
|| defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega164A__) \
|| defined(__AVR_ATmega164P__) || defined(__AVR_ATmega32__) \
|| defined(__AVR_ATmega16__) || defined(__AVR_ATmega8535__) \
|| defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) \
|| defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__) \
|| defined(__AVR_ATmega8515__) || defined(__AVR_ATmega162__)
#define IR_SEND_PIN  13             // MightyCore, MegaCore, MajorCore

#elif defined(__AVR_ATtiny84__)
# define IR_SEND_PIN  6

#else
#define IR_SEND_PIN  9              // Arduino Duemilanove, Diecimila, LilyPad, Sparkfun Pro Micro, Leonardo etc.
#endif // defined(CORE_OC1A_PIN)

//---------------------------------------------------------
// Timer3 (16 bits)
//
#elif defined(IR_USE_TIMER3)

#define TIMER_RESET
#define TIMER_ENABLE_PWM     (TCCR3A |= _BV(COM3A1))
#define TIMER_DISABLE_PWM    (TCCR3A &= ~(_BV(COM3A1)))
#define TIMER_ENABLE_INTR    (TIMSK3 = _BV(OCIE3A))
#define TIMER_DISABLE_INTR   (TIMSK3 = 0)
#define TIMER_INTR_NAME      TIMER3_COMPA_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint32_t pwmval = SYSCLOCK / 2000 / (frequency);
    TCCR3A = _BV(WGM31);
    TCCR3B = _BV(WGM33) | _BV(CS30);
    ICR3 = pwmval;
    OCR3A = pwmval * DUTY_CYCLE / 100;
}

static void timerConfigNormal() {
    TCCR3A = 0;
    TCCR3B = _BV(WGM32) | _BV(CS30);
    OCR3A = SYSCLOCK * MICROS_PER_TICK / 1000000;
    TCNT3 = 0;
}

//-----------------
#if defined(CORE_OC3A_PIN)
#define IR_SEND_PIN  CORE_OC3A_PIN  // Teensy

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) \
|| defined(__AVR_ATmega32U4__) || defined(ARDUINO_AVR_PROMICRO)
#define IR_SEND_PIN  5              // Arduino Mega, Arduino Leonardo, Sparkfun Pro Micro

#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
#define IR_SEND_PIN  6              // MightyCore, MegaCore

#else
#error "Please add OC3A pin number here\n"
#endif

//---------------------------------------------------------
// Timer4 (10 bits, high speed option)
//
#elif defined(IR_USE_TIMER4_HS)

#define TIMER_RESET
#if defined(ARDUINO_AVR_PROMICRO) // Sparkfun Pro Micro
#define TIMER_ENABLE_PWM    (TCCR4A |= _BV(COM4A0))     // Use complimentary O̅C̅4̅A̅ output on pin 5
#define TIMER_DISABLE_PWM   (TCCR4A &= ~(_BV(COM4A0)))  // (Pro Micro does not map PC7 (32/ICP3/CLK0/OC4A)
                                                            // of ATmega32U4 )
#else
#define TIMER_ENABLE_PWM    (TCCR4A |= _BV(COM4A1))
#define TIMER_DISABLE_PWM   (TCCR4A &= ~(_BV(COM4A1)))
#endif
#define TIMER_ENABLE_INTR   (TIMSK4 = _BV(TOIE4))
#define TIMER_DISABLE_INTR  (TIMSK4 = 0)
#define TIMER_INTR_NAME     TIMER4_OVF_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint32_t pwmval = SYSCLOCK / 2000 / (frequency);
    TCCR4A = (1 << PWM4A);
    TCCR4B = _BV(CS40);
    TCCR4C = 0;
    TCCR4D = (1 << WGM40);
    TCCR4E = 0;
    TC4H = pwmval >> 8;
    OCR4C = pwmval;
    TC4H = (pwmval * DUTY_CYCLE / 100) >> 8;
    OCR4A = (pwmval * DUTY_CYCLE / 100) & 255;
}

static void timerConfigNormal() {
    TCCR4A = 0;
    TCCR4B = _BV(CS40);
    TCCR4C = 0;
    TCCR4D = 0;
    TCCR4E = 0;
    TC4H = (SYSCLOCK * MICROS_PER_TICK / 1000000) >> 8;
    OCR4C = (SYSCLOCK * MICROS_PER_TICK / 1000000) & 255;
    TC4H = 0;
    TCNT4 = 0;
}

//-----------------
#if defined(CORE_OC4A_PIN)
#define IR_SEND_PIN  CORE_OC4A_PIN  // Teensy
#elif defined(ARDUINO_AVR_PROMICRO)
#   define IR_SEND_PIN  5              // Sparkfun Pro Micro
#elif defined(__AVR_ATmega32U4__)
#define IR_SEND_PIN  13             // Leonardo
#else
#error "Please add OC4A pin number here\n"
#endif

//---------------------------------------------------------
// Timer4 (16 bits)
//
#elif defined(IR_USE_TIMER4)

#define TIMER_RESET
#define TIMER_ENABLE_PWM    (TCCR4A |= _BV(COM4A1))
#define TIMER_DISABLE_PWM   (TCCR4A &= ~(_BV(COM4A1)))
#define TIMER_ENABLE_INTR   (TIMSK4 = _BV(OCIE4A))
#define TIMER_DISABLE_INTR  (TIMSK4 = 0)
#define TIMER_INTR_NAME     TIMER4_COMPA_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint32_t pwmval = SYSCLOCK / 2000 / (frequency);
    TCCR4A = _BV(WGM41);
    TCCR4B = _BV(WGM43) | _BV(CS40);
    ICR4 = pwmval;
    OCR4A = pwmval * DUTY_CYCLE / 100;
}

static void timerConfigNormal() {
    TCCR4A = 0;
    TCCR4B = _BV(WGM42) | _BV(CS40);
    OCR4A = SYSCLOCK * MICROS_PER_TICK / 1000000;
    TCNT4 = 0;
}

//-----------------
#if defined(CORE_OC4A_PIN)
#define IR_SEND_PIN  CORE_OC4A_PIN
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define IR_SEND_PIN  6  // Arduino Mega
#else
#error "Please add OC4A pin number here\n"
#endif

//---------------------------------------------------------
// Timer5 (16 bits)
//
#elif defined(IR_USE_TIMER5)

#define TIMER_RESET
#define TIMER_ENABLE_PWM    (TCCR5A |= _BV(COM5A1))
#define TIMER_DISABLE_PWM   (TCCR5A &= ~(_BV(COM5A1)))
#define TIMER_ENABLE_INTR   (TIMSK5 = _BV(OCIE5A))
#define TIMER_DISABLE_INTR  (TIMSK5 = 0)
#define TIMER_INTR_NAME     TIMER5_COMPA_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint32_t pwmval = SYSCLOCK / 2000 / (frequency);
    TCCR5A = _BV(WGM51);
    TCCR5B = _BV(WGM53) | _BV(CS50);
    ICR5 = pwmval;
    OCR5A = pwmval * DUTY_CYCLE / 100;
}

static void timerConfigNormal() {
    TCCR5A = 0;
    TCCR5B = _BV(WGM52) | _BV(CS50);
    OCR5A = SYSCLOCK * MICROS_PER_TICK / 1000000;
    TCNT5 = 0;
}

//-----------------
#if defined(CORE_OC5A_PIN)
#define IR_SEND_PIN  CORE_OC5A_PIN
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define IR_SEND_PIN  46  // Arduino Mega
#else
#error "Please add OC5A pin number here\n"
#endif

//---------------------------------------------------------
// Special carrier modulator timer for Teensy 3.0 / Teensy 3.1
//
#elif defined(IR_USE_TIMER_CMT)

#define TIMER_RESET ({     \
uint8_t tmp __attribute__((unused)) = CMT_MSC; \
CMT_CMD2 = 30;         \
})

#define TIMER_ENABLE_PWM  do {                                         \
CORE_PIN5_CONFIG = PORT_PCR_MUX(2) | PORT_PCR_DSE | PORT_PCR_SRE;  \
} while(0)

#define TIMER_DISABLE_PWM  do {                                        \
CORE_PIN5_CONFIG = PORT_PCR_MUX(1) | PORT_PCR_DSE | PORT_PCR_SRE;  \
} while(0)

#define TIMER_ENABLE_INTR   NVIC_ENABLE_IRQ(IRQ_CMT)
#define TIMER_DISABLE_INTR  NVIC_DISABLE_IRQ(IRQ_CMT)
#define TIMER_INTR_NAME     cmt_isr

//-----------------
#ifdef ISR
#undef ISR
#endif
#define  ISR(f)  void f(void)

//-----------------
#define CMT_PPS_DIV  ((F_BUS + 7999999) / 8000000)
#if F_BUS < 8000000
#error IRremote requires at least 8 MHz on Teensy 3.x
#endif

static void timerConfigkHz(uint16_t frequency) {
    SIM_SCGC4 |= SIM_SCGC4_CMT;
    SIM_SOPT2 |= SIM_SOPT2_PTD7PAD;
    CMT_PPS = CMT_PPS_DIV - 1;
    CMT_CGH1 = ((F_BUS / CMT_PPS_DIV / 3000) + ((frequency) / 2)) / (frequency);
    CMT_CGL1 = ((F_BUS / CMT_PPS_DIV / 1500) + ((frequency) / 2)) / (frequency);
    CMT_CMD1 = 0;
    CMT_CMD2 = 30;
    CMT_CMD3 = 0;
    CMT_CMD4 = 0;
    CMT_OC = 0x60;
    CMT_MSC = 0x01;
}

static void timerConfigNormal() {
    SIM_SCGC4 |= SIM_SCGC4_CMT;
    CMT_PPS = CMT_PPS_DIV - 1;
    CMT_CGH1 = 1;
    CMT_CGL1 = 1;
    CMT_CMD1 = 0;
    CMT_CMD2 = 30;
    CMT_CMD3 = 0;
    CMT_CMD4 = (F_BUS / 160000 + CMT_PPS_DIV / 2) / CMT_PPS_DIV - 31;
    CMT_OC = 0;
    CMT_MSC = 0x03;
}

#define IR_SEND_PIN  5

// defines for TPM1 timer on Teensy-LC
#elif defined(IR_USE_TIMER_TPM1)
#define TIMER_RESET          FTM1_SC |= FTM_SC_TOF;
#define TIMER_ENABLE_PWM     CORE_PIN16_CONFIG = PORT_PCR_MUX(3)|PORT_PCR_DSE|PORT_PCR_SRE
#define TIMER_DISABLE_PWM    CORE_PIN16_CONFIG = PORT_PCR_MUX(1)|PORT_PCR_SRE
#define TIMER_ENABLE_INTR    NVIC_ENABLE_IRQ(IRQ_FTM1)
#define TIMER_DISABLE_INTR   NVIC_DISABLE_IRQ(IRQ_FTM1)
#define TIMER_INTR_NAME      ftm1_isr
#ifdef ISR
#undef ISR
#endif
#define ISR(f) void f(void)

static void timerConfigkHz(uint16_t frequency) {
    SIM_SCGC6 |= SIM_SCGC6_TPM1;
    FTM1_SC = 0;
    FTM1_CNT = 0;
    FTM1_MOD = (F_PLL / 2000) / frequency - 1;
    FTM1_C0V = (F_PLL / 6000) / frequency - 1;
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);
}

static void timerConfigNormal() {
    SIM_SCGC6 |= SIM_SCGC6_TPM1;
    FTM1_SC = 0;
    FTM1_CNT = 0;
    FTM1_MOD = (F_PLL / 40000) - 1;
    FTM1_C0V = 0;
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0) | FTM_SC_TOF | FTM_SC_TOIE;
}
#define IR_SEND_PIN        16

// defines for timer_tiny0 (8 bits)
#elif defined(IR_USE_TIMER_TINY0)
#define TIMER_RESET
#define TIMER_ENABLE_PWM     (TCCR0A |= _BV(COM0B1))
#define TIMER_DISABLE_PWM    (TCCR0A &= ~(_BV(COM0B1)))
#define TIMER_ENABLE_INTR    (TIMSK |= _BV(OCIE0A))
#define TIMER_DISABLE_INTR   (TIMSK &= ~(_BV(OCIE0A)))
#define TIMER_INTR_NAME      TIMER0_COMPA_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint16_t pwmval = SYSCLOCK / 2000 / (frequency);
    TCCR0A = _BV(WGM00);
    TCCR0B = _BV(WGM02) | _BV(CS00);
    OCR0A = pwmval;
    OCR0B = pwmval * DUTY_CYCLE / 100;
}

#define TIMER_COUNT_TOP  (SYSCLOCK * MICROS_PER_TICK / 1000000)
static void timerConfigNormal() {
#if (TIMER_COUNT_TOP < 256)
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS00);
    OCR0A = TIMER_COUNT_TOP;
    TCNT0 = 0;
#else
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS01);
    OCR0A = TIMER_COUNT_TOP / 8;
    TCNT0 = 0;
#endif
}

#define IR_SEND_PIN        1  /* ATtiny85 */

#elif defined(IR_USE_TIMER_4809_1)
// ATmega4809 TCB0
#define TIMER_RESET          TCB0.INTFLAGS = TCB_CAPT_bm
#define TIMER_ENABLE_PWM     (TCB0.CTRLB |= TCB_CCMPEN_bm)
#define TIMER_DISABLE_PWM    (TCB0.CTRLB &= ~(TCB_CCMPEN_bm))
#define TIMER_ENABLE_INTR    (TCB0.INTCTRL = TCB_CAPT_bm)
#define TIMER_DISABLE_INTR   (TCB0.INTCTRL &= ~(TCB_CAPT_bm))
#define TIMER_INTR_NAME      TCB0_INT_vect

static void timerConfigkHz(uint16_t frequency) {
    const uint32_t pwmval = (SYSCLOCK / 2000) / (frequency);
    TCB0.CTRLB = TCB_CNTMODE_PWM8_gc;
    TCB0.CCMPL = pwmval;
    TCB0.CCMPH = (pwmval * DUTY_CYCLE) / 100;
    TCB0.CTRLA = (TCB_CLKSEL_CLKDIV2_gc) | (TCB_ENABLE_bm);
}

static void timerConfigNormal() {
    TCB0.CTRLB = (TCB_CNTMODE_INT_gc);
    TCB0.CCMP = ((SYSCLOCK * MICROS_PER_TICK) / 1000000);
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.CTRLA = (TCB_CLKSEL_CLKDIV1_gc) | (TCB_ENABLE_bm);
}

#define IR_SEND_PIN        6  /* Nano Every, Uno WiFi Rev2 */
//---------------------------------------------------------
// ESP32 (ESP8266 should likely be added here too)
//

// ESP32 has it own timer API and does not use these macros, but to avoid ifdef'ing
// them out in the common code, they are defined to no-op. This allows the code to compile
// (which it wouldn't otherwise) but irsend will not work until ESP32 specific code is written
// for that -- merlin
// As a warning, sending timing specific code from an ESP32 can be challenging if you need 100%
// reliability because the arduino code may be interrupted and cause your sent waveform to be the
// wrong length. This is specifically an issue for neopixels which require 800Khz resolution.
// IR may just work as is with the common code since it's lower frequency, but if not, the other
// way to do this on ESP32 is using the RMT built in driver like in this incomplete library below
// https://github.com/ExploreEmbedded/ESP32_RMT
#elif defined(IR_TIMER_USE_ESP32)

#if ! defined(IR_SEND_PIN)
#define IR_SEND_PIN 4 // can use any pin, no timer restrictions
#endif

#if ! defined(LEDCHANNEL)
#define LEDCHANNEL 0 // The channel used for PWM 0 to 7 are high speed PWM channels
#endif

#define TIMER_RESET
#define TIMER_ENABLE_PWM    ledcWrite(LEDCHANNEL, DUTY_CYCLE) // we must use channel here not pin number
#define TIMER_DISABLE_PWM   ledcWrite(LEDCHANNEL, 0)

#ifdef ISR
#undef ISR
#endif
#define  ISR(f)  void IRAM_ATTR IRTimer()

#elif defined(ARDUINO_ARCH_SAMD)
// use timer 3 hardcoded at this time

#define IR_SEND_PIN 9

#define TIMER_RESET
#define TIMER_ENABLE_PWM     // Not presently used
#define TIMER_DISABLE_PWM
#define TIMER_ENABLE_INTR    NVIC_EnableIRQ(TC3_IRQn) // Not presently used
#define TIMER_DISABLE_INTR   NVIC_DisableIRQ(TC3_IRQn)
#define TIMER_INTR_NAME      TC3_Handler // Not presently used
static void timerConfigkHz(uint16_t frequency) {

}

#ifdef ISR
#undef ISR
#endif
#define  ISR(f)  void irs()

//---------------------------------------------------------
// Unknown Timer
//
#else
#error "Internal code configuration error, no known IR_USE_TIMER* defined\n"
#endif

#endif // ! IRremoteBoardDefs_h
