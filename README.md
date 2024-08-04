# NT35510_t4x_p
## A basic display driver for NT35510 based displays on a Teensy 4.x(IMXRT) boards 

**Disclaimer: This is an experimental library, currently a WIP. I cannot guarantee that all functions will work nor can I guarantee that this library will work with other libraries. Use at your own risk**  

This library can communicate with an NT35510 based TFT LCD via an 8 bit parallel interface (8080)
It utilizes FlexIO and DMA to write data to the screen while offloading the task from the CPU.

First include the library and create a constructor:
```
#include "NT35510_t4x_p.h"
#define CS 5
#define DC 4
#define RST 3
NT35510_t4x_p lcd = NT35510_t4x_p(DC,CS,RST);
```
You can use and GPIO pins for CS, DC and RST

Next, wire up your LCD - use Teensy pins:
Note: The are FLEXIO 2 pins on the Micromod The flexio pins shown in ()
* pin 10 - WR (0)
* pin 12 - RD (1)
* pin 40 - D0 (4)
* pin 41 - D1 (5)
* pin 42 - D2 (6)
* pin 43 - D3 (7)
* pin 44 - D4 (8)
* pin 45 - D5 (9)
* pin 6 - D6 (10)
* pin 9 - D7 (11)
   
in the setup function call:
```
NT35510_t4x_p::begin();
```
The default baud rate is 20Mhz

This library has most of the graphic primitives that we have in most of our SPI based tft drivers such as
my ILI9341_t3n library.
<More updates updates to come>
