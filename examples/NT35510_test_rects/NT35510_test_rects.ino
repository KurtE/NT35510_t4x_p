//-------------------------------------------------------------------
// Kurt's Frame buffer and clip tests
//
// This test program is a set of random tests that have been done
// over time to test out the different functions to make sure they
// are working with the clipping functions as well as the frame
// buffer.  So you can for example test to see if you get the
// same results with the frame buffer turned on or off
//
// this sketch is in the public domain.
//
// This sketch depends on the fonts that are contained in the library
//     https://github.com/mjs513/ILI9341_fonts
//-------------------------------------------------------------------
// Set which Display we are using and at what speed
// Currently I have options for both MICROMOD and T42 to make it
// easier for testing

#define NT35510X NT35510
#define NT35510X_SPEED_MHZ 20


#include <MemoryHexDump.h>

#include <NT35510_t4x_p.h>
#include <Teensy_Parallel_GFX.h>
#include <Adafruit_GFX.h>  // Core graphics library
#include "font_Arial.h"
#include "font_ArialBold.h"
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#define ROTATION 1

#define KURTS_MICROMOD

#include "SPI.h"

//Adafruit_GFX_Button button;


// Let's allocate the frame buffer ourself.
//DMAMEM uint16_t tft_frame_buffer[NT35510_TFTWIDTH * NT35510_TFTHEIGHT];

uint8_t use_dma = 0;
uint8_t use_clip_rect = 0;
uint8_t use_set_origin = 0;
uint8_t use_fb = 0;
uint8_t *tft_frame_buffer = nullptr;

#define ORIGIN_TEST_X 50
#define ORIGIN_TEST_Y 50

#ifdef ARDUINO_TEENSY41
NT35510_t4x_p tft = NT35510_t4x_p(10, 8, 9);  //(dc, cs, rst)
#elif ARDUINO_TEENSY40
NT35510_t4x_p tft = NT35510_t4x_p(0, 1, 2);  //(dc, cs, rst)
#elif defined(ARDUINO_TEENSY_DEVBRD4) || defined(ARDUINO_TEENSY_DEVBRD5)
NT35510_t4x_p tft = NT35510_t4x_p(10, 11, 12);  //(dc, cs, rst)
#else
NT35510_t4x_p tft = NT35510_t4x_p(4, 5, 3);  //(dc, cs, rst)
#endif

void setup() {
    while (!Serial && (millis() < 4000))
        ;
    Serial.begin(115200);

    if (CrashReport) {
        Serial.print(CrashReport);
        WaitForUserInput();
    }
    //Serial.printf("Begin: CS:%d, DC:%d, MOSI:%d, MISO: %d, SCK: %d, RST: %d\n", TFT_CS, TFT_DC, TFT_MOSI, TFT_MISO, TFT_SCK, TFT_RST);
    Serial.println("\n*** Sketch Startup ***");
#ifdef TFT_TOUCH_CS
    pinMode(TFT_TOUCH_CS, OUTPUT);
    digitalWrite(TFT_TOUCH_CS, HIGH);
#endif


/*
   * begin(Dispalay type, baud)
   * Display type is associated with the the diplay
   * init configurations:
   * ILI9588, ILI9486, ILI9481_1, ILI9481_2, R61529
   * Baud can be as defined in the readme.
   * begin defaults to ILI9488 and 20Mhz:
   *     lcd.begin();
  */
// Begin optionally change FlexIO pins.
//    WRITE, READ, D0, [D1 - D7]
//    tft.setFlexIOPins(7, 8);
//    tft.setFlexIOPins(7, 8, 40);
//    tft.setFlexIOPins(7, 8, 40, 41, 42, 43, 44, 45, 6, 9);
//tft.setFlexIOPins(7, 8);
#if defined(ARDUINO_TEENSY_DEVBRD4)
    Serial.print("DEVBRD4 - ");
    tft_frame_buffer = (uint8_t *)sdram_malloc(tft.width() * tft.height() * 2 + 32);
#elif defined(ARDUINO_TEENSY_DEVBRD5)
    Serial.print("DEVBRD5 - ");
    tft_frame_buffer = (uint8_t *)sdram_malloc(tft.width() * tft.height() * 2 + 32);
#elif defined(ARDUINO_TEENSY_MICROMOD)
    Serial.print("Micromod - ");
#elif defined(ARDUINO_TEENSY41)
    Serial.print("Teensy4.1 - ");
    tft_frame_buffer = (uint8_t *)extmem_malloc(tft.width() * tft.height() * 2 + 32);
#endif
    Serial.println(NT35510X_SPEED_MHZ);
    tft.begin(NT35510X, NT35510X_SPEED_MHZ);

    tft.setBitDepth(24);

    tft.displayInfo();

    // Frame buffer will not fit work with malloc see if
    if (tft_frame_buffer) tft.setFrameBuffer((uint16_t *)(((uintptr_t)tft_frame_buffer + 32) & ~((uintptr_t)(31))));

    tft.setRotation(ROTATION);
    tft.fillScreen(NT35510_BLACK);
    Serial.printf("Screen width:%u height:%u\n", tft.width(), tft.height());

    delay(500);
    tft.fillScreen(NT35510_RED);
    delay(500);
    tft.fillScreen(NT35510_GREEN);
    delay(500);
    tft.fillScreen(NT35510_BLUE);
}

void WaitForUserInput() {
    Serial.println("Hit Enter to continue");
    Serial.flush();
    while (Serial.read() == -1)
        ;
    while (Serial.read() != -1)
        ;
}


uint16_t colors[] = { NT35510_RED, NT35510_GREEN, NT35510_BLUE, NT35510_LIGHTGREY, NT35510_YELLOW, NT35510_DARKGREY, NT35510_CYAN, NT35510_PINK };
uint8_t count_colors = sizeof(colors) / sizeof(colors[0]);
uint16_t pixel_buffer[12000];

void drawTestScreen(uint8_t rot) {

    uint16_t band_width = tft.width() / count_colors;
    uint16_t band_height = tft.height() / 4;

    tft.fillScreen(NT35510_BLACK);
    Serial.printf("Screen: %u %u Bands: %u %u\n", tft.width(), tft.height(), band_width, band_height);

    for (uint8_t i = 0; i < count_colors; i++) {
        int x = i * band_width + 1;

        tft.drawRect(x, 1, band_width - 2, band_height - 2, colors[i]);
        //WaitForUserInput();
        tft.fillRect(x, band_height + 1, band_width - 2, band_height - 2, colors[(i + 1) & 7]);
        //WaitForUserInput();
        for (uint32_t j = 0; j < 12000; j++) pixel_buffer[j] = colors[(i + 2) & 7];
        tft.writeRect(x, 2 * band_height + 1, band_width - 2, band_height - 2, pixel_buffer);

        //WaitForUserInput();
        tft.fillRectVGradient(x, 3 * band_height + 1, band_width - 2, band_height - 2,
                              colors[(i + 3) & 7], colors[(i + 4) & 7]);

        //WaitForUserInput();
    }
    tft.drawLine(0, 0, tft.width()-1, tft.height()-1, NT35510_OLIVE);
    tft.drawLine(0, tft.height()-1, tft.width()-1, 0, NT35510_ORANGE);
    tft.setTextSize(4);
    tft.setTextColor(NT35510_PURPLE);
    tft.setCursor(tft.width() / 2, tft.height() / 2, true);
    tft.print(rot);
}


uint8_t rotation = 3;
void loop(void) {
    WaitForUserInput();
    rotation = (rotation + 1) & 0x3;
    Serial.printf("Test Rotation: %u\n", rotation);
    tft.setRotation(rotation);
    drawTestScreen(rotation);
}
