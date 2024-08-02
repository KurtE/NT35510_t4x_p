/***************************************************
  This is our GFX example for the Adafruit ILI9488 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#ifdef ARDUINO_TEENSY_MICROMOD
#define NT35510X ILI9486
#define NT35510X_SPEED_MHZ 24
#elif defined(ARDUINO_TEENSY41)
#define NT35510X ILI9488
#define NT35510X_SPEED_MHZ 24
#endif


#include <Teensy_Parallel_GFX.h>
#include <Adafruit_GFX.h>  // Core graphics library
#include "NT35510_t4x_p.h"
#include <NT35510_t3_font_ComicSansMS.h>

//#define TEENSY64

#ifdef ARDUINO_TEENSY41
NT35510_t4x_p tft = NT35510_t4x_p(10, 8, 9);  //(dc, cs, rst)
#else
NT35510_t4x_p tft = NT35510_t4x_p(4, 5, 3);  //(dc, cs, rst)
#endif

void setup() {

    Serial.begin(9600);

    // Begin optionally change FlexIO pins.
    //    WRITE, READ, D0, [D1 - D7]
    //    tft.setFlexIOPins(7, 8);
    //    tft.setFlexIOPins(7, 8, 40);
    //    tft.setFlexIOPins(7, 8, 40, 41, 42, 43, 44, 45, 6, 9);
    //tft.setFlexIOPins(7, 8);
    tft.begin(NT35510X, NT35510X_SPEED_MHZ);
    tft.setBitDepth(16);

    tft.setRotation(3);

    tft.fillScreen(NT35510_BLACK);
    while (!Serial)
        ;
    tft.setTextColor(NT35510_WHITE);
    tft.setTextSize(4);
    tft.enableScroll();
    tft.setScrollTextArea(0, 0, 120, 240);
    tft.setScrollBackgroundColor(NT35510_GREEN);

    tft.setCursor(180, 100);

    tft.setFont(ComicSansMS_12);
    tft.print("Fixed text");

    tft.setCursor(0, 0);

    tft.setTextColor(NT35510_BLACK);

    for (int i = 0; i < 20; i++) {
        tft.print("  this is line ");
        tft.println(i);
        delay(500);
    }

    tft.fillScreen(NT35510_BLACK);
    tft.setScrollTextArea(40, 50, 120, 120);
    tft.setScrollBackgroundColor(NT35510_GREEN);
    tft.setFont(ComicSansMS_10);

    tft.setTextSize(1);
    tft.setCursor(40, 50);

    for (int i = 0; i < 20; i++) {
        tft.print("  this is line ");
        tft.println(i);
        delay(500);
    }
}



void loop(void) {
}
