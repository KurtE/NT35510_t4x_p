/***************************************************
  This is a modified version of the painting example
  from Adafruit_FT6206, that is modified to work with
  the NT35510

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#define NT35510X NT35510
#define NT35510X_SPEED_MHZ 24
#define BUS_WIDTH 8
#define BIT_DEPTH 16
#define ROTATION 3

#ifdef ARDUINO_TEENSY41
#define TOUCH_WIRE Wire2
#define CTP_INT_PIN 26

#else
#define TOUCH_WIRE Wire
#define CTP_INT_PIN 22  // 0xff if pin is not connected
#endif

#define MAX_TOUCH 1  // can be 1 or 2, but we don't use 2nd so...

//#define SHOW_TOUCH_INFO

#include <Wire.h>  // this is needed for FT6206
#include <FT6x36_t4.h>
#include <NT35510_t4x_p.h>
#include <Teensy_Parallel_GFX.h>
#include <Adafruit_GFX.h>  // Core graphics library

// The FT6206 uses hardware I2C (SCL/SDA)
//Adafruit_FT6206 ctp = Adafruit_FT6206();
FT6x36_t4 ctp(CTP_INT_PIN, MAX_TOUCH);

#ifdef ARDUINO_TEENSY41
NT35510_t4x_p tft = NT35510_t4x_p(10, 8, 9);  //(dc, cs, rst)
#elif ARDUINO_TEENSY40
NT35510_t4x_p tft = NT35510_t4x_p(0, 1, 2);  //(dc, cs, rst)
#elif defined(ARDUINO_TEENSY_DEVBRD4)
NT35510_t4x_p tft = NT35510_t4x_p(10, 11, 12);  //(dc, cs, rst)
#elif defined(ARDUINO_TEENSY_DEVBRD5)
#undef ROTATION
#define ROTATION 3
#if BUS_WIDTH == 18
NT35510_t4x_p tft = NT35510_t4x_p(55, 59, 54);  //(dc, cs, rst)
#else
NT35510_t4x_p tft = NT35510_t4x_p(55, 53, 54);  //(dc, cs, rst)
#endif
#else
NT35510_t4x_p tft = NT35510_t4x_p(4, 5, 3);  //(dc, cs, rst)
#endif



// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 80
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
    Serial.begin(115200);
    while (!Serial && millis() < 5000) delay(10);  // pause the serial port
    if (CrashReport) {
        Serial.print(CrashReport);
        Serial.println("Press any key to continue");
        while (Serial.read() == -1) {}
        while (Serial.read() != -1) {}
    }

    Serial.println(F("NT35510 Cap Touch Paint!"));

#if defined(ARDUINO_TEENSY_DEVBRD4)
    Serial.print("DEVBRD4 - ");
#elif defined(ARDUINO_TEENSY_DEVBRD5)
    Serial.print("DEVBRD5 - ");
    tft.setBusWidth(BUS_WIDTH);
#elif defined(ARDUINO_TEENSY_MICROMOD)
    Serial.print("Micromod - ");
#elif defined(ARDUINO_TEENSY41)
    Serial.print("Teensy4.1 - ");
#endif
    Serial.println(NT35510X_SPEED_MHZ);
#ifdef ARDUINO_TEENSY41
    pinMode(27, INPUT_PULLDOWN);
    delay(10);  // plenty of time
    // if the user tied this pin to 3.3v then try 16 bit bus...
    Serial.printf("Pin 27, %u\n", digitalRead(27));
    tft.setBusWidth(digitalRead(27) ? 16 : 8);
#endif

    Serial.println("Before tft.begin");
    Serial.flush();
    tft.begin(NT35510X, NT35510X_SPEED_MHZ);
    tft.setRotation(ROTATION);

    Serial.printf("Before setBitDepth: %u\n", BIT_DEPTH);
    Serial.flush();
    tft.setBitDepth(BIT_DEPTH);

    Serial.println("Before displayInfo");
    Serial.flush();
    tft.displayInfo();
    tft.fillScreen(NT35510_RED);

    Serial.println("\n*** Start Touch controller ***");
    TOUCH_WIRE.begin();
    //    if (!ctp.begin(&Wire, 0x38)) {  // Optional pass in which Wire object and device ID
    if (!ctp.begin(&TOUCH_WIRE)) {  // Use default: Wire and 0x38
        Serial.println("Couldn't start FT6236 touchscreen controller");
        while (1) delay(10);
    }

    Serial.println("Capacitive touchscreen started");
    ctp.showAllRegisters();

    tft.fillScreen(NT35510_BLACK);

    // make the color selection boxes
    tft.fillRect(0, 0, BOXSIZE, BOXSIZE, NT35510_RED);
    tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, NT35510_YELLOW);
    tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, NT35510_GREEN);
    tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, NT35510_CYAN);
    tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, NT35510_BLUE);
    tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, NT35510_MAGENTA);

    // select the current color 'red'
    tft.drawRect(0, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
    currentcolor = NT35510_RED;
}

// quick and dirty conversion struct;
typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;

uint32_t time_sum = 0;
uint32_t time_count = 0;

void loop() {
    // Check for Serial at start, so don't have be touching display to get touch information...
    if (Serial.available()) {
        while (Serial.read() != -1) {}
        ctp.showAllRegisters();
    }
    // Wait for a touch
    elapsedMicros em;
    if (!ctp.touched()) {
        if (time_count) {
            Serial.printf("touch cnt: %u dt: %u avg:%u\n", time_count, time_sum, time_sum / time_count);
            time_sum = 0;
            time_count = 0;
        }
        return;
    }

    // Retrieve a point
    point_t p;
    //TS_Point p = ctp.getPoint();
    ctp.touchPoint(p.x, p.y);

    time_sum += em;
    time_count++;

    // Print out raw data from screen touch controller

    #ifdef SHOW_TOUCH_INFO
    Serial.print("X = ");
    Serial.print(p.x);
    Serial.print("\tY = ");
    Serial.print(p.y);
    Serial.print(" -> ");
    #endif

// flip it around to match the screen.
#if ROTATION == 3
    int y = map(p.x, 0, 480, 0, tft.height());
    p.x = map(p.y, 768, 0, 0, tft.width());
    p.y = y;
#else
    p.x = map(p.x, 0, 240, 240, 0);
    p.y = map(p.y, 0, 320, 320, 0);
#endif
    // Print out the remapped (rotated) coordinates
    #ifdef SHOW_TOUCH_INFO
    Serial.print("(");
    Serial.print(p.x);
    Serial.print(", ");
    Serial.print(p.y);
    Serial.println(")");
    #endif

    if (p.y < BOXSIZE) {
        oldcolor = currentcolor;

        if (p.x < BOXSIZE) {
            currentcolor = NT35510_RED;
            tft.drawRect(0, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
        } else if (p.x < BOXSIZE * 2) {
            currentcolor = NT35510_YELLOW;
            tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
        } else if (p.x < BOXSIZE * 3) {
            currentcolor = NT35510_GREEN;
            tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
        } else if (p.x < BOXSIZE * 4) {
            currentcolor = NT35510_CYAN;
            tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
        } else if (p.x < BOXSIZE * 5) {
            currentcolor = NT35510_BLUE;
            tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
        } else if (p.x <= BOXSIZE * 6) {
            currentcolor = NT35510_MAGENTA;
            tft.drawRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, NT35510_WHITE);
        }

        if (oldcolor != currentcolor) {
            if (oldcolor == NT35510_RED)
                tft.fillRect(0, 0, BOXSIZE, BOXSIZE, NT35510_RED);
            if (oldcolor == NT35510_YELLOW)
                tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, NT35510_YELLOW);
            if (oldcolor == NT35510_GREEN)
                tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, NT35510_GREEN);
            if (oldcolor == NT35510_CYAN)
                tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, NT35510_CYAN);
            if (oldcolor == NT35510_BLUE)
                tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, NT35510_BLUE);
            if (oldcolor == NT35510_MAGENTA)
                tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, NT35510_MAGENTA);
        }
    }
    if (((p.y - PENRADIUS) > BOXSIZE) && ((p.y + PENRADIUS) < tft.height())) {
        tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
}