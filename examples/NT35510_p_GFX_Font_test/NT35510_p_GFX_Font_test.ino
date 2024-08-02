//-------------------------------------------------------------------
//
// This test program illustrates a simple use of ILI9341 fonts with the
// ILI9488 display.
//
// this sketch is in the public domain.
//
// This sketch depends on the fonts that are contained in the library
//     https://github.com/mjs513/ILI9341_fonts
//-------------------------------------------------------------------
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



#include <Adafruit_GFX.h>
#include <Teensy_Parallel_GFX.h>

#include <SPI.h>
#include "NT35510_t4x_p.h"

#include <NT35510_t3_font_ComicSansMS.h>
#include "Fonts/FreeSansOblique12pt7b.h"

#ifdef ARDUINO_TEENSY41
NT35510_t4x_p tft = NT35510_t4x_p(10, 8, 9);  //(dc, cs, rst)
#else
NT35510_t4x_p tft = NT35510_t4x_p(4, 5, 3);  //(dc, cs, rst)
#endif


void setup() {
    Serial.begin(38400);
    long unsigned debug_start = millis();
    while (!Serial && ((millis() - debug_start) <= 5000))
        ;
    Serial.println("Setup");

    // Begin optionally change FlexIO pins.
    //    WRITE, READ, D0, [D1 - D7]
    //    tft.setFlexIOPins(7, 8);
    //    tft.setFlexIOPins(7, 8, 40);
    //    tft.setFlexIOPins(7, 8, 40, 41, 42, 43, 44, 45, 6, 9);
    //tft.setFlexIOPins(7, 8);
    tft.begin(NT35510X, NT35510X_SPEED_MHZ);
    tft.setBitDepth(16);

    tft.setRotation(1);
    tft.fillScreen(NT35510_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(NT35510_WHITE);
    tft.println("Test of the default font");
    tft.println();
    tft.setTextSize(1);

    tft.setTextColor(NT35510_WHITE, NT35510_BLUE);
    tft.setFont(ComicSansMS_12);
    tft.println("Opaque ILI font ComicSansMS_12");
    tft.println();

    tft.setTextColor(NT35510_GREEN);
    tft.println("Transparent ILIComicSansMS_12");
    tft.println();

    tft.setTextColor(NT35510_WHITE, NT35510_RED);
    tft.setFont(&FreeSansOblique12pt7b);
    tft.println("Opaque GFX FreeSansOblique12pt");
    int cursor_x = tft.getCursorX();
    int cursor_y = tft.getCursorY();
    tft.fillRect(cursor_x, cursor_y, tft.width(), 20, NT35510_YELLOW);
    tft.setTextColor(NT35510_BLUE);
    tft.println("Transparent GFX");

    tft.setFont();
    tft.setTextSize(2);
    tft.setTextColor(NT35510_GREEN);
    tft.setTextSize(1);
    tft.println("This is default font:");
    //tft.setFontSpacing(1);//now give 5 pix extra spacing between chars
    tft.println("ABCDEF 1 2 3 4 5 6 7");
}

void loop() {}