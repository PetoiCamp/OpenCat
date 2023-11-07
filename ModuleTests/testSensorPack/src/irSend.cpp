#include "IRremote.h"

#ifdef SENDING_SUPPORTED
//+=============================================================================
void IRsend::sendRaw(const unsigned int buf[], unsigned int len, unsigned int hz) {
    // Set IR carrier frequency
    enableIROut(hz);

    for (unsigned int i = 0; i < len; i++) {
        if (i & 1) {
            space(buf[i]);
        } else {
            mark(buf[i]);
        }
    }

    space(0);  // Always end with the LED off
}

void IRsend::sendRaw_P(const unsigned int buf[], unsigned int len, unsigned int hz) {
#if !defined(__AVR__)
    sendRaw(buf,len,hz); // Let the function work for non AVR platforms
#else
    // Set IR carrier frequency
    enableIROut(hz);

    for (unsigned int i = 0; i < len; i++) {
        uint16_t duration = pgm_read_word_near(buf + sizeof(uint16_t) * i);
        if (i & 1) {
            space(duration);
        } else {
            mark(duration);
        }
    }
    space(0);  // Always end with the LED off
#endif

}

#ifdef USE_SOFT_CARRIER
void inline IRsend::sleepMicros(unsigned long us) {
#ifdef USE_SPIN_WAIT
    sleepUntilMicros(micros() + us);
#else
    if (us > 0U) { // Is this necessary? (Official docu https://www.arduino.cc/en/Reference/DelayMicroseconds does not tell.)
        delayMicroseconds((unsigned int) us);
    }
#endif
}

void inline IRsend::sleepUntilMicros(unsigned long targetTime) {
#ifdef USE_SPIN_WAIT
    while (micros() < targetTime)
    ;
#else
    unsigned long now = micros();
    if (now < targetTime) {
        sleepMicros(targetTime - now);
    }
#endif
}
#endif // USE_SOFT_CARRIER

//+=============================================================================
// Sends an IR mark for the specified number of microseconds.
// The mark output is modulated at the PWM frequency.
//

void IRsend::mark(unsigned int time) {
#ifdef USE_SOFT_CARRIER
    unsigned long start = micros();
    unsigned long stop = start + time;
    if (stop + periodTime < start) {
        // Counter wrap-around, happens very seldomly, but CAN happen.
        // Just give up instead of possibly damaging the hardware.
        return;
    }
    unsigned long nextPeriodEnding = start;
    unsigned long now = micros();
    while (now < stop) {
        SENDPIN_ON(sendPin);
        sleepMicros (periodOnTime);
        SENDPIN_OFF(sendPin);
        nextPeriodEnding += periodTime;
        sleepUntilMicros(nextPeriodEnding);
        now = micros();
    }
#elif defined(USE_NO_CARRIER)
    digitalWrite(sendPin, LOW); // Set output to active low.
#else
    TIMER_ENABLE_PWM; // Enable pin 3 PWM output
#endif
    if (time > 0) {
        custom_delay_usec(time);
    }
}

//+=============================================================================
// Leave pin off for time (given in microseconds)
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//
void IRsend::space(unsigned int time) {
#if defined(USE_NO_CARRIER)
    digitalWrite(sendPin, HIGH); // Set output to inactive high.
#else
    TIMER_DISABLE_PWM; // Disable pin 3 PWM output
#endif
    if (time > 0) {
        IRsend::custom_delay_usec(time);
    }
}

#ifdef USE_DEFAULT_ENABLE_IR_OUT
//+=============================================================================
// Enables IR output.  The khz value controls the modulation frequency in kilohertz.
// The IR output will be on pin 3 (OC2B).
// This routine is designed for 36-40KHz; if you use it for other values, it's up to you
// to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
// TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
// controlling the duty cycle.
// There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
// To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
// A few hours staring at the ATmega documentation and this will all make sense.
// See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.
//
void IRsend::enableIROut(int khz) {
#ifdef USE_SOFT_CARRIER
    periodTime = (1000U + khz / 2) / khz; // = 1000/khz + 1/2 = round(1000.0/khz)
    periodOnTime = periodTime * DUTY_CYCLE / 100U - PULSE_CORRECTION;
#endif

#if defined(USE_NO_CARRIER)
    pinMode(sendPin, OUTPUT);
    digitalWrite(sendPin, HIGH); // Set output to inactive high.
#else
    // Disable the Timer2 Interrupt (which is used for receiving IR)
    TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt

    pinMode(sendPin, OUTPUT);

    SENDPIN_OFF(sendPin); // When not sending, we want it low

    timerConfigkHz(khz);
#endif
}
#endif

//+=============================================================================
// Custom delay function that circumvents Arduino's delayMicroseconds limit

void IRsend::custom_delay_usec(unsigned long uSecs) {
    if (uSecs > 4) {
        unsigned long start = micros();
        unsigned long endMicros = start + uSecs - 4;
        if (endMicros < start) { // Check if overflow
            while (micros() > start) {
            } // wait until overflow
        }
        while (micros() < endMicros) {
        } // normal wait
    }
    //else {
    //  __asm__("nop\n\t"); // must have or compiler optimizes out
    //}
}

#endif // SENDING_SUPPORTED
