//=============================================================================
// Default flexio pins - Can be setup for different teensy 4.x boards
//=============================================================================

#if defined(ARDUINO_TEENSY41)
// FlexIO pins: data: 19 18 14 15 40 41 17 16 WR:36 RD:37
//  FlexIO3
#define DISPLAY_WR 36
#define DISPLAY_RD 37

#define DISPLAY_D0 19
#define DISPLAY_D1 18
#define DISPLAY_D2 14
#define DISPLAY_D3 15
#define DISPLAY_D4 40
#define DISPLAY_D5 41
#define DISPLAY_D6 17
#define DISPLAY_D7 16

#define DISPLAY_D8 22
#define DISPLAY_D9 23
#define DISPLAY_D10 20
#define DISPLAY_D11 21
#define DISPLAY_D12 38
#define DISPLAY_D13 39
#define DISPLAY_D14 26
#define DISPLAY_D15 27


#elif defined(ARDUINO_TEENSY40)
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

#elif defined(ARDUINO_TEENSY_DEVBRD4)
// FLEXIO2 pins.

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

#elif defined(ARDUINO_TEENSY_DEVBRD5)
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


#else // #if defined(ARDUINO_TEENSY_MICROMOD)
// FLEXIO2 pins.

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
#endif