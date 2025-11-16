/** 
 * This example shows how to use the TcMenu logging framework, to log simple information
 * to the serial port. It should work on Arduino, mbed and PicoSDK with little to no
 * adjustment.
 *
 * As this example logs output, either define flag `IO_LOGGING_DEBUG` or open IoLogging.h
 * and uncomment `#define IO_LOGGING_DEBUG` to enable.
 * 
 * This logging is only complied in when the above define is set, if it is not set then
 * the logging is completely removed.
 *
 * Full documentation is here:
 * https://tcmenu.github.io/documentation/arduino-libraries//io-abstraction/arduino-logging-with-io-logging/
 */

/* The logging levels are below:
    SER_WARNING = 0x0001,
    SER_ERROR = 0x0002,
    SER_DEBUG = 0x0004,
    SER_TCMENU_INFO = 0x0008,
    SER_NETWORK_INFO = 0x0010,
    SER_IOA_INFO = 0x0020,
    SER_USER_1 = 0x0040,
    SER_USER_2 = 0x0080,
    SER_USER_3 = 0x0100,
    SER_USER_4 = 0x0200,
    SER_USER_5 = 0x0400,
    SER_USER_6 = 0x0800,
    SER_TCMENU_DEBUG = 0x1000,
    SER_NETWORK_DEBUG = 0x2000,
    SER_IOA_DEBUG = 0x4000,
    SER_LOG_EVERYTHING = 0xffff

 You can enable and disable logging altogether by defining: IO_LOGGING_DEBUG
 You can set the levels as a bit mask of the above levels using flag: IO_LOGGING_DEFAULT_LEVEL
 At runtime a level can be turned on/off using: void serEnableLevel(SerLoggingLevel level, bool active)
 */

#include <IoLogging.h>

char sz[] = {"hello world"};

// this sets up logging on mbed but is ignored on Arduino, so you can leave in place for both.
IOLOG_MBED_PORT_IF_NEEDED(USBTX, USBRX);

void setup() {
    // This example logs using IoLogging, see the following guide to enable
    // https://www.thecoderscorner.com/products/arduino-libraries/io-abstraction/arduino-logging-with-io-logging/
    IOLOG_START_SERIAL

    Serial.println("Starting ioLogging example");

    // enable an extra level
    serEnableLevel(SER_IOA_DEBUG, true);

    // write a string entry that is applied with F(..) so in progmem on AVR
    // with an integer second value.
    serdebugF2("In setup function - A0=", analogRead(A0));

    // write values to log in HEX - first parameter is wrapped in F(..) using the F variant
    serlogFHex2(SER_DEBUG, "Two Values in hex: ", 0xFADE, 0xFACE);
    serlogFHex(SER_DEBUG, "One Values in hex: ", 0xFADE);
    serlogF4(SER_ERROR, "This is an error", 100, 200, 300);
    serlogF3(SER_WARNING, "This is an warning", 100, 200);

    // log at SER_DEBUG, for legacy support
    serdebugF2("Int value: ", 109298384L);
    serdebugF2("Bool value: ", true);

    // here we hex dump an array
    serlogHexDump(SER_DEBUG, "Hex dump", sz, sizeof sz);

    // the F variant always tries to use F(..) to save ram on the first parameter on AVR
    serdebugF("String in flash");

    // this version does not use F(..) so we can pass RAM strings even on AVR
    serdebug(sz);
}

void loop() {
    delay(1000);
    serlogF2(SER_USER_1, "Millis: ", millis());
}