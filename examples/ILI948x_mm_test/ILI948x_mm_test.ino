// Set which Display we are using and at what speed
// Currently I have options for both MICROMOD and T42 to make it
// easier for testing
#ifdef ARDUINO_TEENSY_MICROMOD
#define NT35510X ILI9486
#define NT35510X_SPEED_MHZ 24
#elif defined(ARDUINO_TEENSY41)
#define NT35510X ILI9488
#define NT35510X_SPEED_MHZ 24
#endif

#include "NT35510_t4x_p.h"
#include "flexio_teensy_mm.c"

#ifdef ARDUINO_TEENSY41
NT35510_t4x_p tft = NT35510_t4x_p(10, 8, 9);  //(dc, cs, rst)
#else
NT35510_t4x_p tft = NT35510_t4x_p(4, 5, 3);  //(dc, cs, rst)
#endif

void setup() {
    Serial.begin(115200);
    delay(1000);
    if (CrashReport) {
        Serial.print(CrashReport);
    }

    // Begin optionally change FlexIO pins.
    //    WRITE, READ, D0, [D1 - D7]
    //    tft.setFlexIOPins(7, 8);
    //    tft.setFlexIOPins(7, 8, 40);
    //    tft.setFlexIOPins(7, 8, 40, 41, 42, 43, 44, 45, 6, 9);
    //tft.setFlexIOPins(7, 8);
    tft.begin(NT35510X, NT35510X_SPEED_MHZ);
    tft.setBitDepth(16);
    tft.displayInfo();
    tft.setRotation(3);
}

void loop() {
    tft.pushPixels16bit(flexio_teensy_mm, 0, 0, 479, 319);  // 480x320
    delay(3000);
    tft.fillScreen(NT35510_BLACK);
    delay(1000);
    tft.pushPixels16bitDMA(flexio_teensy_mm, 0, 0, 479, 319);  // 480x320
    delay(3000);
    tft.fillScreen(NT35510_BLACK);
    delay(1000);
    tft.writeRect(0, 0, 480, 320, flexio_teensy_mm);  // 480x320
    delay(2000);
}
