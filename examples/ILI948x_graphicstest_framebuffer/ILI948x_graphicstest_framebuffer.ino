
#include <Adafruit_GFX.h>
#include <Teensy_Parallel_GFX.h>

#include "NT35510_t4x_p.h"

#ifdef ARDUINO_TEENSY41
NT35510_t4x_p lcd = NT35510_t4x_p(10, 8, 9);  //(dc, cs, rst)
#else
NT35510_t4x_p lcd = NT35510_t4x_p(4, 5, 3);  //(dc, cs, rst)
#endif

#define CENTER Teensy_Parallel_GFX::CENTER

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000)  {};
  delay(1000);
  if (CrashReport)Serial.print(CrashReport);
  Serial.println("Graphic Test - with frame buffer");
  /*
   * begin(Dispalay type, baud)
   * Display type is associated with the the diplay
   * init configurations:
   * ILI9588, ILI9486, ILI9481_1, ILI9481_2, R61529
   * Baud can be as defined in the readme.
   * begin defaults to ILI9488 and 20Mhz:
   *     lcd.begin();
  */
  lcd.begin(ILI9486, 30);
  lcd.setBitDepth(16);
  lcd.setRotation(3);

  //lcd.displayInfo();

  delay(1000);

  lcd.useFrameBuffer(true);

  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(600);
  
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(1000);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(NT35510_CYAN));
  delay(1000);

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(NT35510_RED, NT35510_BLUE));
  lcd.updateScreen();
  delay(1000);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(NT35510_GREEN));
  lcd.updateScreen();
  delay(1000);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(NT35510_YELLOW, NT35510_MAGENTA));
  lcd.updateScreen();
  delay(1000);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, NT35510_MAGENTA));

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, NT35510_WHITE));
  lcd.updateScreen();
  delay(500);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  lcd.updateScreen();
  delay(1000);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  lcd.updateScreen();
  delay(1000);

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  lcd.updateScreen();
  delay(1000);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  lcd.updateScreen();
  delay(1000);

  Serial.println(F("Done!"));
}

void loop() {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    lcd.fillScreen(NT35510_BLACK);
    lcd.setRotation(rotation);
    testText();
    lcd.updateScreen();
    delay(1000);
  }
}


unsigned long testText() {
  lcd.fillScreen(NT35510_BLACK);
  unsigned long start = micros();
  lcd.setCursor(0, 0);
  lcd.setTextColor(NT35510_WHITE);  lcd.setTextSize(1);
  lcd.println("Hello World!");
  lcd.setTextColor(NT35510_YELLOW); lcd.setTextSize(2);
  lcd.println(1234.56);
  lcd.setTextColor(NT35510_RED);    lcd.setTextSize(3);
  lcd.println(0xDEADBEEF, HEX);
  lcd.println();
  lcd.setTextColor(NT35510_GREEN);
  lcd.setTextSize(5);
  lcd.println("Groop");
  lcd.setTextSize(2);
  lcd.println("I implore thee,");
  lcd.setTextSize(1);
  lcd.println("my foonting turlingdromes.");
  lcd.println("And hooptiously drangle me");
  lcd.println("with crinkly bindlewurdles,");
  lcd.println("Or I will rend thee");
  lcd.println("in the gobberwarts");
  lcd.println("with my blurglecruncheon,");
  lcd.println("see if I don't!");
  lcd.updateScreen();
  return micros() - start;
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  lcd.fillScreen(NT35510_BLACK);
  lcd.updateScreen();
  lcd.fillScreen(NT35510_RED);
  lcd.updateScreen();
  lcd.fillScreen(NT35510_GREEN);
  lcd.updateScreen();
  lcd.fillScreen(NT35510_BLUE);
  lcd.updateScreen();
  lcd.fillScreen(NT35510_BLACK);
  lcd.updateScreen();
  return (micros() - start);
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = lcd.width(),
                h = lcd.height();

  lcd.fillScreen(NT35510_BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing
  lcd.updateScreen();
  delay(500);
  lcd.fillScreen(NT35510_BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;
  lcd.updateScreen();
  delay(500);

  lcd.fillScreen(NT35510_BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;
  lcd.updateScreen();
  delay(500);

  lcd.fillScreen(NT35510_BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) lcd.drawLine(x1, y1, x2, y2, color);
  lcd.updateScreen();
  t    += micros() - start;
  //lcd.updateScreen();

  return t;
}


unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = lcd.width(), h = lcd.height();

  lcd.fillScreen(NT35510_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) lcd.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) lcd.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = lcd.width()  / 2,
                cy = lcd.height() / 2;

  lcd.fillScreen(NT35510_BLACK);
  n     = min(lcd.width(), lcd.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    lcd.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}


unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = lcd.width()  / 2 - 1,
                cy = lcd.height() / 2 - 1;

  lcd.fillScreen(NT35510_BLACK);
  n = min(lcd.width(), lcd.height()) - 1;
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    lcd.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    lcd.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  return t;
}



unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = lcd.width(), h = lcd.height(), r2 = radius * 2;

  lcd.fillScreen(NT35510_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      lcd.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = lcd.width()  + radius,
                h = lcd.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      lcd.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}



unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = lcd.width()  / 2 - 1,
                      cy = lcd.height() / 2 - 1;

  lcd.fillScreen(NT35510_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    lcd.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      lcd.color565(0, 0, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = lcd.width()  / 2 - 1,
                   cy = lcd.height() / 2 - 1;

  lcd.fillScreen(NT35510_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    lcd.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      lcd.color565(0, i, i));
    t += micros() - start;
    lcd.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      lcd.color565(i, i, 0));
  }

  return t;
}


unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = lcd.width()  / 2 - 1,
                cy = lcd.height() / 2 - 1;

  lcd.fillScreen(NT35510_BLACK);
  w     = min(lcd.width(), lcd.height()) - 1;
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    lcd.drawRoundRect(cx-i2, cy-i2, i, i, i/8, lcd.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = lcd.width()  / 2 - 1,
                cy = lcd.height() / 2 - 1;

  lcd.fillScreen(NT35510_BLACK);
  start = micros();
  for(i=min(lcd.width(), lcd.height()) - 1; i>20; i-=6) {
    i2 = i / 2;
    lcd.fillRoundRect(cx-i2, cy-i2, i, i, i/8, lcd.color565(0, i, 0));
  }

  return micros() - start;
}

