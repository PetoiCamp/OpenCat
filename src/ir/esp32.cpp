#ifdef ESP32

// This file contains functions specific to the ESP32.

#include "IRremote.h"

// "Idiot check"
#ifdef USE_DEFAULT_ENABLE_IR_IN
#error Must undef USE_DEFAULT_ENABLE_IR_IN
#endif

hw_timer_t *timer;
IRAM_ATTR void IRTimer(); // defined in IRremote.cpp, masqueraded as ISR(TIMER_INTR_NAME)

//+=============================================================================
// initialization
//
void IRrecv::enableIRIn() {
// Interrupt Service Routine - Fires every 50uS
    // ESP32 has a proper API to setup timers, no weird chip macros needed
    // simply call the readable API versions :)
    // 3 timers, choose #1, 80 divider nanosecond precision, 1 to count up
    timer = timerBegin(1, 80, 1);
    timerAttachInterrupt(timer, &IRTimer, 1);
    // every 50ns, autoreload = true
    timerAlarmWrite(timer, 50, true);
    timerAlarmEnable(timer);

    // Initialize state machine variables
    irparams.rcvstate = IR_REC_STATE_IDLE;
    irparams.rawlen = 0;

    // Set pin modes
    pinMode(irparams.recvpin, INPUT);
}

void IRrecv::disableIRIn() {
    timerEnd(timer);
    timerDetachInterrupt(timer);
}

void IRsend::enableIROut(int khz) {
    ledcSetup(LEDCHANNEL, khz * 1000, 8);  // 8 bit PWM resolution
    ledcAttachPin(IR_SEND_PIN, LEDCHANNEL); // bind pin to channel
}

#endif // ESP32
