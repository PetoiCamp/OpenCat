/**
 * @file IRremote.h
 * @brief Public API to the library.
 */

//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
// Edited by Mitra to add new controller SANYO
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
// MagiQuest added by E. Stuart Hicks (based on code by mpflaga - https://github.com/mpflaga/Arduino-IRremote/)
//******************************************************************************
#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
// The ISR header contains several useful macros the user may wish to use
//
#include "private/IRremoteInt.h"

#ifdef ARDUINO_ARCH_AVR
#include <avr/pgmspace.h>
#define HAS_FLASH_READ 1
#define STRCPY_PF_CAST(x) (x)
#else
#define HAS_FLASH_READ 0
#endif

//------------------------------------------------------------------------------
// Supported IR protocols
// Each protocol you include costs memory and, during decode, costs time
// Disable (set to 0) all the protocols you do not need/want!
//
#define DECODE_RC5           0
#define SEND_RC5             0

#define DECODE_RC6           0
#define SEND_RC6             0

#define DECODE_NEC           1
#define SEND_NEC             0

#define DECODE_SONY          0
#define SEND_SONY            0

#define DECODE_PANASONIC     0
#define SEND_PANASONIC       0

#define DECODE_JVC           0
#define SEND_JVC             0

#define DECODE_SAMSUNG       0
#define SEND_SAMSUNG         0

#define DECODE_WHYNTER       0
#define SEND_WHYNTER         0

#define DECODE_AIWA_RC_T501  0
#define SEND_AIWA_RC_T501    0

#define DECODE_LG            0
#define SEND_LG              0

#define DECODE_SANYO         0
#define SEND_SANYO           0 // NOT WRITTEN

#define DECODE_MITSUBISHI    0
#define SEND_MITSUBISHI      0 // NOT WRITTEN

#define DECODE_DISH          0 // NOT WRITTEN
#define SEND_DISH            0

#define DECODE_SHARP         0
#define SEND_SHARP           0

#define DECODE_SHARP_ALT     0
#define SEND_SHARP_ALT       0

#define DECODE_DENON         0
#define SEND_DENON           0

#define DECODE_LEGO_PF       0 // NOT WRITTEN
#define SEND_LEGO_PF         0

#define DECODE_BOSEWAVE      0
#define SEND_BOSEWAVE        0

#define DECODE_MAGIQUEST     0
#define SEND_MAGIQUEST       0

#define DECODE_HASH          0 // special decoder for all protocols

/**
 * An enum consisting of all supported formats.
 * You do NOT need to remove entries from this list when disabling protocols!
 */
typedef enum {
    UNKNOWN = -1,
    UNUSED = 0,
    RC5,
    RC6,
    NEC,
    SONY,
    PANASONIC,
    JVC,
    SAMSUNG,
    WHYNTER,
    AIWA_RC_T501,
    LG,
    SANYO,
    MITSUBISHI,
    DISH,
    SHARP,
    SHARP_ALT,
    DENON,
    LEGO_PF,
    BOSEWAVE,
    MAGIQUEST,
} decode_type_t;

/**
 * Set DEBUG to 1 for lots of lovely debug output.
 */
#define DEBUG  0

//------------------------------------------------------------------------------
// Debug directives
//
#if DEBUG
#  define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#  define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
/**
 * If DEBUG, print the arguments, otherwise do nothing.
 */
#  define DBG_PRINT(...) void()
/**
 * If DEBUG, print the arguments as a line, otherwise do nothing.
 */
#  define DBG_PRINTLN(...) void()
#endif

//------------------------------------------------------------------------------
// Helper macro for getting a macro definition as string
//
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int MATCH(int measured, int desired);
int MATCH_MARK(int measured_ticks, int desired_us);
int MATCH_SPACE(int measured_ticks, int desired_us);

/**
 * Results returned from the decoder
 */
class decode_results {
public:
    decode_type_t decode_type;  ///< UNKNOWN, NEC, SONY, RC5, ...
    unsigned int address;       ///< Used by Panasonic & Sharp [16-bits]
    unsigned long value;        ///< Decoded value [max 32-bits]
    unsigned int magnitude;     ///< Used by MagiQuest [16-bits]
    int bits;                   ///< Number of bits in decoded value
    volatile unsigned int *rawbuf;      ///< Raw intervals in 50uS ticks
    unsigned int rawlen;        ///< Number of records in rawbuf
    int overflow;               ///< true if IR raw code too long
};

/**
 * Decoded value for NEC when a repeat code is received
 */
#define REPEAT 0xFFFFFFFF

/**
 * Main class for receiving IR
 */
class IRrecv {
public:
    /**
     * Instantiate the IRrecv class. Multiple instantiation is not supported.
     * @param recvpin Arduino pin to use. No sanity check is made.
     */
    IRrecv(int recvpin);
    /**
     * Instantiate the IRrecv class. Multiple instantiation is not supported.
     * @param recvpin Arduino pin to use, where a demodulating IR receiver is connected.
     * @param blinkpin pin to blink when receiving IR. Not supported by all hardware. No sanity check is made.
     */
    IRrecv(int recvpin, int blinkpin);

    /**
     * TODO: Why is this public???
     * @param blinkflag
     */
    void blink13(int blinkflag);

    /**
     * Attempt to decode the recently receive IR signal
     * @param results decode_results instance returning the decode, if any.
     * @return success of operation. TODO: convert to bool
     */
    int decode(decode_results *results);

    /**
     * Enable IR reception.
     */
    void enableIRIn();

    /**
     * Disable IR reception.
     */
    void disableIRIn();

    /**
     * Returns status of reception
     * @return true if no reception is on-going.
     */
    bool isIdle();

    /**
     * Called to re-enable IR reception.
     */
    void resume();

    /**
     * Print the result (second argument) as Pronto Hex on the Stream supplied as argument.
     * @param stream The Stream on which to write, often Serial
     * @param results the decode_results as delivered from irrecv.decode.
     * @param frequency Modulation frequency in Hz. Often 38000Hz.
     */
    void dumpPronto(Stream& stream, decode_results *results, unsigned int frequency = 38000U);

private:
#if DECODE_HASH
    long decodeHash(decode_results *results);
    int compare(unsigned int oldval, unsigned int newval);
#endif

    //......................................................................
#if (DECODE_RC5 || DECODE_RC6)
    /**
     *  This helper function is shared by RC5 and RC6
     */
    int getRClevel(decode_results *results, unsigned int *offset, int *used, int t1);
#endif
#if DECODE_RC5
    /**
     * Try to decode the recently received IR signal as an RC5 signal-
     * @param results decode_results instance returning the decode, if any.
     * @return Success of the operation.
     */
    bool decodeRC5(decode_results *results);
#endif
#if DECODE_RC6
    bool decodeRC6(decode_results *results);
#endif
    //......................................................................
#if DECODE_NEC
    bool decodeNEC(decode_results *results);
#endif
    //......................................................................
#if DECODE_SONY
    bool decodeSony(decode_results *results);
#endif
    //......................................................................
#if DECODE_PANASONIC
    bool decodePanasonic(decode_results *results);
#endif
    //......................................................................
#if DECODE_JVC
    bool decodeJVC(decode_results *results);
#endif
    //......................................................................
#if DECODE_SAMSUNG
    bool decodeSAMSUNG(decode_results *results);
#endif
    //......................................................................
#if DECODE_WHYNTER
    bool decodeWhynter(decode_results *results);
#endif
    //......................................................................
#if DECODE_AIWA_RC_T501
    bool decodeAiwaRCT501(decode_results *results);
#endif
    //......................................................................
#if DECODE_LG
    bool decodeLG(decode_results *results);
#endif
    //......................................................................
#if DECODE_SANYO
    bool decodeSanyo(decode_results *results);
#endif
    //......................................................................
#if DECODE_MITSUBISHI
    bool decodeMitsubishi(decode_results *results);
#endif
    //......................................................................
#if DECODE_DISH
      bool  decodeDish (decode_results *results) ; // NOT WRITTEN
#endif
    //......................................................................
#if DECODE_SHARP
    bool decodeSharp(decode_results *results); // NOT WRITTEN
#endif
#if DECODE_SHARP_ALT
    bool decodeSharpAlt(decode_results *results);
#endif
    //......................................................................
#if DECODE_DENON
    bool decodeDenon(decode_results *results);
#endif
    //......................................................................
#if DECODE_LEGO_PF
      bool  decodeLegoPowerFunctions (decode_results *results) ;
#endif
    //......................................................................
#if DECODE_BOSEWAVE
    bool decodeBoseWave(decode_results *results);
#endif
    //......................................................................
#if DECODE_MAGIQUEST
    bool decodeMagiQuest(decode_results *results);
#endif
};

/**
 * Main class for sending IR
 */
class IRsend {
public:
#if defined(USE_SOFT_CARRIER) || defined(USE_NO_CARRIER)
    IRsend(int pin = IR_SEND_PIN) {
      sendPin = pin;
    }
#else
    IRsend() {
    }
#endif

    void custom_delay_usec(unsigned long uSecs);
    void enableIROut(int khz);
    void mark(unsigned int usec);
    void space(unsigned int usec);
    void sendRaw(const unsigned int buf[], unsigned int len, unsigned int hz);
    void sendRaw_P(const unsigned int buf[], unsigned int len, unsigned int hz);

    //......................................................................
#if SEND_RC5
    void sendRC5(unsigned long data, int nbits);
    void sendRC5ext(unsigned long addr, unsigned long cmd, boolean toggle);
#endif
#if SEND_RC6
    void sendRC6(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_NEC
    void sendNEC(unsigned long data, int nbits, bool repeat = false);
#endif
    //......................................................................
#if SEND_SONY
    void sendSony(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_PANASONIC
    void sendPanasonic(unsigned int address, unsigned long data);
#endif
    //......................................................................
#if SEND_JVC
    // JVC does NOT repeat by sending a separate code (like NEC does).
    // The JVC protocol repeats by skipping the header.
    // To send a JVC repeat signal, send the original code value
    //   and set 'repeat' to true
    void sendJVC(unsigned long data, int nbits, bool repeat = false);
#endif
    //......................................................................
#if SEND_SAMSUNG
    void sendSAMSUNG(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_WHYNTER
    void sendWhynter(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_AIWA_RC_T501
    void sendAiwaRCT501(int code);
#endif
    //......................................................................
#if SEND_LG
    void sendLG(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_SANYO
      void  sendSanyo      ( ) ; // NOT WRITTEN
#endif
    //......................................................................
#if SEND_MISUBISHI
      void  sendMitsubishi ( ) ; // NOT WRITTEN
#endif
    //......................................................................
#if SEND_DISH
    void sendDISH(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_SHARP
    void sendSharpRaw(unsigned long data, int nbits);
    void sendSharp(unsigned int address, unsigned int command);
#endif
#if SEND_SHARP_ALT
    void sendSharpAltRaw(unsigned long data, int nbits);
    void sendSharpAlt(unsigned int address, unsigned long command);
#endif
    //......................................................................
#if SEND_DENON
    void sendDenon(unsigned long data, int nbits);
#endif
    //......................................................................
#if SEND_LEGO_PF
    void sendLegoPowerFunctions(uint16_t data, bool repeat = true);
#endif
    //......................................................................
#if SEND_BOSEWAVE
    void sendBoseWave(unsigned char code);
#endif
    //......................................................................
#if SEND_MAGIQUEST
    void sendMagiQuest(unsigned long wand_id, unsigned int magnitude);
#endif

    /**
     * Parse the string given as Pronto Hex, and send it a number of times given
     * as the second argument. Thereby the division of the Pronto Hex into
     * an intro-sequence and a repeat sequence is taken into account:
     * First the intro sequence is sent, then the repeat sequence is sent times-1 times.
     * However, if the intro sequence is empty, the repeat sequence is sent times times.
     * <a href="http://www.harctoolbox.org/Glossary.html#ProntoSemantics">Reference</a>.
     *
     * Note: Using this function is very wasteful for the memory consumption on
     * a small board.
     * Normally it is a much better ide to use a tool like e.g. IrScrutinizer
     * to transform Pronto type signals offline
     * to a more memory efficient format.
     *
     * @param prontoHexString C type string (null terminated) containing a Pronto Hex representation.
     * @param times Number of times to send the signal.
     */
    void sendPronto(const char* prontoHexString, unsigned int times = 1U);

    void sendPronto(const uint16_t* data, unsigned int length, unsigned int times = 1U);

#if HAS_FLASH_READ || defined(DOXYGEN)
    void sendPronto_PF(uint_farptr_t str, unsigned int times = 1U);

    /**
     * Version of sendPronto that reads from PROGMEM, saving RAM memory.
     * @param pronto C type string (null terminated) containing a Pronto Hex representation.
     * @param times Number of times to send the signal.
     */
    void sendPronto_PF(const char *str, unsigned int times = 1U);
    void sendPronto(const __FlashStringHelper *str, unsigned int times = 1U);
#endif

#if defined(USE_SOFT_CARRIER) || defined(USE_NO_CARRIER)
  private:
    int sendPin;

#  if defined(USE_SOFT_CARRIER)
    unsigned int periodTime;
    unsigned int periodOnTime;

    void sleepMicros(unsigned long us);
    void sleepUntilMicros(unsigned long targetTime);
#  endif

#else
    const int sendPin = IR_SEND_PIN;
#endif
};

#endif
