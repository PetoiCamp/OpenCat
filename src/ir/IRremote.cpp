//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
//
// Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
// Modified  by Mitra Ardron <mitra@mitra.biz>
// Added Sanyo and Mitsubishi controllers
// Modified Sony to spot the repeat codes that some Sony's send
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
//******************************************************************************

// Defining IR_GLOBAL here allows us to declare the instantiation of global variables
#define IR_GLOBAL
#include "IRremote.h"
#undef IR_GLOBAL

//+=============================================================================
// The match functions were (apparently) originally MACROs to improve code speed
//   (although this would have bloated the code) hence the names being CAPS
// A later release implemented debug output and so they needed to be converted
//   to functions.
// I tried to implement a dual-compile mode (DEBUG/non-DEBUG) but for some
//   reason, no matter what I did I could not get them to function as macros again.
// I have found a *lot* of bugs in the Arduino compiler over the last few weeks,
//   and I am currently assuming that one of these bugs is my problem.
// I may revisit this code at a later date and look at the assembler produced
//   in a hope of finding out what is going on, but for now they will remain as
//   functions even in non-DEBUG mode
//
int MATCH(int measured, int desired) {
#if DEBUG
    Serial.print(F("Testing: "));
    Serial.print(TICKS_LOW(desired), DEC);
    Serial.print(F(" <= "));
    Serial.print(measured, DEC);
    Serial.print(F(" <= "));
    Serial.print(TICKS_HIGH(desired), DEC);
#endif
    bool passed = ((measured >= TICKS_LOW(desired)) && (measured <= TICKS_HIGH(desired)));
#if DEBUG
    if (passed) {
        Serial.println(F("?; passed"));
    } else {
        Serial.println(F("?; FAILED"));
    }
#endif
    return passed;
}

//+========================================================
// Due to sensor lag, when received, Marks tend to be 100us too long
//
int MATCH_MARK(int measured_ticks, int desired_us) {
#if DEBUG
    Serial.print(F("Testing mark (actual vs desired): "));
    Serial.print(measured_ticks * MICROS_PER_TICK, DEC);
    Serial.print(F("us vs "));
    Serial.print(desired_us, DEC);
    Serial.print(F("us: "));
    Serial.print(TICKS_LOW(desired_us + MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(measured_ticks * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(TICKS_HIGH(desired_us + MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
#endif
    // compensate for marks exceeded by demodulator hardware
    bool passed = ((measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS_MICROS))
            && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS_MICROS)));
#if DEBUG
    if (passed) {
        Serial.println(F("?; passed"));
    } else {
        Serial.println(F("?; FAILED"));
    }
#endif
    return passed;
}

//+========================================================
// Due to sensor lag, when received, Spaces tend to be 100us too short
//
int MATCH_SPACE(int measured_ticks, int desired_us) {
#if DEBUG
    Serial.print(F("Testing space (actual vs desired): "));
    Serial.print(measured_ticks * MICROS_PER_TICK, DEC);
    Serial.print(F("us vs "));
    Serial.print(desired_us, DEC);
    Serial.print(F("us: "));
    Serial.print(TICKS_LOW(desired_us - MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(measured_ticks * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(TICKS_HIGH(desired_us - MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
#endif
    // compensate for marks exceeded and spaces shortened by demodulator hardware
    bool passed = ((measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS_MICROS))
            && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS_MICROS)));
#if DEBUG
    if (passed) {
        Serial.println(F("?; passed"));
    } else {
        Serial.println(F("?; FAILED"));
    }
#endif
    return passed;
}

//+=============================================================================
// Interrupt Service Routine - Fires every 50uS
// TIMER2 interrupt code to collect raw data.
// Widths of alternating SPACE, MARK are recorded in rawbuf.
// Recorded in ticks of 50uS [microseconds, 0.000050 seconds]
// 'rawlen' counts the number of entries recorded so far.
// First entry is the SPACE between transmissions.
// As soon as a the first [SPACE] entry gets long:
//   Ready is set; State switches to IDLE; Timing of SPACE continues.
// As soon as first MARK arrives:
//   Gap width is recorded; Ready is cleared; New logging starts
//
ISR (TIMER_INTR_NAME) {
    TIMER_RESET; // reset timer interrupt flag if required (currently only for Teensy and ATmega4809)

    // Read if IR Receiver -> SPACE [xmt LED off] or a MARK [xmt LED on]
    // digitalRead() is very slow. Optimisation is possible, but makes the code unportable
    uint8_t irdata = (uint8_t) digitalRead(irparams.recvpin);

    irparams.timer++;  // One more 50uS tick
    if (irparams.rawlen >= RAW_BUFFER_LENGTH) {
        irparams.rcvstate = IR_REC_STATE_OVERFLOW;  // Buffer overflow
    }

    /*
     * Due to a ESP32 compiler bug https://github.com/espressif/esp-idf/issues/1552 no switch statements are possible for ESP32
     * So we change the code to if / else if
     */
//    switch (irparams.rcvstate) {
    //......................................................................
    if (irparams.rcvstate == IR_REC_STATE_IDLE) { // In the middle of a gap
        if (irdata == MARK) {
            if (irparams.timer < GAP_TICKS) {  // Not big enough to be a gap.
                irparams.timer = 0;
            } else {
                // Gap just ended; Record duration; Start recording transmission
                irparams.overflow = false;
                irparams.rawlen = 0;
                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                irparams.timer = 0;
                irparams.rcvstate = IR_REC_STATE_MARK;
            }
        }
    } else if (irparams.rcvstate == IR_REC_STATE_MARK) {  // Timing Mark
        if (irdata == SPACE) {   // Mark ended; Record time
            irparams.rawbuf[irparams.rawlen++] = irparams.timer;
            irparams.timer = 0;
            irparams.rcvstate = IR_REC_STATE_SPACE;
        }
    } else if (irparams.rcvstate == IR_REC_STATE_SPACE) {  // Timing Space
        if (irdata == MARK) {  // Space just ended; Record time
            irparams.rawbuf[irparams.rawlen++] = irparams.timer;
            irparams.timer = 0;
            irparams.rcvstate = IR_REC_STATE_MARK;

        } else if (irparams.timer > GAP_TICKS) {  // Space
            // A long Space, indicates gap between codes
            // Flag the current code as ready for processing
            // Switch to STOP
            // Don't reset timer; keep counting Space width
            irparams.rcvstate = IR_REC_STATE_STOP;
        }
    } else if (irparams.rcvstate == IR_REC_STATE_STOP) {  // Waiting; Measuring Gap
        if (irdata == MARK) {
            irparams.timer = 0;  // Reset gap timer
        }
    } else if (irparams.rcvstate == IR_REC_STATE_OVERFLOW) {  // Flag up a read overflow; Stop the State Machine
        irparams.overflow = true;
        irparams.rcvstate = IR_REC_STATE_STOP;
    }

#ifdef BLINKLED
    // If requested, flash LED while receiving IR data
    if (irparams.blinkflag) {
        if (irdata == MARK) {
            if (irparams.blinkpin) {
                digitalWrite(irparams.blinkpin, HIGH); // Turn user defined pin LED on
            } else {
                BLINKLED_ON();   // if no user defined LED pin, turn default LED pin for the hardware on
            }
        } else {
            if (irparams.blinkpin) {
                digitalWrite(irparams.blinkpin, LOW); // Turn user defined pin LED on
            } else {
                BLINKLED_OFF();   // if no user defined LED pin, turn default LED pin for the hardware on
            }
        }
    }
#endif // BLINKLED
}
