#include <Teensy_Parallel_GFX.h>
#include "NT35510_t4x_p.h"
//#include <HX8357_t4x_p.h>
#include "cricket.h"
#include "I_can_not_hear_you.h"

#ifdef ARDUINO_TEENSY41
#define TFT_DC 10
#define TFT_CS 8
#define TFT_RST 9
#elif defined(ARDUINO_TEENSY40)
#define TFT_DC 0
#define TFT_CS 1
#define TFT_RST 2
#elif defined(ARDUINO_TEENSY_DEVBRD4) || defined(ARDUINO_TEENSY_DEVBRD5)
extern "C" bool sdram_begin(uint8_t external_sdram_size, uint8_t clock, uint8_t useDQS);
#define TFT_DC 10
#define TFT_CS 11
#define TFT_RST 12
#else  // micromod?
#define TFT_DC 4
#define TFT_CS 5
#define TFT_RST 3
#endif

#ifdef _HX8357_T4X_P_H_
#define DISPLAY_TYPE HX8357D
#define DISPLAY_SPEED_MHZ 20
#define DISPLAY_ROTATION 0
HX8357_t4x_p tft = HX8357_t4x_p(TFT_DC, TFT_CS, TFT_RST);  //(dc, cs, rst)
#else
#define DISPLAY_TYPE NT35510
#define DISPLAY_SPEED_MHZ 30
#define DISPLAY_ROTATION 1
NT35510_t4x_p tft = NT35510_t4x_p(TFT_DC, TFT_CS, TFT_RST);  //(dc, cs, rst)
#endif

#ifndef BLUE
#define BLUE 0x001F
#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define RED 0xf800
#endif

void setup(void) {
    while (!Serial && millis() < 3000)
        ;

    Serial.println("*** start up NT35510 ***");
#ifdef ARDUINO_TEENSY41
    tft.setBusWidth(16);
#else
    tft.setBusWidth(8);
#endif
    tft.begin(DISPLAY_TYPE, DISPLAY_SPEED_MHZ);
    tft.setBitDepth(24);
    tft.setRotation(DISPLAY_ROTATION);
    tft.displayInfo();

    //tft.fillScreen(RED);
    tft.fillRect(0, 0, tft.width() / 2, tft.height(), RED);
    tft.fillRect24BPP(tft.width() / 2, 0, tft.width() / 2, tft.height(), tft.color888(0xff, 0, 0));
    delay(1000);
    //tft.fillScreen(GREEN);
    tft.fillRect(0, 0, tft.width() / 2, tft.height(), GREEN);
    tft.fillRect24BPP(tft.width() / 2, 0, tft.width() / 2, tft.height(), tft.color888(0, 0xff, 0));
    delay(1000);
    //tft.fillScreen(BLUE);
    tft.fillRect(0, 0, tft.width() / 2, tft.height(), BLUE);
    tft.fillRect24BPP(tft.width() / 2, 0, tft.width() / 2, tft.height(), tft.color888(0, 0, 0xff));
    delay(500);

    tft.writeRect(50, 50,
                  gimp_image.width, gimp_image.height, (uint16_t*)gimp_image.pixel_data);

    delay(1000);
    tft.writeRect(tft.width() - (gimp_image_hear.width + 50), tft.height() - (gimp_image_hear.height + 50),
                  gimp_image_hear.width, gimp_image_hear.height, (uint16_t*)gimp_image_hear.pixel_data);
}

void fillScreenOneColorRange(uint32_t color_start, uint32_t color_end) {
    uint8_t r, g, b;
    uint8_t rs, gs, bs, re, ge, be;
    tft.color888toRGB(color_start, rs, gs, bs);
    tft.color888toRGB(color_end, re, ge, be);

    int band_width = tft.width() / 256;
    int band_height = tft.height() / 2;
    int band_start_x = (tft.width() - (band_width * 256)) / 2;

    Serial.printf("fillScreenOneColorRange %x(%x) : %x(%x)\n", 
            color_start, tft.color565(rs, gs, bs), 
            color_end, tft.color565(re, ge, be));

    for (uint32_t i = 0; i < 256; i++) {
        r = rs + (((uint32_t)(re - rs)) * i + 128) / 256;
        g = gs + (((uint32_t)(ge - gs)) * i + 128) / 256;
        b = bs + (((uint32_t)(be - bs)) * i + 128) / 256;
        tft.fillRect(i * band_width + band_start_x, 0, band_width, band_height, tft.color565(r, g, b));
        tft.fillRect24BPP(i * band_width + band_start_x, band_height, band_width, band_height, tft.color888(r, g, b));
    }
}

uint32_t colors[] = { tft.color888(255, 0, 0), tft.color888(0, 255, 0), tft.color888(0, 0, 255),
                      tft.color888(255, 255, 0), tft.color888(255, 0, 255), tft.color888(0, 255, 255), tft.color888(255, 255, 255) };
uint8_t color_index = 0xff;

void loop() {
    Serial.println("Press any key to continue");
    while (Serial.read() == -1) {}
    while (Serial.read() != -1) {}
    color_index++;
    if (color_index == (sizeof(colors) / sizeof(colors[0]))) color_index = 0;
    fillScreenOneColorRange(tft.color888(0, 0, 0), colors[color_index]);
}
