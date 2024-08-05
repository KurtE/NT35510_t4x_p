#include "NT35510_t4x_p.h"
#include "NT35510_t4x_p_default_flexio_pins.h"


#if !defined(__IMXRT1062__)
#warning This library only supports the Teensy 4.x
#endif

//#define DEBUG
#define DEBUG_VERBOSE

#ifndef DEBUG
#undef DEBUG_VERBOSE
void inline DBGPrintf(...){};
void inline DBGWrite(uint8_t ch) {};
void inline DBGFlush(){};
#else
#define DBGPrintf Serial.printf
#define DBGFlush Serial.flush
#define DBGWrite Serial.write
#endif

#ifndef DEBUG_VERBOSE
void inline VDBGPrintf(...){};
#else
#define VDBGPrintf Serial.printf
#endif



//--------------------------------------------------

FLASHMEM NT35510_t4x_p::NT35510_t4x_p(int8_t dc, int8_t cs, int8_t rst)
    : Teensy_Parallel_GFX(_TFTWIDTH, _TFTHEIGHT), _dc(dc), _cs(cs), _rst(rst),
      _data_pins{DISPLAY_D0, DISPLAY_D1, DISPLAY_D2, DISPLAY_D3, DISPLAY_D4, DISPLAY_D5, DISPLAY_D6, DISPLAY_D7},
      _wr_pin(DISPLAY_WR), _rd_pin(DISPLAY_RD) {
}

FLASHMEM void NT35510_t4x_p::begin(uint8_t display_name, uint8_t baud_speed_mhz) {
    // DBGPrintf("Bus speed: %d Mhz \n", baud_speed_mhz);

    _display_name = display_name;

    switch (baud_speed_mhz) {
    case 2:
        _baud_div = 120;
        break;
    case 4:
        _baud_div = 60;
        break;
    case 8:
        _baud_div = 30;
        break;
    case 12:
        _baud_div = 20;
        break;
    case 20:
        _baud_div = 12;
        break;
    case 24:
        _baud_div = 10;
        break;
    case 30:
        _baud_div = 8;
        break;
    case 40:
        _baud_div = 6;
        break;
    default:
        _baud_div = 20; // 12Mhz
        break;
    }
    DBGPrintf("Bus speed: %d Mhz Div: %d\n", baud_speed_mhz, _baud_div);
    pinMode(_cs, OUTPUT);  // CS
    pinMode(_dc, OUTPUT);  // DC
    pinMode(_rst, OUTPUT); // RST

    *(portControlRegister(_cs)) = 0xFF;
    *(portControlRegister(_dc)) = 0xFF;
    *(portControlRegister(_rst)) = 0xFF;

    digitalWriteFast(_cs, HIGH);
    digitalWriteFast(_dc, HIGH);
    digitalWriteFast(_rst, HIGH);

    delay(200);
    digitalWrite(_rst, LOW);
    delay(800);
    digitalWriteFast(_rst, HIGH);
    delay(800);

    FlexIO_Init();

    displayInit(display_name);

    setBitDepth(_bitDepth);
    /*
    setTearingEffect(_bTearingOn);
    if (_bTearingOn == true) {
      setTearingScanLine(_tearingScanLine);
    }
    setFrameRate(_frameRate);
    */

    _width = _TFTWIDTH;
    _height = _TFTHEIGHT;

    setClipRect();
    setOrigin();
    setTextSize(1);
}

FLASHMEM uint8_t NT35510_t4x_p::setBitDepth(uint8_t bitDepth) {
    uint8_t bd;

    switch (bitDepth) {
    case 16:
        _bitDepth = 16;
        bd = 0x55;
        break;
    case 18:
        _bitDepth = 18;
        bd = 0x66;
        break;
    case 24: // Unsupported
        _bitDepth = 24;
        bd = 0x77;
        break;
    default: // Unsupported
        return _bitDepth;
        break;
    }

    //SglBeatWR_nPrm_8(NT35510_COLMOD, &bd, 1);
    write_command_and_data(NT35510_COLMOD, bd);

    // Insert small delay here as rapid calls appear to fail
    delay(10);

    return _bitDepth;
}

FLASHMEM uint8_t NT35510_t4x_p::getBitDepth() {
    return _bitDepth;
}

FLASHMEM void NT35510_t4x_p::setFrameRate(uint8_t frRate) {
    _frameRate = frRate;
#ifdef LATER

    uint8_t fr28Hz[2] = {0x00, 0x11}; // 28.78fps, 17 clocks
    uint8_t fr30Hz[2] = {0x10, 0x11}; // 30.38fps, 17 clocks
    uint8_t fr39Hz[2] = {0x50, 0x11}; // 39.06fps, 17 clocks
    uint8_t fr45Hz[2] = {0x70, 0x11}; // 45.57fps, 17 clocks
    uint8_t fr54Hz[2] = {0x90, 0x11}; // 54.69ps, 17 clocks
    uint8_t fr60Hz[2] = {0xA0, 0x11}; // 60.76fps, 17 clocks
    uint8_t fr68Hz[2] = {0xB0, 0x11}; // 68.36fps, 17 clocks (ILI9488 default)
    uint8_t fr78Hz[2] = {0xC0, 0x11}; // 78.13fps, 17 clocks
    uint8_t fr91Hz[2] = {0xD0, 0x11}; // 91.15fps, 17 clocks

    uint8_t frData[2];
    // Select parameters for frame rate
    switch (frRate) {
    case 28:
        memcpy(frData, fr28Hz, sizeof fr28Hz);
        break;
    case 30:
        memcpy(frData, fr30Hz, sizeof fr30Hz);
        break;
    case 39:
        memcpy(frData, fr39Hz, sizeof fr39Hz);
        break;
    case 45:
        memcpy(frData, fr45Hz, sizeof fr45Hz);
        break;
    case 54:
        memcpy(frData, fr54Hz, sizeof fr54Hz);
        break;
    case 60:
        memcpy(frData, fr60Hz, sizeof fr60Hz);
        break;
    case 68:
        memcpy(frData, fr68Hz, sizeof fr68Hz);
        break;
    case 78:
        memcpy(frData, fr78Hz, sizeof fr78Hz);
        break;
    case 91:
        memcpy(frData, fr91Hz, sizeof fr91Hz);
        break;
    default:
        memcpy(frData, fr60Hz, sizeof fr60Hz);
        _frameRate = 60;
        break;
    }

    SglBeatWR_nPrm_8(NT35510_FRMCTR1, frData, 2);
#endif
}

FLASHMEM uint8_t NT35510_t4x_p::getFrameRate() {
    return _frameRate;
}

FLASHMEM void NT35510_t4x_p::setTearingEffect(bool tearingOn) {

#ifdef LATER
    _bTearingOn = tearingOn;
    uint8_t mode = 0x00;

    CSLow();
    if (_bTearingOn == true) {
        SglBeatWR_nPrm_8(NT35510_TEON, &mode, 1); // Tearing effect line on, mode 0 (V-Blanking)
    } else {
        SglBeatWR_nPrm_8(NT35510_TEOFF, 0, 0);
    }
    CSHigh();
#endif    
}

FLASHMEM bool NT35510_t4x_p::getTearingEffect() {
    return _bTearingOn;
}

FLASHMEM void NT35510_t4x_p::setTearingScanLine(uint16_t scanLine) {
#ifdef LATER
    _tearingScanLine = scanLine;

    uint8_t params[2] = {(uint8_t)(_tearingScanLine << 8), (uint8_t)(_tearingScanLine & 0xFF)};
    SglBeatWR_nPrm_8(NT35510_TESLWR, params, 2); // Tearing effect write scan line : 0x00 0x00 = line 0 (default), 0x00 0xA0 = line 160, 0x00 0xF0 = line 240
#endif    
}

FLASHMEM uint16_t NT35510_t4x_p::getTearingScanLine() {
    return _tearingScanLine;
}

FLASHMEM void NT35510_t4x_p::setRotation(uint8_t r) {
    _rotation = r & 3;
    uint8_t m = 0;  // MADCTL_BGR ?
    switch(_rotation) {
        case 0:
            //m       = /* MADCTL_BGR */;
            _width  = _TFTWIDTH;
            _height = _TFTHEIGHT;
            break;
        case 1:
            m      |= (MADCTL_MV | MADCTL_MX);
            _width  = _TFTHEIGHT;
            _height = _TFTWIDTH;
            break;
        case 2:
            m       |= (MADCTL_MX | MADCTL_MY);
            _width  = _TFTWIDTH;
            _height = _TFTHEIGHT;
            break;
        case 3:
            m       |= (MADCTL_MV | MADCTL_MY);
            _width  = _TFTHEIGHT;
            _height = _TFTWIDTH;
            break;

    }

    setClipRect();
    setOrigin();

    cursor_x = 0;
    cursor_y = 0;
    //uint8_t  am[2] = {0, m};
    //SglBeatWR_nPrm_8(NT35510_MADCTL, am, 2);
    write_command_and_data(NT35510_MADCTL, m);
}

void NT35510_t4x_p::setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    //Serial.printf("setAddr(%d, %d, %d, %d)", x0, y0, x1, y1);            

    if ((x0 != _previous_addr_x0) || (x1 != _previous_addr_x1)) {
        write_command_and_data(NT35510_CASET + 0, x0 >> 8U);
        write_command_and_data(NT35510_CASET + 1, x0 & 0xFF);
        write_command_and_data(NT35510_CASET + 2, x1 >> 8U);
        write_command_and_data(NT35510_CASET + 3, x1 & 0xFF);
        _previous_addr_x0 = x0;
        _previous_addr_x1 = x1;
    }
    if ((y0 != _previous_addr_y0) || (y1 != _previous_addr_y1)) {
        write_command_and_data(NT35510_RASET + 0, y0 >> 8U);
        write_command_and_data(NT35510_RASET + 1, y0 & 0xFF);
        write_command_and_data(NT35510_RASET + 2, y1 >> 8U);
        write_command_and_data(NT35510_RASET + 3, y1 & 0xFF);
        _previous_addr_y0 = y0;
        _previous_addr_y1 = y1;
    }
}



FLASHMEM void NT35510_t4x_p::invertDisplay(bool invert) {
    SglBeatWR_nPrm_8(invert ? NT35510_INVON : NT35510_INVOFF, 0, 0);
}

void NT35510_t4x_p::setScroll(uint16_t offset) {
    //   SglBeatWR_nPrm_8(NT35510_VSCRSADD, offset, 1); // Changed, offset is
    //$$ SglBeatWR_nPrm_16(NT35510_VSCRSADD, &offset, 1); // a pointer to a 16 bit value.
}

FLASHMEM void NT35510_t4x_p::onCompleteCB(CBF callback) {
    _callback = callback;
    isCB = true;
}

FASTRUN void NT35510_t4x_p::displayInfo() {
    CSLow();
/*
    Serial.printf("Manufacturer ID: 0x%02X\n", readCommand(HX8357B_RDID1));
    Serial.printf("Module Version ID: 0x%02X\n", readCommand(HX8357B_RDID2));
    Serial.printf("Module ID: 0x%02X\n", readCommand(HX8357B_RDID3));
    Serial.printf("Display Power Mode: 0x%02X\n", readCommand(NT35510_RDMODE));
    Serial.printf("MADCTL Mode: 0x%02X\n", readCommand(NT35510_RDMADCTL));
    //Serial.printf("Pixel Format: 0x%02X\n", readCommand(NT35510_RDCOLMOD));
    Serial.printf("Image Format: 0x%02X\n", readCommand(NT35510_RDIMGFMT));
    //Serial.printf("Signal Mode: 0x%02X\n", readCommand(NT35510_RDDSM));
    uint8_t sdRes = readCommand(NT35510_RDSELFDIAG);
    Serial.printf("Self Diagnostic: %s (0x%02X)\n", sdRes == 0xc0 ? "OK" : "Failed", sdRes);
    Serial.printf("Device Information: %06X\n", readCommandN(NT35510_RDDID, 3));
    uint32_t device_status = readCommandN(NT35510_RDDST, 4);
    Serial.printf("Device Status: %08X\n", device_status);
    Serial.printf("\tOrder: %s\n", (device_status & (1 << 26)) ? "BGR" : "RGB");
    Serial.print("\tinterface pixel format: ");
    switch ((device_status >> 20) & 0x7) {
    case 0x5:
        Serial.println("16 bit");
        break;
    case 0x6:
        Serial.println("18 bit");
        break;
    case 0x7:
        Serial.println("24 bit");
        break;
    default:
        Serial.println("????");
    }
*/
    CSHigh();
}

FASTRUN void NT35510_t4x_p::pushPixels16bit(const uint16_t *pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }
    uint32_t area = (x2 - x1 + 1) * (y2 - y1 + 1);
    setAddr(x1, y1, x2, y2);
    SglBeatWR_nPrm_16(NT35510_RAMWR, pcolors, area);
}

FASTRUN void NT35510_t4x_p::pushPixels16bitDMA(const uint16_t *pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }
    uint32_t area = (x2 - x1 + 1) * (y2 - y1 + 1);
    setAddr(x1, y1, x2, y2);
    MulBeatWR_nPrm_DMA(NT35510_RAMWR, pcolors, area);
}

///////////////////
// Private functions
///////////////////
FLASHMEM void NT35510_t4x_p::displayInit(uint8_t disp_name) {
    Serial.print("displayInit called\n");

    // Start of hard coded like BuyDisplay code:

   //35510h 
#if 1
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }

    FlexIO_Config_SnglBeat();
    /* Assert CS, RS pins */

    // delay(1);
    CSLow();
  uint8_t ini01[] = {0x55, 0xAA, 0x52, 0x08, 0x01};
  writeRegM(0xF000, sizeof(ini01), ini01);
  uint8_t ini03[] = {0x34, 0x34, 0x34};
  writeRegM(0xB600, sizeof(ini03), ini03);
  uint8_t ini02[] = {0x0D, 0x0D, 0x0D};
  writeRegM(0xB000, sizeof(ini02), ini02); // AVDD Set AVDD 5.2V
  uint8_t ini05[] = {0x34, 0x34, 0x34};
  writeRegM(0xB700, sizeof(ini05), ini05); // AVEE ratio
  uint8_t ini04[] = {0x0D, 0x0D, 0x0D};
  writeRegM(0xB100, sizeof(ini04), ini04); // AVEE  -5.2V
  uint8_t ini07[] = {0x24, 0x24, 0x24};
  writeRegM(0xB800, sizeof(ini07), ini07); // VCL ratio
  uint8_t ini10[] = {0x34, 0x34, 0x34};
  writeRegM(0xB900, sizeof(ini10), ini10); // VGH  ratio
  uint8_t ini09[] = {0x0F, 0x0F, 0x0F};
  writeRegM(0xB300, sizeof(ini09), ini09);
  uint8_t ini14[] = {0x24, 0x24, 0x24};
  writeRegM(0xBA00, sizeof(ini14), ini14); // VGLX  ratio
  uint8_t ini12[] = {0x08, 0x08};
  writeRegM(0xB500, sizeof(ini12), ini12);
  uint8_t ini15[] = {0x00, 0x78, 0x00};
  writeRegM(0xBC00, sizeof(ini15), ini15); // VGMP/VGSP 4.5V/0V
  uint8_t ini16[] = {0x00, 0x78, 0x00};
  writeRegM(0xBD00, sizeof(ini16), ini16); // VGMN/VGSN -4.5V/0V
  uint8_t ini17[] = {0x00, 0x89};
  writeRegM(0xBE00, sizeof(ini17), ini17); // VCOM  -1.325V
  // Gamma Setting
  uint8_t ini20[] = {
      0x00, 0x2D, 0x00, 0x2E, 0x00, 0x32, 0x00, 0x44, 0x00, 0x53, 0x00, 0x88, 0x00, 0xB6, 0x00, 0xF3, 0x01, 0x22, 0x01, 0x64,
      0x01, 0x92, 0x01, 0xD4, 0x02, 0x07, 0x02, 0x08, 0x02, 0x34, 0x02, 0x5F, 0x02, 0x78, 0x02, 0x94, 0x02, 0xA6, 0x02, 0xBB,
      0x02, 0xCA, 0x02, 0xDB, 0x02, 0xE8, 0x02, 0xF9, 0x03, 0x1F, 0x03, 0x7F};
  writeRegM(0xD100, sizeof(ini20), ini20);
  writeRegM(0xD400, sizeof(ini20), ini20); // R+ R-
  writeRegM(0xD200, sizeof(ini20), ini20);
  writeRegM(0xD500, sizeof(ini20), ini20); // G+ G-
  writeRegM(0xD300, sizeof(ini20), ini20);
  writeRegM(0xD600, sizeof(ini20), ini20); // B+ B-
  //
  uint8_t ini21[] = {0x55, 0xAA, 0x52, 0x08, 0x00};
  writeRegM(0xF000, sizeof(ini21), ini21); // #Enable Page0
  uint8_t ini22[] = {0x08, 0x05, 0x02, 0x05, 0x02};
  writeRegM(0xB000, sizeof(ini22), ini22); // # RGB I/F Setting

  uint8_t ini23[] = {0x08};
  writeRegM(0xB600, sizeof(ini23), ini23); 
  
  uint8_t ini23a[] = {0x50};
  writeRegM(0xB500, sizeof(ini23a), ini23a); 
  uint8_t ini24[] = {0x00, 0x00};
  writeRegM(0xB700, sizeof(ini24), ini24); // ## Gate EQ:
  uint8_t ini25[] = {0x01, 0x05, 0x05, 0x05};
  writeRegM(0xB800, sizeof(ini25), ini25); // ## Source EQ:
  uint8_t ini26[] = {0x00, 0x00, 0x00};
  writeRegM(0xBC00, sizeof(ini26), ini26); // # Inversion: Column inversion (NVT)
  uint8_t ini27[] = {0x03, 0x00, 0x00};
  writeRegM(0xCC00, sizeof(ini27), ini27); // # BOE's Setting(default)
  uint8_t ini28[] = {0x01, 0x84, 0x07, 0x31, 0x00, 0x01};
  writeRegM(0xBD00, sizeof(ini28), ini28); // # Display Timing:
  //
  uint8_t ini30[] = {0xAA, 0x55, 0x25, 0x01};
  writeRegM(0xFF00, sizeof(ini30), ini30);

  uint8_t ini31[] = {0x00};
  writeRegM(NT35510_TEON, sizeof(ini31), ini31);
  uint8_t ini32[] = {0x55};
  writeRegM(NT35510_COLMOD, sizeof(ini32), ini32);

  output_command_helper(NT35510_SLPOUT);
  CSHigh();



#else        
    write_command_and_data(0xF000, 0x55);
    write_command_and_data(0xF001, 0xAA);
    write_command_and_data(0xF002, 0x52);
    write_command_and_data(0xF003, 0x08);
    write_command_and_data(0xF004, 0x01);
    
    //#AVDD Set AVDD 5.2V
    write_command_and_data(0xB000, 0x0D);
    write_command_and_data(0xB001, 0x0D);
    write_command_and_data(0xB002, 0x0D);
    
    //#AVDD ratio
    write_command_and_data(0xB600, 0x34);
    write_command_and_data(0xB601, 0x34);
    write_command_and_data(0xB602, 0x34);
     
    //#AVEE  -5.2V
    write_command_and_data(0xB100, 0x0D);
    write_command_and_data(0xB101, 0x0D);
    write_command_and_data(0xB102, 0x0D);
    
    //#AVEE ratio
    write_command_and_data(0xB700, 0x34);
    write_command_and_data(0xB701, 0x34);
    write_command_and_data(0xB702, 0x34);
    
    //#VCL  -2.5V
    write_command_and_data(0xB200, 0x00);
    write_command_and_data(0xB201, 0x00);
    write_command_and_data(0xB202, 0x00);
    
    //#VCL ratio
    write_command_and_data(0xB800, 0x24);
    write_command_and_data(0xB801, 0x24);
    write_command_and_data(0xB802, 0x24); 
    
    //#VGH  15V
    write_command_and_data(0xBF00, 0x01);
    write_command_and_data(0xB300, 0x0F);
    write_command_and_data(0xB301, 0x0F);
    write_command_and_data(0xB302, 0x0F);
    
    //#VGH  ratio
    write_command_and_data(0xB900, 0x34);
    write_command_and_data(0xB901, 0x34);
    write_command_and_data(0xB902, 0x34); 
    
    //#VGL_REG  -10V
    write_command_and_data(0xB500, 0x08);
    write_command_and_data(0xB500, 0x08);
    write_command_and_data(0xB501, 0x08);
    write_command_and_data(0xC200, 0x03);
    
    //#VGLX  ratio
    write_command_and_data(0xBA00, 0x24);
    write_command_and_data(0xBA01, 0x24);
    write_command_and_data(0xBA02, 0x24);
    
    //#VGMP/VGSP 4.5V/0V
    write_command_and_data(0xBC00, 0x00);
    write_command_and_data(0xBC01, 0x78);
    write_command_and_data(0xBC02, 0x00);
    
    //#VGMN/VGSN -4.5V/0V
    write_command_and_data(0xBD00, 0x00);
    write_command_and_data(0xBD01, 0x78);
    write_command_and_data(0xBD02, 0x00);
    
    //#VCOM  -1.325V
    write_command_and_data(0xBE00, 0x00);
    write_command_and_data(0xBE01, 0x89);//69
    
     //Gamma Setting     
    write_command_and_data(0xD100, 0x00);
    write_command_and_data(0xD101, 0x2D);
    write_command_and_data(0xD102, 0x00);
    write_command_and_data(0xD103, 0x2E);
    write_command_and_data(0xD104, 0x00); 
    write_command_and_data(0xD105, 0x32);
    write_command_and_data(0xD106, 0x00);
    write_command_and_data(0xD107, 0x44);
    write_command_and_data(0xD108, 0x00);
    write_command_and_data(0xD109, 0x53);
    write_command_and_data(0xD10A, 0x00);
    write_command_and_data(0xD10B, 0x88);
    write_command_and_data(0xD10C, 0x00);
    write_command_and_data(0xD10D, 0xB6);
    write_command_and_data(0xD10E, 0x00);
    write_command_and_data(0xD10F, 0xF3); //
    write_command_and_data(0xD110, 0x01);
    write_command_and_data(0xD111, 0x22);
    write_command_and_data(0xD112, 0x01);
    write_command_and_data(0xD113, 0x64);
    write_command_and_data(0xD114, 0x01);
    write_command_and_data(0xD115, 0x92);
    write_command_and_data(0xD116, 0x01);
    write_command_and_data(0xD117, 0xD4);
    write_command_and_data(0xD118, 0x02); 
    write_command_and_data(0xD119, 0x07);
    write_command_and_data(0xD11A, 0x02);
    write_command_and_data(0xD11B, 0x08);
    write_command_and_data(0xD11C, 0x02);
    write_command_and_data(0xD11D, 0x34);
    write_command_and_data(0xD11E, 0x02);
    write_command_and_data(0xD11F, 0x5F); //
    write_command_and_data(0xD120, 0x02);
    write_command_and_data(0xD121, 0x78);
    write_command_and_data(0xD122, 0x02);
    write_command_and_data(0xD123, 0x94);
    write_command_and_data(0xD124, 0x02);
    write_command_and_data(0xD125, 0xA6);
    write_command_and_data(0xD126, 0x02);
    write_command_and_data(0xD127, 0xBB);
    write_command_and_data(0xD128, 0x02); 
    write_command_and_data(0xD129, 0xCA);
    write_command_and_data(0xD12A, 0x02);
    write_command_and_data(0xD12B, 0xDB);
    write_command_and_data(0xD12C, 0x02);
    write_command_and_data(0xD12D, 0xE8);
    write_command_and_data(0xD12E, 0x02);
    write_command_and_data(0xD12F, 0xF9); //
    write_command_and_data(0xD130, 0x03); 
    write_command_and_data(0xD131, 0x1F);
    write_command_and_data(0xD132, 0x03);
    write_command_and_data(0xD133, 0x7F);
             
    write_command_and_data(0xD200, 0x00);
    write_command_and_data(0xD201, 0x2D);
    write_command_and_data(0xD202, 0x00);
    write_command_and_data(0xD203, 0x2E);
    write_command_and_data(0xD204, 0x00); 
    write_command_and_data(0xD205, 0x32);
    write_command_and_data(0xD206, 0x00);
    write_command_and_data(0xD207, 0x44);
    write_command_and_data(0xD208, 0x00);
    write_command_and_data(0xD209, 0x53);
    write_command_and_data(0xD20A, 0x00);
    write_command_and_data(0xD20B, 0x88);
    write_command_and_data(0xD20C, 0x00);
    write_command_and_data(0xD20D, 0xB6);
    write_command_and_data(0xD20E, 0x00);
    write_command_and_data(0xD20F, 0xF3); //
    write_command_and_data(0xD210, 0x01);
    write_command_and_data(0xD211, 0x22);
    write_command_and_data(0xD212, 0x01);
    write_command_and_data(0xD213, 0x64);
    write_command_and_data(0xD214, 0x01);
    write_command_and_data(0xD215, 0x92);
    write_command_and_data(0xD216, 0x01);
    write_command_and_data(0xD217, 0xD4);
    write_command_and_data(0xD218, 0x02); 
    write_command_and_data(0xD219, 0x07);
    write_command_and_data(0xD21A, 0x02);
    write_command_and_data(0xD21B, 0x08);
    write_command_and_data(0xD21C, 0x02);
    write_command_and_data(0xD21D, 0x34);
    write_command_and_data(0xD21E, 0x02);
    write_command_and_data(0xD21F, 0x5F); //
    write_command_and_data(0xD220, 0x02);
    write_command_and_data(0xD221, 0x78);
    write_command_and_data(0xD222, 0x02);
    write_command_and_data(0xD223, 0x94);
    write_command_and_data(0xD224, 0x02);
    write_command_and_data(0xD225, 0xA6);
    write_command_and_data(0xD226, 0x02);
    write_command_and_data(0xD227, 0xBB);
    write_command_and_data(0xD228, 0x02); 
    write_command_and_data(0xD229, 0xCA);
    write_command_and_data(0xD22A, 0x02);
    write_command_and_data(0xD22B, 0xDB);
    write_command_and_data(0xD22C, 0x02);
    write_command_and_data(0xD22D, 0xE8);
    write_command_and_data(0xD22E, 0x02);
    write_command_and_data(0xD22F, 0xF9); //
    write_command_and_data(0xD230, 0x03); 
    write_command_and_data(0xD231, 0x1F);
    write_command_and_data(0xD232, 0x03);
    write_command_and_data(0xD233, 0x7F);
         
    write_command_and_data(0xD300, 0x00);
    write_command_and_data(0xD301, 0x2D);
    write_command_and_data(0xD302, 0x00);
    write_command_and_data(0xD303, 0x2E);
    write_command_and_data(0xD304, 0x00); 
    write_command_and_data(0xD305, 0x32);
    write_command_and_data(0xD306, 0x00);
    write_command_and_data(0xD307, 0x44);
    write_command_and_data(0xD308, 0x00);
    write_command_and_data(0xD309, 0x53);
    write_command_and_data(0xD30A, 0x00);
    write_command_and_data(0xD30B, 0x88);
    write_command_and_data(0xD30C, 0x00);
    write_command_and_data(0xD30D, 0xB6);
    write_command_and_data(0xD30E, 0x00);
    write_command_and_data(0xD30F, 0xF3); //
    write_command_and_data(0xD310, 0x01);
    write_command_and_data(0xD311, 0x22);
    write_command_and_data(0xD312, 0x01);
    write_command_and_data(0xD313, 0x64);
    write_command_and_data(0xD314, 0x01);
    write_command_and_data(0xD315, 0x92);
    write_command_and_data(0xD316, 0x01);
    write_command_and_data(0xD317, 0xD4);
    write_command_and_data(0xD318, 0x02); 
    write_command_and_data(0xD319, 0x07);
    write_command_and_data(0xD31A, 0x02);
    write_command_and_data(0xD31B, 0x08);
    write_command_and_data(0xD31C, 0x02);
    write_command_and_data(0xD31D, 0x34);
    write_command_and_data(0xD31E, 0x02);
    write_command_and_data(0xD31F, 0x5F); //
    write_command_and_data(0xD320, 0x02);
    write_command_and_data(0xD321, 0x78);
    write_command_and_data(0xD322, 0x02);
    write_command_and_data(0xD323, 0x94);
    write_command_and_data(0xD324, 0x02);
    write_command_and_data(0xD325, 0xA6);
    write_command_and_data(0xD326, 0x02);
    write_command_and_data(0xD327, 0xBB);
    write_command_and_data(0xD328, 0x02); 
    write_command_and_data(0xD329, 0xCA);
    write_command_and_data(0xD32A, 0x02);
    write_command_and_data(0xD32B, 0xDB);
    write_command_and_data(0xD32C, 0x02);
    write_command_and_data(0xD32D, 0xE8);
    write_command_and_data(0xD32E, 0x02);
    write_command_and_data(0xD32F, 0xF9); //
    write_command_and_data(0xD330, 0x03); 
    write_command_and_data(0xD331, 0x1F);
    write_command_and_data(0xD332, 0x03);
    write_command_and_data(0xD333, 0x7F);
         
    write_command_and_data(0xD400, 0x00);
    write_command_and_data(0xD401, 0x2D);
    write_command_and_data(0xD402, 0x00);
    write_command_and_data(0xD403, 0x2E);
    write_command_and_data(0xD404, 0x00); 
    write_command_and_data(0xD405, 0x32);
    write_command_and_data(0xD406, 0x00);
    write_command_and_data(0xD407, 0x44);
    write_command_and_data(0xD408, 0x00);
    write_command_and_data(0xD409, 0x53);
    write_command_and_data(0xD40A, 0x00);
    write_command_and_data(0xD40B, 0x88);
    write_command_and_data(0xD40C, 0x00);
    write_command_and_data(0xD40D, 0xB6);
    write_command_and_data(0xD40E, 0x00);
    write_command_and_data(0xD40F, 0xF3); //
    write_command_and_data(0xD410, 0x01);
    write_command_and_data(0xD411, 0x22);
    write_command_and_data(0xD412, 0x01);
    write_command_and_data(0xD413, 0x64);
    write_command_and_data(0xD414, 0x01);
    write_command_and_data(0xD415, 0x92);
    write_command_and_data(0xD416, 0x01);
    write_command_and_data(0xD417, 0xD4);
    write_command_and_data(0xD418, 0x02); 
    write_command_and_data(0xD419, 0x07);
    write_command_and_data(0xD41A, 0x02);
    write_command_and_data(0xD41B, 0x08);
    write_command_and_data(0xD41C, 0x02);
    write_command_and_data(0xD41D, 0x34);
    write_command_and_data(0xD41E, 0x02);
    write_command_and_data(0xD41F, 0x5F); //
    write_command_and_data(0xD420, 0x02);
    write_command_and_data(0xD421, 0x78);
    write_command_and_data(0xD422, 0x02);
    write_command_and_data(0xD423, 0x94);
    write_command_and_data(0xD424, 0x02);
    write_command_and_data(0xD425, 0xA6);
    write_command_and_data(0xD426, 0x02);
    write_command_and_data(0xD427, 0xBB);
    write_command_and_data(0xD428, 0x02); 
    write_command_and_data(0xD429, 0xCA);
    write_command_and_data(0xD42A, 0x02);
    write_command_and_data(0xD42B, 0xDB);
    write_command_and_data(0xD42C, 0x02);
    write_command_and_data(0xD42D, 0xE8);
    write_command_and_data(0xD42E, 0x02);
    write_command_and_data(0xD42F, 0xF9); //
    write_command_and_data(0xD430, 0x03); 
    write_command_and_data(0xD431, 0x1F);
    write_command_and_data(0xD432, 0x03);
    write_command_and_data(0xD433, 0x7F);
         
    write_command_and_data(0xD500, 0x00);
    write_command_and_data(0xD501, 0x2D);
    write_command_and_data(0xD502, 0x00);
    write_command_and_data(0xD503, 0x2E);
    write_command_and_data(0xD504, 0x00); 
    write_command_and_data(0xD505, 0x32);
    write_command_and_data(0xD506, 0x00);
    write_command_and_data(0xD507, 0x44);
    write_command_and_data(0xD508, 0x00);
    write_command_and_data(0xD509, 0x53);
    write_command_and_data(0xD50A, 0x00);
    write_command_and_data(0xD50B, 0x88);
    write_command_and_data(0xD50C, 0x00);
    write_command_and_data(0xD50D, 0xB6);
    write_command_and_data(0xD50E, 0x00);
    write_command_and_data(0xD50F, 0xF3); //
    write_command_and_data(0xD510, 0x01);
    write_command_and_data(0xD511, 0x22);
    write_command_and_data(0xD512, 0x01);
    write_command_and_data(0xD513, 0x64);
    write_command_and_data(0xD514, 0x01);
    write_command_and_data(0xD515, 0x92);
    write_command_and_data(0xD516, 0x01);
    write_command_and_data(0xD517, 0xD4);
    write_command_and_data(0xD518, 0x02); 
    write_command_and_data(0xD519, 0x07);
    write_command_and_data(0xD51A, 0x02);
    write_command_and_data(0xD51B, 0x08);
    write_command_and_data(0xD51C, 0x02);
    write_command_and_data(0xD51D, 0x34);
    write_command_and_data(0xD51E, 0x02);
    write_command_and_data(0xD51F, 0x5F); //
    write_command_and_data(0xD520, 0x02);
    write_command_and_data(0xD521, 0x78);
    write_command_and_data(0xD522, 0x02);
    write_command_and_data(0xD523, 0x94);
    write_command_and_data(0xD524, 0x02);
    write_command_and_data(0xD525, 0xA6);
    write_command_and_data(0xD526, 0x02);
    write_command_and_data(0xD527, 0xBB);
    write_command_and_data(0xD528, 0x02); 
    write_command_and_data(0xD529, 0xCA);
    write_command_and_data(0xD52A, 0x02);
    write_command_and_data(0xD52B, 0xDB);
    write_command_and_data(0xD52C, 0x02);
    write_command_and_data(0xD52D, 0xE8);
    write_command_and_data(0xD52E, 0x02);
    write_command_and_data(0xD52F, 0xF9); //
    write_command_and_data(0xD530, 0x03); 
    write_command_and_data(0xD531, 0x1F);
    write_command_and_data(0xD532, 0x03);
    write_command_and_data(0xD533, 0x7F);
         
    write_command_and_data(0xD600, 0x00);
    write_command_and_data(0xD601, 0x2D);
    write_command_and_data(0xD602, 0x00);
    write_command_and_data(0xD603, 0x2E);
    write_command_and_data(0xD604, 0x00); 
    write_command_and_data(0xD605, 0x32);
    write_command_and_data(0xD606, 0x00);
    write_command_and_data(0xD607, 0x44);
    write_command_and_data(0xD608, 0x00);
    write_command_and_data(0xD609, 0x53);
    write_command_and_data(0xD60A, 0x00);
    write_command_and_data(0xD60B, 0x88);
    write_command_and_data(0xD60C, 0x00);
    write_command_and_data(0xD60D, 0xB6);
    write_command_and_data(0xD60E, 0x00);
    write_command_and_data(0xD60F, 0xF3); //
    write_command_and_data(0xD610, 0x01);
    write_command_and_data(0xD611, 0x22);
    write_command_and_data(0xD612, 0x01);
    write_command_and_data(0xD613, 0x64);
    write_command_and_data(0xD614, 0x01);
    write_command_and_data(0xD615, 0x92);
    write_command_and_data(0xD616, 0x01);
    write_command_and_data(0xD617, 0xD4);
    write_command_and_data(0xD618, 0x02); 
    write_command_and_data(0xD619, 0x07);
    write_command_and_data(0xD61A, 0x02);
    write_command_and_data(0xD61B, 0x08);
    write_command_and_data(0xD61C, 0x02);
    write_command_and_data(0xD61D, 0x34);
    write_command_and_data(0xD61E, 0x02);
    write_command_and_data(0xD61F, 0x5F); //
    write_command_and_data(0xD620, 0x02);
    write_command_and_data(0xD621, 0x78);
    write_command_and_data(0xD622, 0x02);
    write_command_and_data(0xD623, 0x94);
    write_command_and_data(0xD624, 0x02);
    write_command_and_data(0xD625, 0xA6);
    write_command_and_data(0xD626, 0x02);
    write_command_and_data(0xD627, 0xBB);
    write_command_and_data(0xD628, 0x02); 
    write_command_and_data(0xD629, 0xCA);
    write_command_and_data(0xD62A, 0x02);
    write_command_and_data(0xD62B, 0xDB);
    write_command_and_data(0xD62C, 0x02);
    write_command_and_data(0xD62D, 0xE8);
    write_command_and_data(0xD62E, 0x02);
    write_command_and_data(0xD62F, 0xF9); //
    write_command_and_data(0xD630, 0x03); 
    write_command_and_data(0xD631, 0x1F);
    write_command_and_data(0xD632, 0x03);
    write_command_and_data(0xD633, 0x7F);
     
    //#LV2 Page 0 enable
    write_command_and_data(0xF000, 0x55);
    write_command_and_data(0xF001, 0xAA);
    write_command_and_data(0xF002, 0x52);
    write_command_and_data(0xF003, 0x08);
    write_command_and_data(0xF004, 0x00); 
    
    //#DISPLAY CONTROL
    write_command_and_data(0xB100, 0xCC);
    write_command_and_data(0xB101, 0x00); 


    {write_command_and_data(0xB500, 0x50);}
        
    //#SOURCE HOLD TIME
    write_command_and_data(0xB600, 0x05);
    
    //Set Gate EQ     
    write_command_and_data(0xB700, 0x70); 
    write_command_and_data(0xB701, 0x70);
    
    //#Source EQ control (Mode 2)
    write_command_and_data(0xB800, 0x01);
    write_command_and_data(0xB801, 0x03);
    write_command_and_data(0xB802, 0x03);
    write_command_and_data(0xB803, 0x03);
    
    //#INVERSION MODE
    write_command_and_data(0xBC00, 0x02);
    write_command_and_data(0xBC01, 0x00); 
    write_command_and_data(0xBC02, 0x00); 
    
    //#Timing control
    write_command_and_data(0xC900, 0xD0);   
    write_command_and_data(0xC901, 0x02);
    write_command_and_data(0xC902, 0x50);
    write_command_and_data(0xC903, 0x50); 
    write_command_and_data(0xC904, 0x50); 
    

   write_command_and_data(0x3500, 0x00); 


  write_command_and_data(0x3A00, 0x55); //Data format 16-Bits
  write_command_and_data(0x3600, 0x00);   

#endif
  SglBeatWR_nPrm_8(0x1100, nullptr, 0);   //StartUp  
  
  delay(120);

  SglBeatWR_nPrm_8(0x2900, nullptr, 0);   //Display On  
   delay(100);


    Serial.print("displayInit return\n");
}

FASTRUN void NT35510_t4x_p::CSLow() {
    digitalWriteFast(_cs, LOW); // Select TFT
}

FASTRUN void NT35510_t4x_p::CSHigh() {
    digitalWriteFast(_cs, HIGH); // Deselect TFT
}

FASTRUN void NT35510_t4x_p::DCLow() {
    digitalWriteFast(_dc, LOW); // Writing command to TFT
}

FASTRUN void NT35510_t4x_p::DCHigh() {
    digitalWriteFast(_dc, HIGH); // Writing data to TFT
}

FASTRUN void NT35510_t4x_p::microSecondDelay() {
    for (uint32_t i = 0; i < 99; i++)
        __asm__("nop\n\t");
}

FASTRUN void NT35510_t4x_p::gpioWrite() {
    pFlex->setIOPinToFlexMode(_wr_pin);
    pinMode(_rd_pin, OUTPUT);
    digitalWriteFast(_rd_pin, HIGH);
}

FASTRUN void NT35510_t4x_p::gpioRead() {
    pFlex->setIOPinToFlexMode(_rd_pin);
    pinMode(_wr_pin, OUTPUT);
    digitalWriteFast(_wr_pin, HIGH);
}

// If used this must be called before begin
// Set the FlexIO pins.  The first version you can specify just the wr, and read and optionsl first Data.
// it will use information in the Flexio library to fill in d1-d7
FASTRUN bool NT35510_t4x_p::setFlexIOPins(uint8_t write_pin, uint8_t rd_pin, uint8_t tft_d0) {
    DBGPrintf("NT35510_t4x_p::setFlexIOPins(%u, %u, %u) %u %u %u\n", write_pin, rd_pin, tft_d0, _data_pins[0], _wr_pin, _rd_pin);
    DBGFlush();
    if (tft_d0 != 0xff) {
#ifdef FLEX_IO_HAS_FULL_PIN_MAPPING
        DBGPrintf("\td0 != 0xff\n\n");

        uint8_t flexio_pin;
        pFlex = FlexIOHandler::mapIOPinToFlexIOHandler(tft_d0, flexio_pin);
        if ((pFlex == nullptr) || (flexio_pin == 0xff))
            return false;

        _data_pins[0] = tft_d0;

        // lets dos some quick validation of the pins.
        for (uint8_t i = 1; i < 8; i++) {
            flexio_pin++; // lets look up the what pins come next.
            _data_pins[i] = pFlex->mapFlexPinToIOPin(flexio_pin);
            if (_data_pins[i] == 0xff) {
                Serial.printf("Failed to find Teensy IO pin for Flexio pin %u\n", flexio_pin);
                return false;
            }
        }
#else
        return false;
#endif
    }
    // set the write and read pins and see if d0 is not 0xff set it and compute the others.
    if (write_pin != 0xff)
        _wr_pin = write_pin;
    if (rd_pin != 0xff)
        _rd_pin = rd_pin;

    DBGPrintf("FlexIO pins: data: %u %u %u %u %u %u %u %u WR:%u RD:%u\n",
              _data_pins[0], _data_pins[1], _data_pins[2], _data_pins[3], _data_pins[4], _data_pins[5], _data_pins[6], _data_pins[7],
              _wr_pin, _rd_pin);
    return true;
}

// Set the FlexIO pins.  Specify all of the pins for 8 bit mode. Must be called before begin
FLASHMEM bool NT35510_t4x_p::setFlexIOPins(uint8_t write_pin, uint8_t rd_pin, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                                           uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {

    _data_pins[0] = d0;
    _data_pins[1] = d1;
    _data_pins[2] = d2;
    _data_pins[3] = d3;
    _data_pins[4] = d4;
    _data_pins[5] = d5;
    _data_pins[6] = d6;
    _data_pins[7] = d7;
    _wr_pin = write_pin;
    _rd_pin = rd_pin;

    DBGPrintf("FlexIO pins: data: %u %u %u %u %u %u %u %u WR:%u RD:%u\n",
              _data_pins[0], _data_pins[1], _data_pins[2], _data_pins[3], _data_pins[4], _data_pins[5], _data_pins[6], _data_pins[7],
              _wr_pin, _rd_pin);
    // Note this does not verify the pins are valid.
    return true;
}

FASTRUN void NT35510_t4x_p::FlexIO_Init() {
    /* Get a FlexIO channel */
    // lets assume D0 is the valid one...
    DBGPrintf("FlexIO_Init: D0:%u WR:%u RD:%u\n", _data_pins[0], _wr_pin, _rd_pin);
    DBGFlush();
    pFlex = FlexIOHandler::mapIOPinToFlexIOHandler(_data_pins[0], _flexio_D0);
    // pFlex = FlexIOHandler::flexIOHandler_list[1]; // use FlexIO2
    /* Pointer to the port structure in the FlexIO channel */
    p = &pFlex->port();
    /* Pointer to the hardware structure in the FlexIO channel */
    hw = &pFlex->hardware();

    DBGPrintf("\tFlex:%p port:%p hw:%p\n", pFlex, p, hw);

    // lets do some quick validation of the pins.
    // BUGBUG: nibble mode sort of hard coded pin wise..
    _bus_width = 8;
    uint8_t previous_flexio_pin = _flexio_D0;
    for (uint8_t i = 1; i < 8; i++) {
        uint8_t flexio_pin = pFlex->mapIOPinToFlexPin(_data_pins[i]);
        if (flexio_pin != (previous_flexio_pin + 1)) {
            if ((i == 4) && (flexio_pin != 0xff)) {
                DBGPrintf("\tNibble Mode: D4(%u): %u\n", _data_pins[4], flexio_pin);
                _bus_width = 10;
            }
            Serial.printf("NT35510_t4x_p::FlexIO_Ini - Flex IO Data pins pin issue D0(%u), D%u(%u)\n", _flexio_D0, i, flexio_pin);
        }
        previous_flexio_pin = flexio_pin;
    }

    // Lets reserve the timer and shifters.
    _flexio_timer = pFlex->requestTimers();
    _flexio_timer_mask = 1 << _flexio_timer;

    if (pFlex->claimShifter(0)) {
        _write_shifter = 0;
    } else if (pFlex->claimShifter(4)) {
        _write_shifter = 4;
    } else {
        Serial.println("NT35510_t4x_p::FlexIO_Init could not claim write Shifter(0 or 4");
    }

    // Maybe this is optional
    if (pFlex->claimShifter(3)) {
        _read_shifter = 3;
    } else if (pFlex->claimShifter(7)) {
        _read_shifter = 7;
    } else {
        Serial.println("NT35510_t4x_p::FlexIO_Init could not claim Read Shifter(3 or 7");
    }
    _write_shifter_mask = 1 << _write_shifter;
    _read_shifter_mask = 1 << _read_shifter;

    DBGPrintf("FlexIO Timer:%u Shifter Write:%u Read:%u\n", _flexio_timer, _write_shifter, _read_shifter);

    _flexio_WR = pFlex->mapIOPinToFlexPin(_wr_pin);
    _flexio_RD = pFlex->mapIOPinToFlexPin(_rd_pin);

    if ((_flexio_WR == 0xff) || (_flexio_RD == 0xff)) {
        Serial.printf("NT35510_t4x_p::FlexIO_Ini - RD/WR pin issue: WR:%u(%u) RD:%u(%u)\n", _wr_pin, _flexio_WR, _rd_pin, _flexio_RD);
    }

    DBGPrintf("FlexIO pin mappings: D0(%u)=%u  WR(%u)=%u RD(%u)=%u\n)", _data_pins[0], _flexio_D0, _wr_pin, _flexio_WR, _rd_pin, _flexio_RD);

    // Now l
    for (uint8_t pin_index = 0; pin_index < 8; pin_index++) {
        pinMode(_data_pins[pin_index], OUTPUT);
    }

    /* Basic pin setup */
    pinMode(_wr_pin, OUTPUT); // FlexIO2:0 WR
    pinMode(_rd_pin, OUTPUT); // FlexIO2:1 RD

    digitalWriteFast(_wr_pin, HIGH);
    digitalWriteFast(_rd_pin, HIGH);

    /* High speed and drive strength configuration */
    *(portControlRegister(_wr_pin)) = 0xFF;
    *(portControlRegister(_rd_pin)) = 0xFF;

    for (uint8_t pin_index = 0; pin_index < 8; pin_index++) {
        *(portControlRegister(_data_pins[pin_index])) = 0xFF;
    }

    /* Set clock */
    pFlex->setClockSettings(3, 1, 0); // (480 MHz source, 1+1, 1+0) >> 480/2/1 >> 240Mhz

    /* Enable the clock */
    hw->clock_gate_register |= hw->clock_gate_mask;

    pFlex->setIOPinToFlexMode(_wr_pin);
    pFlex->setIOPinToFlexMode(_rd_pin);

    for (uint8_t pin_index = 0; pin_index < 8; pin_index++) {
        pFlex->setIOPinToFlexMode(_data_pins[pin_index]);
    }

    // Lets print out all of the pins, configurations
    for (uint8_t pin_index = 0; pin_index < 8; pin_index++) {
        DBGPrintf("Data%u: pin:%u Port:%08x Mux:%08x\n", pin_index, _data_pins[pin_index],
                  *(portControlRegister(_data_pins[pin_index])), *(portConfigRegister(_data_pins[pin_index])));
    }
    DBGPrintf("WR: pin:%u Port:%08x Mux:%08x\n", _wr_pin, *(portControlRegister(_wr_pin)), *(portConfigRegister(_wr_pin)));
    DBGPrintf("RD: pin:%u Port:%08x Mux:%08x\n", _rd_pin, *(portControlRegister(_rd_pin)), *(portConfigRegister(_rd_pin)));

    /* Enable the FlexIO with fast access */
    p->CTRL = FLEXIO_CTRL_FLEXEN /*| FLEXIO_CTRL_FASTACC */;

    gpioWrite();

    DBGPrintf("*** flexio_init completed ***\n");
}

FASTRUN void NT35510_t4x_p::FlexIO_Config_SnglBeat_Read() {

    if (flex_config == CONFIG_SNGLREAD)
        return;
    flex_config = CONFIG_SNGLREAD;
    DBGPrintf("NT35510_t4x_p::FlexIO_Config_SnglBeat_Read - Enter\n");
    p->CTRL &= ~FLEXIO_CTRL_FLEXEN;
    // p->CTRL |= FLEXIO_CTRL_SWRST;
    p->CTRL &= ~FLEXIO_CTRL_SWRST;

    gpioRead(); // write line high, pin 12(rst) as output

    /* Configure the shifters */
    p->SHIFTCFG[_read_shifter] =
        // FLEXIO_SHIFTCFG_INSRC                                                  /* Shifter input */
        FLEXIO_SHIFTCFG_SSTOP(0)     /* Shifter stop bit disabled */
        | FLEXIO_SHIFTCFG_SSTART(0)  /* Shifter start bit disabled and loading data on enabled */
        | FLEXIO_SHIFTCFG_PWIDTH(_bus_width - 1); /* Bus width */

    p->SHIFTCTL[_read_shifter] =
        FLEXIO_SHIFTCTL_TIMSEL(_flexio_timer)        /* Shifter's assigned timer index */
        | FLEXIO_SHIFTCTL_TIMPOL * (1)               /* Shift on posedge of shift clock */
        | FLEXIO_SHIFTCTL_PINCFG(0)                  /* Shifter's pin configured as input */
        | FLEXIO_SHIFTCTL_PINSEL(_flexio_D0) /*D0 */ /* Shifter's pin start index */
        | FLEXIO_SHIFTCTL_PINPOL * (0)               /* Shifter's pin active high */
        | FLEXIO_SHIFTCTL_SMOD(1);                   /* Shifter mode as recieve */

    /* Configure the timer for shift clock */
    p->TIMCMP[_flexio_timer] =
        (((1 * 2) - 1) << 8)                /* TIMCMP[15:8] = number of beats x 2 – 1 */
        | (((NT35510_CLOCK_READ) / 2) - 1); /* TIMCMP[7:0] = baud rate divider / 2 – 1 ::: 30 = 8Mhz with current controller speed */

    p->TIMCFG[_flexio_timer] =
        FLEXIO_TIMCFG_TIMOUT(0)       /* Timer output logic one when enabled and not affected by reset */
        | FLEXIO_TIMCFG_TIMDEC(0)     /* Timer decrement on FlexIO clock, shift clock equals timer output */
        | FLEXIO_TIMCFG_TIMRST(0)     /* Timer never reset */
        | FLEXIO_TIMCFG_TIMDIS(2)     /* Timer disabled on timer compare */
        | FLEXIO_TIMCFG_TIMENA(2)     /* Timer enabled on trigger high */
        | FLEXIO_TIMCFG_TSTOP(1)      /* Timer stop bit disabled */
        | FLEXIO_TIMCFG_TSTART * (0); /* Timer start bit disabled */

    p->TIMCTL[_flexio_timer] =
        FLEXIO_TIMCTL_TRGSEL((((_read_shifter) << 2) | 1)) /* Timer trigger selected as shifter's status flag */
        | FLEXIO_TIMCTL_TRGPOL * (1)                       /* Timer trigger polarity as active low */
        | FLEXIO_TIMCTL_TRGSRC * (1)                       /* Timer trigger source as internal */
        | FLEXIO_TIMCTL_PINCFG(3)                          /* Timer' pin configured as output */
        | FLEXIO_TIMCTL_PINSEL(_flexio_RD)                 /* Timer' pin index: RD pin */
        | FLEXIO_TIMCTL_PINPOL * (1)                       /* Timer' pin active low */
        | FLEXIO_TIMCTL_TIMOD(1);                          /* Timer mode as dual 8-bit counters baud/bit */


    // Clear the shifter status 
    p->SHIFTSTAT = _read_shifter_mask;
    p->SHIFTERR = _read_shifter_mask;
    
    /* Enable FlexIO */
    p->CTRL |= FLEXIO_CTRL_FLEXEN;
    DBGPrintf("NT35510_t4x_p::FlexIO_Config_SnglBeat_Read - Exit\n");
}

FASTRUN uint8_t NT35510_t4x_p::readCommand(uint16_t const cmd) {
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }

    FlexIO_Config_SnglBeat();
    output_command_helper(cmd);
    CSLow();


    FlexIO_Clear_Config_SnglBeat();
    FlexIO_Config_SnglBeat_Read();

    uint8_t dummy __attribute__((unused)) = 0;
    uint8_t data = 0;

    waitReadShiftStat(__LINE__);
    // digitalToggleFast(2);
    dummy = read_shiftbuf_byte();

    waitReadShiftStat(__LINE__);
    // digitalToggleFast(2);
    data = read_shiftbuf_byte();

    CSHigh();
    microSecondDelay();
    // Serial.printf("Dummy 0x%x, data 0x%x\n", dummy, data);

    // Set FlexIO back to Write mode
    FlexIO_Config_SnglBeat();
    return data;
};

// Note we could combine the above with thsi.
FASTRUN uint32_t NT35510_t4x_p::readCommandN(uint16_t const cmd, uint8_t count_bytes) {
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }

    FlexIO_Config_SnglBeat();
    output_command_helper(cmd);
    CSLow();

    FlexIO_Clear_Config_SnglBeat();
    FlexIO_Config_SnglBeat_Read();

    uint8_t __attribute__((unused)) dummy = 0;
    uint32_t data = 0;

    waitReadShiftStat(__LINE__);
    // digitalToggleFast(2);
    dummy = read_shiftbuf_byte();

    while (count_bytes--) {
        waitReadShiftStat(__LINE__);
        data = (data << 8) | read_shiftbuf_byte();
        // digitalToggleFast(2);
    }
    // Serial.printf("Dummy 0x%x, data 0x%x\n", dummy, data);

    // Set FlexIO back to Write mode
    microSecondDelay();
    CSHigh();
    microSecondDelay();
    FlexIO_Config_SnglBeat();
    return data;
};

void print_flexio_debug_data(FlexIOHandler *pFlex, uint8_t flexio_timer, uint8_t write_shifter, uint8_t read_shifter) {
    IMXRT_FLEXIO_t *p = &pFlex->port();
    Serial.println("\n**********************************");
    Serial.printf("FlexIO Index: %u Timer:%u Write Shifter:%u Read Shifter:%u\n", pFlex->FlexIOIndex(), flexio_timer, write_shifter, read_shifter);
    Serial.printf("CCM_CDCDR: %x\n", CCM_CDCDR);
    Serial.printf("CCM FlexIO1: %x FlexIO2: %x FlexIO3: %x\n", CCM_CCGR5 & CCM_CCGR5_FLEXIO1(CCM_CCGR_ON),
                  CCM_CCGR3 & CCM_CCGR3_FLEXIO2(CCM_CCGR_ON), CCM_CCGR7 & CCM_CCGR7_FLEXIO3(CCM_CCGR_ON));
    Serial.printf("VERID:%x PARAM:%x CTRL:%x PIN: %x\n", p->VERID, p->PARAM, p->CTRL, p->PIN);
    Serial.printf("SHIFTSTAT:%x SHIFTERR=%x TIMSTAT=%x\n", p->SHIFTSTAT, p->SHIFTERR, p->TIMSTAT);
    Serial.printf("SHIFTSIEN:%x SHIFTEIEN=%x TIMIEN=%x\n", p->SHIFTSIEN, p->SHIFTEIEN, p->TIMIEN);
    Serial.printf("SHIFTSDEN:%x SHIFTSTATE=%x\n", p->SHIFTSDEN, p->SHIFTSTATE);
    Serial.print("SHIFTCTL:");
    for (int i = 0; i < 8; i++) {
        Serial.printf(" %08x", p->SHIFTCTL[i]);
    }
    Serial.print("\nSHIFTCFG:");
    for (int i = 0; i < 8; i++) {
        Serial.printf(" %08x", p->SHIFTCFG[i]);
    }

    Serial.printf("\nTIMCTL:%x %x %x %x\n", p->TIMCTL[0], p->TIMCTL[1], p->TIMCTL[2], p->TIMCTL[3]);
    Serial.printf("TIMCFG:%x %x %x %x\n", p->TIMCFG[0], p->TIMCFG[1], p->TIMCFG[2], p->TIMCFG[3]);
    Serial.printf("TIMCMP:%x %x %x %x\n", p->TIMCMP[0], p->TIMCMP[1], p->TIMCMP[2], p->TIMCMP[3]);
}

FASTRUN void NT35510_t4x_p::FlexIO_Config_SnglBeat() {

    if (flex_config == CONFIG_SNGLBEAT)
        return;
    flex_config = CONFIG_SNGLBEAT;


    gpioWrite();

    p->CTRL &= ~FLEXIO_CTRL_FLEXEN;
    // p->CTRL |= FLEXIO_CTRL_SWRST;
    // Make sure we are not in reset...
    p->CTRL &= ~FLEXIO_CTRL_SWRST;

    /* Configure the shifters */
    // try setting it twice
    p->SHIFTCFG[_write_shifter] =
        FLEXIO_SHIFTCFG_INSRC * (1)  /* Shifter input */
        | FLEXIO_SHIFTCFG_SSTOP(0)   /* Shifter stop bit disabled */
        | FLEXIO_SHIFTCFG_SSTART(0)  /* Shifter start bit disabled and loading data on enabled */
        | FLEXIO_SHIFTCFG_PWIDTH(_bus_width - 1); /* Bus width */

    p->SHIFTCTL[_write_shifter] =
        FLEXIO_SHIFTCTL_TIMSEL(_flexio_timer) /* Shifter's assigned timer index */
        | FLEXIO_SHIFTCTL_TIMPOL * (0)        /* Shift on posedge of shift clock */
        | FLEXIO_SHIFTCTL_PINCFG(3)           /* Shifter's pin configured as output */
        | FLEXIO_SHIFTCTL_PINSEL(_flexio_D0)  /* Shifter's pin start index */
        | FLEXIO_SHIFTCTL_PINPOL * (0)        /* Shifter's pin active high */
        | FLEXIO_SHIFTCTL_SMOD(2);            /* Shifter mode as transmit */

    /* Configure the timer for shift clock */
    p->TIMCMP[_flexio_timer] =
        (((1 * 2) - 1) << 8)     /* TIMCMP[15:8] = number of beats x 2 – 1 */
        | ((_baud_div / 2) - 1); /* TIMCMP[7:0] = baud rate divider / 2 – 1 */

    p->TIMCFG[_flexio_timer] =
        FLEXIO_TIMCFG_TIMOUT(0)       /* Timer output logic one when enabled and not affected by reset */
        | FLEXIO_TIMCFG_TIMDEC(0)     /* Timer decrement on FlexIO clock, shift clock equals timer output */
        | FLEXIO_TIMCFG_TIMRST(0)     /* Timer never reset */
        | FLEXIO_TIMCFG_TIMDIS(2)     /* Timer disabled on timer compare */
        | FLEXIO_TIMCFG_TIMENA(2)     /* Timer enabled on trigger high */
        | FLEXIO_TIMCFG_TSTOP(0)      /* Timer stop bit disabled */
        | FLEXIO_TIMCFG_TSTART * (0); /* Timer start bit disabled */

    p->TIMCTL[_flexio_timer] =
        FLEXIO_TIMCTL_TRGSEL((((_write_shifter) << 2) | 1)) /* Timer trigger selected as shifter's status flag */
        | FLEXIO_TIMCTL_TRGPOL * (1)                        /* Timer trigger polarity as active low */
        | FLEXIO_TIMCTL_TRGSRC * (1)                        /* Timer trigger source as internal */
        | FLEXIO_TIMCTL_PINCFG(3)                           /* Timer' pin configured as output */
        | FLEXIO_TIMCTL_PINSEL(_flexio_WR)                  /* Timer' pin index: WR pin */
        | FLEXIO_TIMCTL_PINPOL * (1)                        /* Timer' pin active low */
        | FLEXIO_TIMCTL_TIMOD(1);                           /* Timer mode as dual 8-bit counters baud/bit */


    /*
    static uint8_t DEBUG_COUNT = 1;
    if (DEBUG_COUNT) {
        DEBUG_COUNT--;
        print_flexio_debug_data(pFlex, _flexio_timer, _write_shifter, _read_shifter);
    }
    */

    /* Enable FlexIO */
    p->CTRL |= FLEXIO_CTRL_FLEXEN;
}

FASTRUN void NT35510_t4x_p::FlexIO_Clear_Config_SnglBeat() {
    if (flex_config == CONFIG_CLEAR)
        return;
    DBGPrintf("NT35510_t4x_p::FlexIO_Clear_Config_SnglBeat() - Enter\n");
    flex_config = CONFIG_CLEAR;

    p->CTRL &= ~FLEXIO_CTRL_FLEXEN;
    // p->CTRL |= FLEXIO_CTRL_SWRST;
    p->CTRL &= ~FLEXIO_CTRL_SWRST;

    p->SHIFTCFG[_write_shifter] = 0;
    p->SHIFTCTL[_write_shifter] = 0;
    p->SHIFTSTAT = _write_shifter_mask;
    p->TIMCMP[_flexio_timer] = 0;
    p->TIMCFG[_flexio_timer] = 0;
    p->TIMSTAT = _flexio_timer_mask; /* Timer start bit disabled */
    p->TIMCTL[_flexio_timer] = 0;

    /* Enable FlexIO */
    p->CTRL |= FLEXIO_CTRL_FLEXEN;
    DBGPrintf("NT35510_t4x_p::FlexIO_Clear_Config_SnglBeat() - Exit\n");
}

FASTRUN void NT35510_t4x_p::FlexIO_Config_MultiBeat() {
    if (flex_config == CONFIG_MULTIBEAT)
        return;
    flex_config = CONFIG_MULTIBEAT;
    DBGPrintf("NT35510_t4x_p::FlexIO_Config_MultiBeat() - Enter\n");

    uint32_t i;
    uint8_t MulBeatWR_BeatQty = SHIFTNUM * sizeof(uint32_t) / sizeof(uint8_t); // Number of beats = number of shifters * beats per shifter
    if (_bus_width > 8)  MulBeatWR_BeatQty = MulBeatWR_BeatQty / 2;            // we use 16 bits at a time for T4...

    /* Disable and reset FlexIO */
    p->CTRL &= ~FLEXIO_CTRL_FLEXEN;
    // p->CTRL |= FLEXIO_CTRL_SWRST;
    p->CTRL &= ~FLEXIO_CTRL_SWRST;

    gpioWrite();

    for (i = 0; i <= SHIFTNUM - 1; i++) {
        p->SHIFTCFG[_write_shifter + i] =
            FLEXIO_SHIFTCFG_INSRC * (1U)       /* Shifter input from next shifter's output */
            | FLEXIO_SHIFTCFG_SSTOP(0U)        /* Shifter stop bit disabled */
            | FLEXIO_SHIFTCFG_SSTART(0U)       /* Shifter start bit disabled and loading data on enabled */
            | FLEXIO_SHIFTCFG_PWIDTH(_bus_width - 1); /* 8 bit shift width */
    }

    p->SHIFTCTL[_write_shifter] =
        FLEXIO_SHIFTCTL_TIMSEL(_flexio_timer) /* Shifter's assigned timer index */
        | FLEXIO_SHIFTCTL_TIMPOL * (0U)       /* Shift on posedge of shift clock */
        | FLEXIO_SHIFTCTL_PINCFG(3U)          /* Shifter's pin configured as output */
        | FLEXIO_SHIFTCTL_PINSEL(_flexio_D0)  /* Shifter's pin start index */
        | FLEXIO_SHIFTCTL_PINPOL * (0U)       /* Shifter's pin active high */
        | FLEXIO_SHIFTCTL_SMOD(2U);           /* shifter mode transmit */

    for (i = 1; i <= SHIFTNUM - 1; i++) {
        p->SHIFTCTL[_write_shifter + i] =
            FLEXIO_SHIFTCTL_TIMSEL(_flexio_timer) /* Shifter's assigned timer index */
            | FLEXIO_SHIFTCTL_TIMPOL * (0U)       /* Shift on posedge of shift clock */
            | FLEXIO_SHIFTCTL_PINCFG(0U)          /* Shifter's pin configured as output disabled */
            | FLEXIO_SHIFTCTL_PINSEL(_flexio_D0)  /* Shifter's pin start index */
            | FLEXIO_SHIFTCTL_PINPOL * (0U)       /* Shifter's pin active high */
            | FLEXIO_SHIFTCTL_SMOD(2U);           /* shifter mode transmit */

        //p->SHIFTSTAT = 1 << (_write_shifter + i); // clear out any previous state
        p->SHIFTERR = 1 << (_write_shifter + i); // clear out any previous state
    }

    /* Configure the timer for shift clock */
    p->TIMCMP[_flexio_timer] =
        ((MulBeatWR_BeatQty * 2U - 1) << 8) /* TIMCMP[15:8] = number of beats x 2 – 1 */
        | (_baud_div / 2U - 1U);            /* TIMCMP[7:0] = shift clock divide ratio / 2 - 1 */

    p->TIMCFG[_flexio_timer] = FLEXIO_TIMCFG_TIMOUT(0U)       /* Timer output logic one when enabled and not affected by reset */
                               | FLEXIO_TIMCFG_TIMDEC(0U)     /* Timer decrement on FlexIO clock, shift clock equals timer output */
                               | FLEXIO_TIMCFG_TIMRST(0U)     /* Timer never reset */
                               | FLEXIO_TIMCFG_TIMDIS(2U)     /* Timer disabled on timer compare */
                               | FLEXIO_TIMCFG_TIMENA(2U)     /* Timer enabled on trigger high */
                               | FLEXIO_TIMCFG_TSTOP(0U)      /* Timer stop bit disabled */
                               | FLEXIO_TIMCFG_TSTART * (0U); /* Timer start bit disabled */

    p->TIMCTL[_flexio_timer] =
        FLEXIO_TIMCTL_TRGSEL((_write_shifter << 2) | 1U) /* Timer trigger selected as highest shifter's status flag */
        | FLEXIO_TIMCTL_TRGPOL * (1U)                    /* Timer trigger polarity as active low */
        | FLEXIO_TIMCTL_TRGSRC * (1U)                    /* Timer trigger source as internal */
        | FLEXIO_TIMCTL_PINCFG(3U)                       /* Timer' pin configured as output */
        | FLEXIO_TIMCTL_PINSEL(_flexio_WR)               /* Timer' pin index: WR pin */
        | FLEXIO_TIMCTL_PINPOL * (1U)                    /* Timer' pin active low */
        | FLEXIO_TIMCTL_TIMOD(1U);                       /* Timer mode 8-bit baud counter */


    /* Enable FlexIO */
    p->CTRL |= FLEXIO_CTRL_FLEXEN;

    //print_flexio_debug_data(pFlex, _flexio_timer, _write_shifter, _read_shifter);

   // configure interrupts
    if (hw->shifters_dma_channel[SHIFTER_DMA_REQUEST] == 0xff) {
        DBGPrintf("NT35510_t4x_p::FlexIO_Config_MultiBeat() - IRQ mode\n");
        attachInterruptVector(hw->flex_irq, flexio_ISR);
        NVIC_ENABLE_IRQ(hw->flex_irq);
        NVIC_SET_PRIORITY(hw->flex_irq, FLEXIO_ISR_PRIORITY);
        
        // disable interrupts until later
        p->SHIFTSIEN &= ~(1 << SHIFTER_IRQ);
        p->TIMIEN &= ~_flexio_timer_mask;

    } else {
        p->SHIFTSDEN |= 1U << (SHIFTER_DMA_REQUEST); // enable DMA trigger when shifter status flag is set on shifter SHIFTER_DMA_REQUEST
    }
    DBGPrintf("NT35510_t4x_p::FlexIO_Config_MultiBeat() - Exit\n");
}


FASTRUN void NT35510_t4x_p::write_command_and_data(uint32_t cmd, uint8_t val) {
    //Serial.printf("WCD: %x %x\n", cmd, val);
    FlexIO_Config_SnglBeat();
    /* Assert CS, RS pins */

    // delay(1);
    output_command_helper(cmd);
    microSecondDelay();

    CSLow();
    DCHigh();
    microSecondDelay();
    p->SHIFTBUF[_write_shifter] = 0;
    waitWriteShiftStat(__LINE__);

    p->SHIFTBUF[_write_shifter] = generate_output_word(val);
    waitWriteShiftStat(__LINE__);
    waitTimStat(__LINE__);
    microSecondDelay();
    CSHigh();
}

//--------------------------------------------------------------------
// Helper function to handle 16 bit cmds depending on bus width
void NT35510_t4x_p::writeRegM(uint16_t addr, uint8_t len, uint8_t data[]) {
  for (uint16_t i = 0; i < len; i++)
  {
#if 1
    write_command_and_data(addr++, *data++);
#else    
    output_command_helper(addr++);
    microSecondDelay();
    CSLow();
    DCHigh();
    microSecondDelay();
    p->SHIFTBUF[_write_shifter] = 0;
    waitWriteShiftStat(__LINE__);
    p->SHIFTBUF[_write_shifter] = generate_output_word(*data++);
    waitWriteShiftStat(__LINE__);
    waitTimStat(__LINE__);
    microSecondDelay();
    CSHigh();
#endif
  }    
}


FASTRUN void NT35510_t4x_p::output_command_helper(uint32_t cmd) {
    CSLow();
    DCLow();

    /* Write command index */
    // This display commands are 16 bits so need handle.
    if (_bus_width == 16) {
        p->SHIFTBUF[_write_shifter] = cmd;
    } else {
        p->SHIFTBUF[_write_shifter] = generate_output_word(cmd >> 8);  // high byte
        waitWriteShiftStat(__LINE__);
        //waitTimStat(__LINE__);
        p->SHIFTBUF[_write_shifter] = generate_output_word(cmd);  // low byte
    }

    /*Wait for transfer to be completed */
    waitWriteShiftStat(__LINE__);
    waitTimStat(__LINE__);

    /* De-assert RS pin */

    microSecondDelay();
    DCHigh();
    CSHigh();
}

FASTRUN void NT35510_t4x_p::SglBeatWR_nPrm_8(uint32_t const cmd, const uint8_t *value = NULL, uint32_t const length = 0) {
    Serial.printf("NT35510_t4x_p::SglBeatWR_nPrm_8(%x, %x, %u\n", cmd, value, length);
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }

    FlexIO_Config_SnglBeat();
    uint32_t i;
    /* Assert CS, RS pins */

    // delay(1);
    CSLow();
    output_command_helper(cmd);
    microSecondDelay();

    if (length) {
        for (i = 0; i < length; i++) {
            p->SHIFTBUF[_write_shifter] = generate_output_word(*value++);
            waitWriteShiftStat(__LINE__);
        }
        waitTimStat(__LINE__);
    }
    microSecondDelay();
    CSHigh();
    /* De-assert CS pin */
}

FASTRUN void NT35510_t4x_p::SglBeatWR_nPrm_16(uint32_t const cmd, const uint16_t *value, uint32_t const length) {
    //Serial.printf("NT35510_t4x_p::SglBeatWR_nPrm_16(%x, %p, %u) %u\n", cmd, value, length, _bitDepth);
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }
    FlexIO_Config_SnglBeat();
    uint16_t buf;
    /* Assert CS, RS pins */
    output_command_helper(cmd);
    microSecondDelay();
    CSLow();

    if (length) {
        if (_bitDepth == 24) {
            uint8_t r, g, b;
            for (uint32_t i = 0; i < length/* - 1U*/; i++) {
                buf = *value++;
                color565toRGB(buf, r, g, b);
                waitWriteShiftStat(__LINE__);
                p->SHIFTBUF[_write_shifter] = generate_output_word(r);
                waitWriteShiftStat(__LINE__);
                p->SHIFTBUF[_write_shifter] = generate_output_word(g);
                waitWriteShiftStat(__LINE__);
                p->SHIFTBUF[_write_shifter] = generate_output_word(b);
            }
        } else {

            for (uint32_t i = 0; i < length - 1U; i++) {
                buf = *value++;
                waitWriteShiftStat(__LINE__);
                p->SHIFTBUF[_write_shifter] = generate_output_word(buf >> 8);

                waitWriteShiftStat(__LINE__);
                p->SHIFTBUF[_write_shifter] = generate_output_word(buf & 0xFF);
            }
            buf = *value++;
            /* Write the last byte */
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(buf >> 8);

            waitWriteShiftStat(__LINE__);
            p->TIMSTAT |= _flexio_timer_mask;

            p->SHIFTBUF[_write_shifter] = generate_output_word(buf & 0xFF);
        }

        /*Wait for transfer to be completed */
        waitTimStat();
    }
    microSecondDelay();
    CSHigh();
}

void dumpDMA_TCD(DMABaseClass *dmabc, const char *psz_title) {
  if (psz_title)
    Serial.print(psz_title);
  Serial.printf("%x %x:", (uint32_t)dmabc, (uint32_t)dmabc->TCD);

  Serial.printf(
      "SA:%x SO:%d AT:%x NB:%x SL:%d DA:%x DO: %d CI:%x DL:%x CS:%x BI:%x\n",
      (uint32_t)dmabc->TCD->SADDR, dmabc->TCD->SOFF, dmabc->TCD->ATTR,
      dmabc->TCD->NBYTES, dmabc->TCD->SLAST, (uint32_t)dmabc->TCD->DADDR,
      dmabc->TCD->DOFF, dmabc->TCD->CITER, dmabc->TCD->DLASTSGA,
      dmabc->TCD->CSR, dmabc->TCD->BITER);
}


NT35510_t4x_p *NT35510_t4x_p::dmaCallback = nullptr;
DMAChannel NT35510_t4x_p::flexDma;
DMASetting NT35510_t4x_p::_dmaSettings[2];

FASTRUN void NT35510_t4x_p::MulBeatWR_nPrm_DMA(uint32_t const cmd, const void *value, uint32_t const length) {
    DBGPrintf("NT35510_t4x_p::MulBeatWR_nPrm_DMA(%x, %x, %u\n", cmd, value, length);
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }
    uint32_t BeatsPerMinLoop = SHIFTNUM * sizeof(uint32_t) / sizeof(uint8_t); // Number of shifters * number of 8 bit values per shifter
    uint32_t majorLoopCount, minorLoopBytes;
    uint32_t destinationModulo = 31 - (__builtin_clz(SHIFTNUM * sizeof(uint32_t))); // defines address range for circular DMA destination buffer

    FlexIO_Config_SnglBeat();
    output_command_helper(cmd);
    CSLow();
    microSecondDelay();

    if (length < 8) {
        // Serial.println ("In DMA but to Short to multibeat");
        const uint16_t *newValue = (uint16_t *)value;
        uint16_t buf;
        for (uint32_t i = 0; i < length; i++) {
            buf = *newValue++;
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(buf >> 8);

            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(buf & 0xFF);
        }
        // Wait for transfer to be completed
        while (0 == (p->TIMSTAT & _flexio_timer_mask)) {
        }

        microSecondDelay();
        CSHigh();

    }

    else {
        // memcpy(framebuff, value, length);
        // arm_dcache_flush((void*)framebuff, sizeof(framebuff)); // always flush cache after writing to DMAMEM variable that will be accessed by DMA
        if ((uint32_t)value >= 0x20200000u) {
            arm_dcache_flush((void*)value, length*2);
        }

        FlexIO_Config_MultiBeat();

        MulBeatCountRemain = length % BeatsPerMinLoop;
        MulBeatDataRemain = (uint16_t *)value + ((length - MulBeatCountRemain)); // pointer to the next unused byte (overflow if MulBeatCountRemain = 0)
        TotalSize = (length - MulBeatCountRemain) * 2;                           /* in bytes */
        minorLoopBytes = SHIFTNUM * sizeof(uint32_t);
        majorLoopCount = TotalSize / minorLoopBytes;
        // Serial.printf("Length(16bit): %d, Count remain(16bit): %d, Data remain: %d, TotalSize(8bit): %d, majorLoopCount: %d \n",length, MulBeatCountRemain, MulBeatDataRemain, TotalSize, majorLoopCount );

        /* Configure FlexIO with multi-beat write configuration */
        flexDma.begin();

        /* Setup DMA transfer with on-the-fly swapping of MSB and LSB in 16-bit data:
         *  Within each minor loop, read 16-bit values from buf in reverse order, then write 32bit values to SHIFTBUFBYS[i] in reverse order.
         *  Result is that every pair of bytes are swapped, while half-words are unswapped.
         *  After each minor loop, advance source address using minor loop offset. */
        int destinationAddressOffset, destinationAddressLastOffset, sourceAddressOffset, sourceAddressLastOffset, minorLoopOffset;
        volatile void *destinationAddress, *sourceAddress;

        DMA_CR |= DMA_CR_EMLM; // enable minor loop mapping

        sourceAddress = (uint16_t *)value + minorLoopBytes / sizeof(uint16_t) - 1; // last 16bit address within current minor loop
        sourceAddressOffset = -sizeof(uint16_t);                                   // read values in reverse order
        minorLoopOffset = 2 * minorLoopBytes;                                      // source address offset at end of minor loop to advance to next minor loop
        sourceAddressLastOffset = minorLoopOffset - TotalSize;                     // source address offset at completion to reset to beginning
        destinationAddress = (uint32_t *)&p->SHIFTBUFBYS[SHIFTNUM - 1];            // last 32bit shifter address (with reverse byte order)
        destinationAddressOffset = -sizeof(uint32_t);                              // write words in reverse order
        destinationAddressLastOffset = 0;

         if (majorLoopCount <= 32767) {
            flexDma.TCD->SADDR = sourceAddress;
            flexDma.TCD->SOFF = sourceAddressOffset;
            flexDma.TCD->SLAST = sourceAddressLastOffset;
            flexDma.TCD->DADDR = destinationAddress;
            flexDma.TCD->DOFF = destinationAddressOffset;
            flexDma.TCD->DLASTSGA = destinationAddressLastOffset;
            flexDma.TCD->ATTR =
                DMA_TCD_ATTR_SMOD(0U) | DMA_TCD_ATTR_SSIZE(DMA_TCD_ATTR_SIZE_16BIT)                   // 16bit reads
                | DMA_TCD_ATTR_DMOD(destinationModulo) | DMA_TCD_ATTR_DSIZE(DMA_TCD_ATTR_SIZE_32BIT); // 32bit writes
            flexDma.TCD->NBYTES_MLOFFYES =
                DMA_TCD_NBYTES_SMLOE | DMA_TCD_NBYTES_MLOFFYES_MLOFF(minorLoopOffset) | DMA_TCD_NBYTES_MLOFFYES_NBYTES(minorLoopBytes);
            flexDma.TCD->CITER = majorLoopCount; // Current major iteration count
            flexDma.TCD->BITER = majorLoopCount; // Starting major iteration count

            flexDma.triggerAtHardwareEvent(hw->shifters_dma_channel[SHIFTER_DMA_REQUEST]);
            flexDma.disableOnCompletion();
            flexDma.interruptAtCompletion();
            flexDma.clearComplete();
            #ifdef DEBUG
            dumpDMA_TCD(&flexDma, "NT35510_t4x_p)\n");
            #endif
        } else {
                // Too big to fit into one DMASetting.
            // Too big to fit into one...  
            uint32_t half_major_loop_count = majorLoopCount / 2;      
            _dmaSettings[0].TCD->SADDR = sourceAddress;
            _dmaSettings[0].TCD->SOFF = sourceAddressOffset;
            _dmaSettings[0].TCD->SLAST = sourceAddressLastOffset;
            _dmaSettings[0].TCD->DADDR = destinationAddress;
            _dmaSettings[0].TCD->DOFF = destinationAddressOffset;
            _dmaSettings[0].TCD->DLASTSGA = destinationAddressLastOffset;
            _dmaSettings[0].TCD->ATTR =
                DMA_TCD_ATTR_SMOD(0U)
                | DMA_TCD_ATTR_SSIZE(DMA_TCD_ATTR_SIZE_16BIT) // 16bit reads
                | DMA_TCD_ATTR_DMOD(destinationModulo)
                | DMA_TCD_ATTR_DSIZE(DMA_TCD_ATTR_SIZE_32BIT); // 32bit writes
            _dmaSettings[0].TCD->NBYTES_MLOFFYES = 
                DMA_TCD_NBYTES_SMLOE
                | DMA_TCD_NBYTES_MLOFFYES_MLOFF(minorLoopOffset)
                | DMA_TCD_NBYTES_MLOFFYES_NBYTES(minorLoopBytes);
            _dmaSettings[0].TCD->CITER = half_major_loop_count; // Current major iteration count
            _dmaSettings[0].TCD->BITER = half_major_loop_count; // Starting major iteration count
            _dmaSettings[0].replaceSettingsOnCompletion(_dmaSettings[1]);

            _dmaSettings[1].TCD->SADDR = (uint8_t *)sourceAddress + (half_major_loop_count * minorLoopBytes);
            _dmaSettings[1].TCD->SOFF = sourceAddressOffset;
            _dmaSettings[1].TCD->SLAST = sourceAddressLastOffset;
            _dmaSettings[1].TCD->DADDR = destinationAddress;
            _dmaSettings[1].TCD->DOFF = destinationAddressOffset;
            _dmaSettings[1].TCD->DLASTSGA = destinationAddressLastOffset;
            _dmaSettings[1].TCD->ATTR =
                DMA_TCD_ATTR_SMOD(0U)
                | DMA_TCD_ATTR_SSIZE(DMA_TCD_ATTR_SIZE_16BIT) // 16bit reads
                | DMA_TCD_ATTR_DMOD(destinationModulo)
                | DMA_TCD_ATTR_DSIZE(DMA_TCD_ATTR_SIZE_32BIT); // 32bit writes
            _dmaSettings[1].TCD->NBYTES_MLOFFYES = 
                DMA_TCD_NBYTES_SMLOE
                | DMA_TCD_NBYTES_MLOFFYES_MLOFF(minorLoopOffset)
                | DMA_TCD_NBYTES_MLOFFYES_NBYTES(minorLoopBytes);
            _dmaSettings[1].TCD->CITER = majorLoopCount - half_major_loop_count; // Current major iteration count
            _dmaSettings[1].TCD->BITER = majorLoopCount - half_major_loop_count; // Starting major iteration count
            _dmaSettings[1].disableOnCompletion();
            _dmaSettings[1].interruptAtCompletion();
            _dmaSettings[1].replaceSettingsOnCompletion(_dmaSettings[0]);

            flexDma = _dmaSettings[0];
            flexDma.triggerAtHardwareEvent(hw->shifters_dma_channel[SHIFTER_DMA_REQUEST]);
            flexDma.clearComplete();
            #ifdef DEBUG
            dumpDMA_TCD(&flexDma, "\n*** MulBeatWR_nPrm_DMA ***\n");
            dumpDMA_TCD(&_dmaSettings[0], "DmaSettings[0]\n");
            dumpDMA_TCD(&_dmaSettings[1], "DmaSettings[1]\n");
            #endif
        }

        // Serial.println("Dma setup done");

        /* Start data transfer by using DMA */
        WR_AsyncTransferDone = false;
        flexDma.attachInterrupt(dmaISR);


        flexDma.enable();
        // Serial.println("Starting transfer");
        dmaCallback = this;
    }
}

FASTRUN void NT35510_t4x_p::_onCompleteCB() {
    if (_callback) {
        _callback();
    }
    return;
}

FASTRUN void NT35510_t4x_p::dmaISR() {
    flexDma.clearInterrupt();
    asm volatile("dsb"); // prevent interrupt from re-entering
    dmaCallback->flexDma_Callback();
}

FASTRUN void NT35510_t4x_p::flexDma_Callback() {
    // Serial.printf("DMA callback start triggred \n");

    /* the interrupt is called when the final DMA transfer completes writing to the shifter buffers, which would generally happen while
    data is still in the process of being shifted out from the second-to-last major iteration. In this state, all the status flags are cleared.
    when the second-to-last major iteration is fully shifted out, the final data is transfered from the buffers into the shifters which sets all the status flags.
    if you have only one major iteration, the status flags will be immediately set before the interrupt is called, so the while loop will be skipped. */
    while (0 == (p->SHIFTSTAT & (1U << (SHIFTNUM - 1)))) {
    }

    /* Wait the last multi-beat transfer to be completed. Clear the timer flag
    before the completing of the last beat. The last beat may has been completed
    at this point, then code would be dead in the while() below. So mask the
    while() statement and use the software delay .*/
    p->TIMSTAT |= (1U << 0U);

    /* Wait timer flag to be set to ensure the completing of the last beat.
    while(0 == (p->TIMSTAT & (1U << 0U)))
    {
    }
    */
    delayMicroseconds(200);

    if (MulBeatCountRemain) {
        // Serial.printf("MulBeatCountRemain in DMA callback: %d, MulBeatDataRemain %x \n", MulBeatCountRemain,MulBeatDataRemain);
        uint16_t value;
        /* Configure FlexIO with 1-beat write configuration */
        FlexIO_Config_SnglBeat();

        // Serial.printf("Starting single beat completion: %d \n", MulBeatCountRemain);

        /* Use polling method for data transfer */
        for (uint32_t i = 0; i < (MulBeatCountRemain); i++) {
            value = *MulBeatDataRemain++;
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(value >> 8);

            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(value & 0xFF);
        }
        p->TIMSTAT |= _flexio_timer_mask;
        /*
        value = *MulBeatDataRemain++;
        //Write the last byte

        while(0 == (p->SHIFTSTAT & _flexio_timer_mask))
            {
            }
        p->SHIFTBUF[_write_shifter] = generate_output_word(value >> 8);

        while(0 == (p->SHIFTSTAT & _flexio_timer_mask))
        {
        }
        p->TIMSTAT |= _flexio_timer_mask;

        p->SHIFTBUF[_write_shifter] = generate_output_word(value & 0xFF);
        */
        /*Wait for transfer to be completed */
        waitTimStat();
        // Serial.println("Finished single beat completion");
    }

    microSecondDelay();
    CSHigh();
    /* the for loop is probably not sufficient to complete the transfer. Shifting out all 32 bytes takes (32 beats)/(6 MHz) = 5.333 microseconds which is over 3000 CPU cycles.
    If you really need to wait in this callback until all the data has been shifted out, the while loop is probably the correct solution and I don't think it risks an infinite loop.
    however, it seems like a waste of time to wait here, since the process otherwise completes in the background and the shifter buffers are ready to receive new data while the transfer completes.
    I think in most applications you could continue without waiting. You can start a new DMA transfer as soon as the first one completes (no need to wait for FlexIO to finish shifting). */

    WR_AsyncTransferDone = true;
    //    flexDma.disable(); // not necessary because flexDma is already configured to disable on completion
    if (isCB) {
        // Serial.printf("custom callback triggred \n");
        _onCompleteCB();
    }
    // Serial.printf("DMA callback end triggred \n");
}

void NT35510_t4x_p::DMAerror() {
    if (flexDma.error()) {
        Serial.print("DMA error: ");
        Serial.println(DMA_ES, HEX);
    }
}

void NT35510_t4x_p::beginWrite16BitColors() {
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }
    DBGPrintf("NT35510_t4x_p::beginWrite16BitColors() - 0x2c\n");
    FlexIO_Config_SnglBeat();
    /* Assert CS, RS pins */
    output_command_helper(NT35510_RAMWR);
    microSecondDelay();
    CSLow();
}

void NT35510_t4x_p::write16BitColor(uint16_t color) {
    if (_bitDepth == 24) {
        uint8_t r, g, b;
        color565toRGB(color, r, g, b);
        waitWriteShiftStat(__LINE__);
        p->SHIFTBUF[_write_shifter] = generate_output_word(r);

        waitWriteShiftStat(__LINE__);
        p->SHIFTBUF[_write_shifter] = generate_output_word(g);

        waitWriteShiftStat(__LINE__);
        p->SHIFTBUF[_write_shifter] = generate_output_word(b);

    } else {
        waitWriteShiftStat(__LINE__);
        p->SHIFTBUF[_write_shifter] = generate_output_word(color >> 8);

        waitWriteShiftStat(__LINE__);
        p->SHIFTBUF[_write_shifter] = generate_output_word(color & 0xFF);
    }
}

void NT35510_t4x_p::endWrite16BitColors() {
    /*Wait for transfer to be completed */
    waitTimStat();
    microSecondDelay();
    CSHigh();
}

//FASTRUN void NT35510_t4x_p::write16BitColor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *pcolors, uint16_t count) {
FASTRUN void NT35510_t4x_p::writeRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors) {
    while (WR_AsyncTransferDone == false) {
        // Wait for any DMA transfers to complete
    }
    uint32_t length = w * h;
    // bail if nothing to do
    if (length == 0) return;
    setAddr(x, y, x + w - 1, y + h -1);

    SglBeatWR_nPrm_16(NT35510_RAMWR, pcolors, length);
}

void NT35510_t4x_p::fillRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    uint32_t length = w * h;
    // bail if nothing to do
    if (length == 0) return;
    setAddr(x, y, x + w - 1, y + h -1);
//    Serial.printf("FillrectFlexIO(%d, %d, %d, %d, %x): %u\n", x, y, w, h, color, length);

    FlexIO_Config_SnglBeat();
    /* Assert CS, RS pins */
    output_command_helper(NT35510_RAMWR);
    microSecondDelay();
    CSLow();
    if (_bitDepth == 24) {
        uint8_t r, g, b;
        color565toRGB(color, r, g, b);
        while (length-- > 0) {
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(r);
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(g);
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(b);
        }
    } else {
        while (length-- > 1) {
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(color >> 8);

            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(color & 0xFF);
        }
    }
    /* Write the last pixel */
    waitWriteShiftStat(__LINE__);
    p->SHIFTBUF[_write_shifter] = generate_output_word(color >> 8);

    waitWriteShiftStat(__LINE__);
    p->TIMSTAT |= _flexio_timer_mask;

    p->SHIFTBUF[_write_shifter] = generate_output_word(color & 0xFF);
    CSHigh();

    /*Wait for transfer to be completed */
    waitTimStat(__LINE__);
    microSecondDelay();
}

bool NT35510_t4x_p::writeRect24BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint32_t *pixels) {
    uint32_t length = w * h;
    // bail if nothing to do
    if (length == 0) return false;
    setAddr(x, y, x + w - 1, y + h -1);
    Serial.printf("writeRect24BPP(%d, %d, %d, %d, %p): %u\n", x, y, w, h, pixels, length, _bitDepth);

    FlexIO_Config_SnglBeat();
    /* Assert CS, RS pins */
    output_command_helper(NT35510_RAMWR);
    microSecondDelay();
    CSLow();
    if (_bitDepth == 24) {
        while (length-- > 0) {
            uint32_t color = *pixels++;
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(color >> 16);
            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(color >> 8);
            waitWriteShiftStat(__LINE__);
            //if (length == 0)  p->TIMSTAT |= _flexio_timer_mask;
            p->SHIFTBUF[_write_shifter] = generate_output_word(color);
        }
    } else {
        while (length-- > 0) {
            uint32_t pixel = *pixels++;
            uint16_t color = color565(pixel >> 16, pixel >> 8, pixel);

            waitWriteShiftStat(__LINE__);
            p->SHIFTBUF[_write_shifter] = generate_output_word(pixel >> 8);

            waitWriteShiftStat(__LINE__);
            //if (length == 0)  p->TIMSTAT |= _flexio_timer_mask;
            p->SHIFTBUF[_write_shifter] = generate_output_word(pixel & 0xFF);
        }
    }
    CSHigh();
    /* Write the last pixel */
    /*Wait for transfer to be completed */
    waitTimStat(__LINE__);
    microSecondDelay();
    Serial.println("\twriteRect24BPP - exit\n");
    return true;
}


void NT35510_t4x_p::readRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors) {
    DBGPrintf("readRectFlexIO(%d, %d, %d, %d, %p)\n", x, y, w, h, pcolors);
    //  first do any clipping.
    if ((x >= _displayclipx2) || (y >= _displayclipy2))
        return;
    if (((x + w) <= _displayclipx1) || ((y + h) <= _displayclipy1))
        return;
    if (x < _displayclipx1) {
        w -= (_displayclipx1 - x);
        x = _displayclipx1;
    }
    if (y < _displayclipy1) {
        h -= (_displayclipy1 - y);
        y = _displayclipy1;
    }
    if ((x + w - 1) >= _displayclipx2)
        w = _displayclipx2 - x;
    if ((y + h - 1) >= _displayclipy2)
        h = _displayclipy2 - y;

    // We probably need to set the rectangle
    setAddr(x, y, x + w - 1, y + h - 1);
    // now set to ramRD command
    FlexIO_Config_SnglBeat();
    /* Assert CS, RS pins */

    /* Write command index */
    DBGPrintf("\tOutput NT35510_RAMRD\n");
    output_command_helper(NT35510_RAMRD);
    CSLow();
    microSecondDelay();
    // delayMicroseconds(50);

    DBGPrintf("\tcall FlexIO_Clear_Config_SnglBeat\n");
    FlexIO_Clear_Config_SnglBeat();
    DBGPrintf("\tcall FlexIO_Config_SnglBeat_Read\n");
    FlexIO_Config_SnglBeat_Read();

    uint8_t dummy __attribute__((unused)) = 0;
#define DUMMY_COUNT 1
    for (uint8_t i = 0; i < DUMMY_COUNT; i++) {
        // read in dummy bytes
        waitReadShiftStat(__LINE__);
        dummy = read_shiftbuf_byte();
        // digitalToggleFast(2);
        // Serial.printf("\tD%u=%x\n", i, dummy);
    }
    /*Wait for transfer to be completed */

    //if ((_display_name != ILI9488) && (_display_name != HX8357D)) {
        // 16 bit mode
     //   int count_pixels = w * h;
     //   uint8_t *pc = (uint8_t *)pcolors;
     //   while (count_pixels--) {
     //       waitReadShiftStat(__LINE__);
     //       // digitalToggleFast(2);
     //       uint8_t b1 = read_shiftbuf_byte();

     //       waitReadShiftStat(__LINE__);
     //       // digitalToggleFast(2);
     //       *pc++ = read_shiftbuf_byte();
     //       *pc++ = b1;
     //   }
    //} else {
        int count_pixels = w * h;
        while (count_pixels--) {
            uint8_t r, g, b;
            waitReadShiftStat(__LINE__);
            // digitalToggleFast(2);
            r = read_shiftbuf_byte();

            waitReadShiftStat(__LINE__);
            // digitalToggleFast(2);
            g = read_shiftbuf_byte();

            waitReadShiftStat(__LINE__);
            // digitalToggleFast(2);
            b = read_shiftbuf_byte();

            *pcolors++ = color565(r, g, b);
            #ifdef DEBUG
            static uint8_t debug_count = 50;
            if (debug_count) {
                debug_count--;
                Serial.printf("RRFIO %x %x %x - %x\n", r, g, b, pcolors[-1]);
            }
            #endif
        }
    //}

    CSHigh();
    microSecondDelay();
    // Set FlexIO back to Write mode
    FlexIO_Config_SnglBeat();
}



//=============================================================================
// Main Async interface
//      Called by GFX to do updateScreenAsync and new writeRectAsync(;
//=============================================================================
bool NT35510_t4x_p::writeRectAsyncFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors)
{
    // Start off only supporting shifters with DMA Requests
    if (hw->shifters_dma_channel[SHIFTER_DMA_REQUEST] != 0xff) {
        pushPixels16bitDMA(pcolors, x, y, x+w-1, y + h - 1);
    } else {
        // FlexIO3 IRQ version.
        setAddr(x, y, x + w - 1, y + h - 1);
        MulBeatWR_nPrm_IRQ(NT35510_RAMWR, pcolors, w * h);
    }
    return true;
}


bool NT35510_t4x_p::writeRectAsyncActiveFlexIO() { 
    // return the state of last transfer
    // may depend on if the FlexIO shifter supports DMA or not on how
    // we implement this.
    return !WR_AsyncTransferDone; 
}


//=============================================================================
// ASYNC - Done BY IRQ - mainly for FlexIO3
//=============================================================================
//#define BEATS_PER_SHIFTER (sizeof(uint32_t)/sizeof(uint8_t))
//#define BYTES_PER_BURST (BEATS_PER_SHIFTER*SHIFTNUM)


NT35510_t4x_p * NT35510_t4x_p::IRQcallback = nullptr;


FASTRUN void NT35510_t4x_p::MulBeatWR_nPrm_IRQ(uint32_t const cmd,  const void *value, uint32_t const length) 
{
    if (length == 0) return; // bail if no data to output

    DBGPrintf("NT35510_t4x_p::MulBeatWR_nPrm_IRQ(%x, %p, %u) - entered\n", cmd, value, length);
  while(WR_AsyncTransferDone == false)
  {
    //Wait for any DMA transfers to complete
  }
    FlexIO_Config_SnglBeat();
    CSLow();
    DCLow();

    /* Write command index */
    p->SHIFTBUF[_write_shifter] = generate_output_word(cmd);

    /*Wait for transfer to be completed */
    waitTimStat(__LINE__);
    microSecondDelay();
    /* De-assert RS pin */
    DCHigh();
    microSecondDelay();


    FlexIO_Config_MultiBeat();
    WR_AsyncTransferDone = false;
    uint32_t bytes = length*2U;

    _irq_bytes_per_shifter = (_bus_width <= 8) ? 4 : 2;
    _irq_bytes_per_burst = _irq_bytes_per_shifter * SHIFTNUM;

    _irq_bursts_to_complete = bytes / _irq_bytes_per_burst;

    int remainder = bytes % _irq_bytes_per_burst;
    if (remainder != 0) {
        memset(finalBurstBuffer, 0, sizeof(finalBurstBuffer));
        memcpy(finalBurstBuffer, (uint8_t*)value + bytes - remainder, remainder);
        _irq_bursts_to_complete++;
    }

    _irq_bytes_remaining = bytes;
    _irq_readPtr = (uint32_t*)value;
    DBGPrintf("arg addr: %x, _irq_readPtr addr: %x \n", value, _irq_readPtr);
    DBGPrintf("bytes per shifter: %u per burst:%u ", _irq_bytes_per_shifter, _irq_bytes_per_burst);
    DBGPrintf("START::_irq_bursts_to_complete: %d _irq_bytes_remaining: %d remainder:%u\n", _irq_bursts_to_complete, _irq_bytes_remaining, remainder);
  
    uint8_t beats = SHIFTNUM * _irq_bytes_per_shifter;
    p->TIMCMP[_flexio_timer] = ((beats * 2U - 1) << 8) | (_baud_div / 2U - 1U);

    p->TIMSTAT = _flexio_timer_mask; // clear timer interrupt signal
    
    asm("dsb");
    
    IRQcallback = this;
    // enable interrupts to trigger bursts
    //print_flexio_debug_data(pFlex, _flexio_timer, _write_shifter, _read_shifter);

//    digitalToggleFast(2);
    p->TIMIEN |= _flexio_timer_mask;
    p->SHIFTSIEN |= (1 << SHIFTER_IRQ);
}

FASTRUN void NT35510_t4x_p::flexIRQ_Callback(){
    //digitalWriteFast(2, HIGH);
    DBGPrintf("%x %x %u %u ", p->TIMSTAT, p->SHIFTSTAT, _irq_bursts_to_complete, _irq_bytes_remaining);
  
 if (p->TIMSTAT & _flexio_timer_mask) { // interrupt from end of burst
        DBGWrite('T');
        p->TIMSTAT = _flexio_timer_mask; // clear timer interrupt signal
        _irq_bursts_to_complete--;
        //if (_irq_bytes_remaining < 32) Serial.printf("T:%u %u\n", _irq_bursts_to_complete, _irq_bytes_remaining);
        if ((_irq_bursts_to_complete == 0) || (_irq_bytes_remaining == 0)) {
            p->TIMIEN &= ~_flexio_timer_mask; // disable timer interrupt
            asm("dsb");
            WR_AsyncTransferDone = true;
            microSecondDelay();
            CSHigh();
            _onCompleteCB();
            //digitalWriteFast(2, LOW);
            DBGPrintf("END\n");
            return;
        }
    }

    if (p->SHIFTSTAT & (1 << SHIFTER_IRQ)) { // interrupt from empty shifter buffer
        DBGWrite('S');
        // note, the interrupt signal is cleared automatically when writing data to the shifter buffers
        if (_irq_bytes_remaining == 0) { // just started final burst, no data to load
            p->SHIFTSIEN &= ~(1 << SHIFTER_IRQ); // disable shifter interrupt signal
        } else if (_irq_bytes_remaining < _irq_bytes_per_burst) { // just started second-to-last burst, load data for final burst
            p->TIMCMP[0] = ((_irq_bytes_remaining * 2U - 1) << 8) | (_baud_div / 2U - 1); // takes effect on final burst
            _irq_readPtr = finalBurstBuffer;
            _irq_bytes_remaining = 0;
            if (_bus_width == 8) {
                for (int i = SHIFTNUM - 1; i >= 0; i--) {
                    //digitalToggleFast(3);
                    uint32_t data = _irq_readPtr[i];
                    p->SHIFTBUFBYS[i] = ((data >> 16) & 0xFFFF) | ((data << 16) & 0xFFFF0000);
                }
            } else {
                uint8_t *pb = (uint8_t*)_irq_readPtr;
                for (int i = SHIFTNUM - 1; i >= 0; i--) {
                    //digitalToggleFast(3);
                    p->SHIFTBUF[i] = (uint32_t)(generate_output_word(pb[2 * i]) << 16) | (uint32_t)(generate_output_word(pb[i * 2 + 1]) << 0);
                }
            }
        } else {
            _irq_bytes_remaining -= _irq_bytes_per_burst;
            // try filling in reverse order
            if (_bus_width == 8) {
                for (int i = SHIFTNUM - 1; i >= 0; i--) {
                    //digitalToggleFast(3);
                    uint32_t data = _irq_readPtr[i];
                    p->SHIFTBUFBYS[i] = ((data >> 16) & 0xFFFF) | ((data << 16) & 0xFFFF0000);
                }
                _irq_readPtr += SHIFTNUM;
            } else {
                uint8_t *pb = (uint8_t*)_irq_readPtr;
                for (int i = SHIFTNUM - 1; i >= 0; i--) {
                    //digitalToggleFast(3);
                    p->SHIFTBUF[i] = (uint32_t)(generate_output_word(pb[2 * i]) << 16) | (uint32_t)(generate_output_word(pb[i * 2 + 1]) << 0);
                }
                pb += (2 * SHIFTNUM);
                _irq_readPtr = (uint32_t*)pb; 
            }
        }
        if (_irq_bytes_remaining == 0) {
            Serial.println("RM==0");
            p->SHIFTSIEN &= ~(1 << SHIFTER_IRQ);
        }
    }
    DBGWrite('\n');
    asm("dsb");
    //digitalWriteFast(2, LOW);

}



FASTRUN void NT35510_t4x_p::flexio_ISR()
{
  asm("dsb");
  IRQcallback->flexIRQ_Callback();
 }
