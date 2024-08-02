# NT35510_t4x_p
## A basic display driver for HX8357X series on a Teensy 4.1 and Micromod 

**Disclaimer: This is an experimental library, currently a WIP. I cannot guarantee that all functions will work nor can I guarantee that this library will work with other libraries. Use at your own risk**  

This library can communicate (TX only at the moment) with an ILI9488/9486/9481 TFT LCD via an 8 bit parallel interface (8080)
It utilizes FlexIO and DMA to write data to the screen while offloading the task from the CPU.
It can only write an image array at the moment with defined start/end coordinates.
The default bus speed is set to 12Mhz and can be lowered or raised with a simple function call.

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

In the begin(n) function you can pass 2,4,8,12,20,24, 30 and 40 to lower or raise the baud rate.


Call the following function for a polling method write:
```
NT35510_t4x_p::pushPixels16bit(flexio_teensy_mm,0,0,480,320);
```
or call the following function for an async DMA write
```
NT35510_t4x_p::pushPixels16bitDMA(flexio_teensy_mm,0,0,480,320);
```
to push the image data, the arguments are as follows:
* uint16_t color array (RGB565)
* uint16_t x1
* uint16_t y1
* uint16_t x2
* uint16_t y2

Additional API's:


Set rotation: 1,2,3,4
```
NT35510_t4x_p::setRotation(n);
```

Invert display color (true/false)
```
NT35510_t4x_p::invertDisplay(bool);
```

Register a callback to trigger when the DMA transfer completes - ONLY ON DMA METHOD
```
NT35510_t4x_p::onCompleteCB(CBF callback);
```
![Image of TFT with Teensy MM image](https://github.com/david-res/NT35510_t4x_p/blob/master/mm_flexio_example.jpg)

