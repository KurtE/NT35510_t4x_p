# NT35510_t4x_p
## A basic display driver for NT35510 based displays on a Teensy 4.x(IMXRT) boards 

>[!WARNING]
>This is an experimental library, currently a WIP. I cannot guarantee that all functions will work 
nor can I guarantee that this library will work with other libraries. Use at your own risk

This library can communicate with an NT35510 based TFT LCD via a parallel interface (8080).
I am currently experimenting with it using 8-, 16-, and 18-bit data bus. 

This driver uses FlexIO to talk to the device, on some Teensy boards, asynchronous updates
are done using DMA, on other using interrupts.  This depends on which FlexIO object that
is used.  FlexIO1 and 2 supports DMA, whereas FelexIO3 does not

It utilizes FlexIO and DMA to write data to the screen while offloading the task from the CPU.

## Valid FlexIO pins
The driver requires that all the data pins (either 8 or 16 or 18) must be consecutive
FlexIO pins with a minor exception for the T4 described later, in addition to this it
requires two additional FlexIO pins on the same FlexIOobject that is used for Read(RD)
and Write(WR)

**WARNING:** As I am experimenting with different bus widths and data widths and the like,
it, is very possible that I will probably break things, like support for different boards
and pins.

For each of the boards I will show the output from the FlexIO_pin_list example sketch
with the FlexIO object pins list

### Teensy 4.1 
Uses FlexIO3 which does not have DMA support.
```
FlexIO3:
	Pin Order: Pin:flex
		 7:17 8:16 14:2 15:3 16:7 17:6 18:1 19:0 20:10 21:11 22:8 23:9 26:14 27:15 34:29 35:28 36:18 37:19 38:12 39:13 40:4 41:5
	Flex Pin Order: flex:pin
		 0:19 1:18 2:14 3:15 4:40 5:41 6:17 7:16 8:22 9:23 10:20 11:21 12:38 13:39 14:26 15:27 16:8 17:7 18:36 19:37 28:35 29:34
```
This board should support all 3 bus widths:  The default pins for 8-bit bus that have been setup are:
	Data pins: 19 18 14 15 40 41 17 16 WR:36 RD:37
 	WR: 36 RD: 37
For 16-bit mode it adds in the data pins:  22 23 20 21 38 39 26 27
and 18-bit mode adds: 8 and 7

Again, this is the defaults that I setup in the NT35510_t4x_p_default_FlexIO_pins.h file,
You can choose to update this to start with a different first data pin, which will dictate the others:
For example, you could choose data pin 0 to be pin 40, in which case D0-D7 would be: 40 41 17 16 22 23 20 21

### Teensy 4
Uses FlexIO3 which does not have DMA support.

The Teensy 4 does not have any FlexIO object with 8 consecutive FlexIO pins.  The closest it has is FlexIO3 with two groups
of 4 pins, with a gap of 2 between the two groups.  So special code that acts like there are 10 FlexIO pins, and it
updates each byte output or input to convert 8 bits to 10 or 10 bits to 8.
The only valid pins for this are:

```
// BUGBUG Nibble mode
#define DISPLAY_RD 20	// FlexIO3:10: RD
#define DISPLAY_WR 21	// FlexIO3:11 WR

#define DISPLAY_D0 19	 // FlexIO3:0 D0
#define DISPLAY_D1 18	 // FlexIO3:1 |
#define DISPLAY_D2 14	 // FlexIO3:2 |
#define DISPLAY_D3 15	 // FlexIO3:3 |
#define DISPLAY_D4 17	 // FlexIO3:6 |
#define DISPLAY_D5 16	 // FlexIO3:7 |
#define DISPLAY_D6 22	 // FlexIO3:8 |
#define DISPLAY_D7 23	 // FlexIO3:9 D7
```

### Teensy MicroMod
Support on FlexIO2 which does have DMA support. 

```
FlexIO2:
	Pin Order: Pin:flex
		 6:10 7:17 8:16 9:11 10:0 11:2 12:1 13:3 32:12 40:4 41:5 42:6 43:7 44:8 45:9
	Flex Pin Order: flex:in
		 0:10 1:12 2:11 3:13 4:40 5:41 6:42 7:43 8:44 9:45 10:6 11:9 12:32 16:8 17:7
```

As you can see there are only enough FlexIO pins to support the 8-bit buss. 
The currently defined default pins are:
```
#define DISPLAY_WR 7  //		 B1_01  FLEXIO2:17
#define DISPLAY_RD 8  // 		 B1_00  FLEXIO2:16

#define DISPLAY_D0 40 // 40      B0_04   FlexIO2:4
#define DISPLAY_D1 41 // 41      B0_05   FlexIO2:5
#define DISPLAY_D2 42 // 42      B0_06   FlexIO2:6
#define DISPLAY_D3 43 // 43      B0_07   FlexIO2:7
#define DISPLAY_D4 44 // 44      B0_08   FlexIO2:8
#define DISPLAY_D5 45 // 45      B0_09   FlexIO2:9
#define DISPLAY_D6 6  // 6       B0_10   FlexIO2:10
#define DISPLAY_D7 9  // 9       B0_11   FlexIO2:11
```

### experimental boards DEVBRD 4 and DEVBRD 4.5
These are setup using my modified Teensy core that added some variant support,
and the variant DevBoard 4.0.  Note the variant is based off Micromod

We are using FlexIO2, but could also use FleIO3
```
FlexIO2:
	Pin Order: Pin:flex
		 6:10 7:17 8:16 9:11 10:0 11:2 12:1 13:3 32:12 40:4 41:5 42:6 43:7 44:8 45:9 46:12 47:14 48:15 49:18 50:19 51:20 52:21 53:22 54:23 55:24 56:25 57:26 58:27 59:28 60:29 61:30 62:31 63:13
	Flex Pin Order: flex:in
		 0:10 1:12 2:11 3:13 4:40 5:41 6:42 7:43 8:44 9:45 10:6 11:9 12:32 13:63 14:47 15:48 16:8 17:7 18:49 19:50 20:51 21:52 22:53 23:54 24:55 25:56 26:57 27:58 28:59 29:60 30:61 31:62
FlexIO3:
	Pin Order: Pin:flex
		 7:17 8:16 14:2 15:3 16:7 17:6 18:1 19:0 20:10 21:11 22:8 23:9 26:14 27:15 49:18 50:19 51:20 52:21 53:22 54:23 55:24 56:25 57:26 58:27 59:28 60:29 61:30 62:31
	Flex Pin Order: flex:in
		 0:19 1:18 2:14 3:15 6:17 7:16 8:22 9:23 10:20 11:21 14:26 15:27 16:8 17:7 18:49 19:50 20:51 21:52 22:53 23:54 24:55 25:56 26:57 27:58 28:59 29:60 30:61 31:62
```
Default pins:

```
#define DISPLAY_WR 54 // 7  B1_07// FLEXIO2:24
#define DISPLAY_RD 53 // 8  B1_06 // FLEXIO2:23

#define DISPLAY_D0 40 // 40      B0_04   FlexIO2:4
#define DISPLAY_D1 41 // 41      B0_05   FlexIO2:5
#define DISPLAY_D2 42 // 42      B0_06   FlexIO2:6
#define DISPLAY_D3 43 // 43      B0_07   FlexIO2:7
#define DISPLAY_D4 44 // 44      B0_08   FlexIO2:8
#define DISPLAY_D5 45 // 45      B0_09   FlexIO2:9
#define DISPLAY_D6 6  // 6       B0_10   FlexIO2:10
#define DISPLAY_D7 9  // 9       B0_11   FlexIO2:11

#define DISPLAY_D8 32	//		 B0_12 FlexIO2:12 D8
#define DISPLAY_D9 63	//		 B0_13 FlexIO2:13  |
#define DISPLAY_D10 47	//		 B0_14 FlexIO2:14 |
#define DISPLAY_D11 48	//		 B0_15 FlexIO2:15 |
#define DISPLAY_D12 8	//		 B1_00 FlexIO2:16 |
#define DISPLAY_D13 7	//		 B1_01 FlexIO2:17 |
#define DISPLAY_D14 49	//		 B0_02 FlexIO2:18 |
#define DISPLAY_D15 50	//		 B0_03 FlexIO2:19 D15

#define DISPLAY_D16 51	//		 FlexIO2:20  // screwed up with RD pin...
#define DISPLAY_D17 52	//		 FlexIO2:21  // And CS pin

```

### experimental boards DEVBRD 5
Setup using my modified Teensy core that added some variant support,
and the variant DevBoard 4.0.  Note the variant is based off Micromod
Can use FlexIO2 (DMA) or FlexIO3 (INT), we are using 2

```
FlexIO2:
	Pin Order: Pin:flex
		 6:10 7:17 8:16 9:11 10:0 11:2 12:1 13:3 32:12 40:4 41:5 42:6 43:7 44:8 45:9 46:12 47:13 48:14 49:15 50:18 51:19 52:20 53:21 54:22 55:23 56:24 57:25 58:26 59:27 60:28 61:29 62:30 63:31
	Flex Pin Order: flex:in
		 0:10 1:12 2:11 3:13 4:40 5:41 6:42 7:43 8:44 9:45 10:6 11:9 12:32 13:47 14:48 15:49 16:8 17:7 18:50 19:51 20:52 21:53 22:54 23:55 24:56 25:57 26:58 27:59 28:60 29:61 30:62 31:63
FlexIO3:
	Pin Order: Pin:flex
		 7:17 8:16 14:2 15:3 16:7 17:6 18:1 19:0 20:10 21:11 22:8 23:9 26:14 27:15 50:18 51:19 52:20 53:21 54:22 55:23 56:24 57:25 58:26 59:27 60:28 61:29 62:30 63:31 64:4 65:5 66:12 67:13
	Flex Pin Order: flex:in
		 0:19 1:18 2:14 3:15 4:64 5:65 6:17 7:16 8:22 9:23 10:20 11:21 12:66 13:67 14:26 15:27 16:8 17:7 18:50 19:51 20:52 21:53 22:54 23:55 24:56 25:57 26:58 27:59 28:60 29:61 30:62 31:63
Flex IO Pin ranges:
	FlexIO1: 4-8
	FlexIO2: 0-31
	FlexIO3: 0-31
```

The current default pins (used by my shield for DB5)
```
#define DISPLAY_RD 52	// FlexIO2:20: RD
#define DISPLAY_WR 56	// FlexIO2:24 WR

#define DISPLAY_D0 40 // 40      B0_04   FlexIO2:4
#define DISPLAY_D1 41 // 41      B0_05   FlexIO2:5
#define DISPLAY_D2 42 // 42      B0_06   FlexIO2:6
#define DISPLAY_D3 43 // 43      B0_07   FlexIO2:7
#define DISPLAY_D4 44 // 44      B0_08   FlexIO2:8
#define DISPLAY_D5 45 // 45      B0_09   FlexIO2:9
#define DISPLAY_D6 6  // 6       B0_10   FlexIO2:10
#define DISPLAY_D7 9  // 9       B0_11   FlexIO2:11

#define DISPLAY_D8 32	//		 B0_12 FlexIO2:12 D8
#define DISPLAY_D9 47	//		 B0_13 FlexIO2:13  |
#define DISPLAY_D10 48	//		 B0_14 FlexIO2:14 |
#define DISPLAY_D11 49	//		 B0_15 FlexIO2:15 |
#define DISPLAY_D12 8	//		 B1_00 FlexIO2:16 |
#define DISPLAY_D13 7	//		 B1_01 FlexIO2:17 |
#define DISPLAY_D14 50	//		 B0_02 FlexIO2:18 |
#define DISPLAY_D15 51	//		 B0_03 FlexIO2:19 D15

// 
#define DISPLAY_D16 52	//		 FlexIO2:20  // screwed up with RD pin...
#define DISPLAY_D17 53	//		 FlexIO2:21  // And CS pin

```
Note: this pinout is screwed up for 18-bit mode as I choose to put RD/WR on the
logical pins for D16 and D17, So in this case I mucked up the setup, with
stacked header and jumpers and moved RD to 55... 


## Include files and Constructor

First include the library and create a constructor:
```
#include "NT35510_t4x_p.h"
#define CS 5
#define DC 4
#define RST 3
NT35510_t4x_p lcd = NT35510_t4x_p(DC,CS,RST);
```

## Begin and calls to setup different for buss width and data width
   
in the setup function call:
```
NT35510_t4x_p::begin();
```
The default baud rate is 20Mhz

### more to come

This library has most of the graphic primitives that we have in most of our SPI based tft drivers such as
my ILI9341_t3n library.
<More updates to come>
