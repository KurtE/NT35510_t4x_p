#ifndef _NT35510_T4X_P_H_
#define _NT35510_T4X_P_H_

// uncomment below the line corresponding to your screen:


#include "Arduino.h"
#include "DMAChannel.h"
#include "FlexIO_t4.h"

#include "Teensy_Parallel_GFX.h"

#define SHIFTNUM 4            // number of shifters used (must be 1, 2, 4, or 8)
#define SHIFTER_DMA_REQUEST (_write_shifter + SHIFTNUM - 1) // only 0, 1, 2, 3 expected to work
#define SHIFTER_IRQ (_write_shifter + SHIFTNUM - 1)

#define FLEXIO_ISR_PRIORITY 64 // interrupt is timing sensitive, so use relatively high priority (supersedes USB)

#define _TFTWIDTH 480  // ILI9488 TFT width in default rotation
#define _TFTHEIGHT 800 // ILI9488 TFT height in default rotation

// Color definitions
#define NT35510_BLACK 0x0000       /*   0,   0,   0 */
#define NT35510_NAVY 0x000F        /*   0,   0, 128 */
#define NT35510_DARKGREEN 0x03E0   /*   0, 128,   0 */
#define NT35510_DARKCYAN 0x03EF    /*   0, 128, 128 */
#define NT35510_MAROON 0x7800      /* 128,   0,   0 */
#define NT35510_PURPLE 0x780F      /* 128,   0, 128 */
#define NT35510_OLIVE 0x7BE0       /* 128, 128,   0 */
#define NT35510_LIGHTGREY 0xC618   /* 192, 192, 192 */
#define NT35510_DARKGREY 0x7BEF    /* 128, 128, 128 */
#define NT35510_BLUE 0x001F        /*   0,   0, 255 */
#define NT35510_GREEN 0x07E0       /*   0, 255,   0 */
#define NT35510_CYAN 0x07FF        /*   0, 255, 255 */
#define NT35510_RED 0xF800         /* 255,   0,   0 */
#define NT35510_MAGENTA 0xF81F     /* 255,   0, 255 */
#define NT35510_YELLOW 0xFFE0      /* 255, 255,   0 */
#define NT35510_WHITE 0xFFFF       /* 255, 255, 255 */
#define NT35510_ORANGE 0xFD20      /* 255, 165,   0 */
#define NT35510_GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define NT35510_PINK 0xF81F


#define NT35510_NOP        0x0000   ///< No-op
#define NT35510_SWRESET    0x0100   ///< Software reset
#define NT35510_RDDID      0x0400   ///< Read display ID (0x0400 - 0x0402)
#define NT35510_RDNUMED    0x0500   ///< Read number of errors (DSI only)
#define NT35510_RDDPM      0x0A00   ///< Read Display Power Mode
#define NT35510_RDDMADCTL  0x0B00   ///< Read Display MADCTL
#define NT35510_RDDCOLMOD  0x0C00   ///< Read Display Pixel Format
#define NT35510_RDDIM      0x0D00   ///< Read Display Image Mode
#define NT35510_RDDSM      0x0E00   ///< Read Display Signal Mode
#define NT35510_RDDSDR     0x0F00   ///< Read Display Self-Diagnostic Result
#define NT35510_SLPIN      0x1000   ///< Enter Sleep Mode
#define NT35510_SLPOUT     0x1100   ///< Sleep Out
#define NT35510_PTLON      0x1200   ///< Partial Mode ON
#define NT35510_NORON      0x1300   ///< Normal Display Mode ON
#define NT35510_INVOFF     0x2000   ///< Display Inversion OFF
#define NT35510_INVON      0x2100   ///< Display Inversion ON
#define NT35510_ALLPOFF    0x2200   ///< All pixels off
#define NT35510_ALLPON     0x2300   ///< All pixels on
#define NT35510_GAMSET     0x2600   ///< Gamma Set
#define NT35510_DISPOFF    0x2800   ///< Display OFF
#define NT35510_DISPON     0x2900   ///< Display ON
#define NT35510_CASET      0x2A00   ///< Column Address Set (0x2A00 - 0x2A03)
#define NT35510_RASET      0x2B00   ///< Row Address Set (0x2B00 - 0x2B03)
#define NT35510_RAMWR      0x2C00   ///< Memory Write
#define NT35510_RAMRD      0x2E00   ///< Memory Read
#define NT35510_PTLAR      0x3000   ///< Partial Area (0x3000 - 0x3003)
#define NT35510_TEOFF      0x3400   ///< Tearing effect line off
#define NT35510_TEON       0x3500   ///< Tearing effect line on
#define NT35510_MADCTL     0x3600   ///< Memory Access Control
#define NT35510_IDMOFF     0x3800   ///< Idle mode off
#define NT35510_IDMON      0x3900   ///< Idle mode on
#define NT35510_COLMOD     0x3A00   ///< Interface pixel format
#define NT35510_RAMWRC     0x3C00   ///< Memory write continue
#define NT35510_RAMRDC     0x3E00   ///< Memory read continue
#define NT35510_STESL      0x4400   ///< Set tearing effect line (0x4400-4401)
#define NT35510_GSL        0x4500   ///< Get scan line (0x4500 - 0x4501)
#define NT35510_DPCKRGB    0x4A00   ///< Display clock in RGB interface
#define NT35510_DSTBON     0x4F00   ///< Deep standby mode on
#define NT35510_WRPFD      0x5000   ///< Write profile value for display
#define NT35510_WRDISBV    0x5100   ///< Write display brightness
#define NT35510_RDDISBV    0x5200   ///< Read display brightness
#define NT35510_WRCTRLD    0x5300   ///< Write CTRL display
#define NT35510_RDCTRLD    0x5400   ///< Read CTRL display
#define NT35510_WRCABC     0x5500   ///< Write content adaptive brightness
#define NT35510_RDCABC     0x5600   ///< Read content adaptive brightness
#define NT35510_WRHYSTE    0x5700   ///< Write hysteresis (0x5700 - 0x573F)
#define NT35510_WRGAMMASET 0x5800   ///< Write gamma setting (0x5800 - 0x5807)
#define NT35510_RDFSVM     0x5A00   ///< Read FS value MSBs
#define NT35510_RDFSVL     0x5B00   ///< Read FS value LSBs
#define NT35510_RDMFFSVM   0x5C00   ///< Read median filter FS value MSBs
#define NT35510_RDMFFSVL   0x5D00   ///< Read median filter FS value LSBs
#define NT35510_WRCABCMB   0x5E00   ///< Write CABC minimum brightness
#define NT35510_RDCABCMB   0x5F00   ///< Read CABC minimum brightness
#define NT35510_WRLSCC     0x6500   ///< Write light sensor comp (0x6500-6501)
#define NT35510_RDLSCCM    0x6600   ///< Read light sensor value MSBs
#define NT35510_RDLSCCL    0x6700   ///< Read light sensor value LSBs
#define NT35510_RDBWLB     0x7000   ///< Read black/white low bits
#define NT35510_RDBkx      0x7100   ///< Read Bkx
#define NT35510_RDBky      0x7200   ///< Read Bky
#define NT35510_RDWx       0x7300   ///< Read Wx
#define NT35510_RDWy       0x7400   ///< Read Wy
#define NT35510_RDRGLB     0x7500   ///< Read red/green low bits
#define NT35510_RDRx       0x7600   ///< Read Rx
#define NT35510_RDRy       0x7700   ///< Read Ry
#define NT35510_RDGx       0x7800   ///< Read Gx
#define NT35510_RDGy       0x7900   ///< Read Gy
#define NT35510_RDBALB     0x7A00   ///< Read blue/acolor low bits
#define NT35510_RDBx       0x7B00   ///< Read Bx
#define NT35510_RDBy       0x7C00   ///< Read By
#define NT35510_RDAx       0x7D00   ///< Read Ax
#define NT35510_RDAy       0x7E00   ///< Read Ay
#define NT35510_RDDDBS     0xA100   ///< Read DDB start (0xA100 - 0xA104)
#define NT35510_RDDDBC     0xA800   ///< Read DDB continue (0xA800 - 0xA804)
#define NT35510_RDFCS      0xAA00   ///< Read first checksum
#define NT35510_RDCCS      0xAF00   ///< Read continue checksum
#define NT35510_RDID1      0xDA00   ///< Read ID1 value
#define NT35510_RDID2      0xDB00   ///< Read ID2 value
#define NT35510_RDID3      0xDC00   ///< Read ID3 value

#define MADCTL_MY 0x80  // Bottom to top
#define MADCTL_MX 0x40  // Right to left
#define MADCTL_MV 0x20  // Row/Column exchange
#define MADCTL_ML 0x10  // LCD refresh Bottom to top
#define MADCTL_RGB 0x00 // Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 // Blue-Green-Red pixel order
#define MADCTL_MH 0x04  // LCD refresh right to left


/****************************************************************************************/
// #define NT35510_CLOCK_READ 30   //equates to 8mhz
//#define NT35510_CLOCK_READ 60 // equates to 4mhz
#define NT35510_CLOCK_READ 120   //equates to 2mhz

enum {
    NT35510 = 0
};

#ifdef __cplusplus
class NT35510_t4x_p : public Teensy_Parallel_GFX {
  public:
    NT35510_t4x_p(int8_t dc, int8_t cs = -1, int8_t rst = -1);
    void begin(uint8_t display_name = NT35510, uint8_t baud_speed_mhz = 20);
    uint8_t getBusSpd();

    static uint32_t color888(uint8_t r, uint8_t g, uint8_t b) __attribute__((always_inline)) {
            return 0xff000000 | (r << 16) | (g << 8) | b;
    } 
    static void color888toRGB(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b)  __attribute__((always_inline)) {
        r = color >> 16; g = color >> 8; b = color; 
    }

    // If used this must be called before begin
    // Set the FlexIO pins.  The first version you can specify just the wr, and read and optionsl first Data.
    // it will use information in the Flexio library to fill in d1-d7
    bool setFlexIOPins(uint8_t write_pin, uint8_t rd_pin, uint8_t tft_d0 = 0xff);

    // Set the FlexIO pins.  Specify all of the pins for 8 bit mode. Must be called before begin
    bool setFlexIOPins(uint8_t write_pin, uint8_t rd_pin, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                       uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

    uint8_t setBitDepth(uint8_t bitDepth);
    uint8_t getBitDepth();

    void setFrameRate(uint8_t frRate);
    uint8_t getFrameRate();

    void setTearingEffect(bool tearingOn);
    bool getTearingEffect();

    void setTearingScanLine(uint16_t scanLine);
    uint16_t getTearingScanLine();

    void setRotation(uint8_t r);
    void invertDisplay(bool invert);
    void displayInfo();

    void pushPixels16bit(const uint16_t *pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void pushPixels16bitDMA(const uint16_t *pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

    uint8_t readCommand(uint16_t const cmd);
    uint32_t readCommandN(uint16_t const cmd, uint8_t count_bytes);

    // Added functions to read pixel data...
    // uint16_t readPixel(int16_t x, int16_t y);
    void readRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors);
    
    // Called by GFX to do updateScreenAsync and new writeRectAsync(;
    bool writeRectAsyncFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors);
    bool writeRectAsyncActiveFlexIO();

    // void pushPixels16bitTearing(uint16_t * pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );
    // void pushPixels24bitTearing(uint16_t * pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );
    void DMAerror();

    bool writeRect24BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint32_t *pixels);

    /**************************************************************/
    void setScroll(uint16_t offset);

    uint16_t _previous_addr_x0 = 0xffff;
    uint16_t _previous_addr_x1 = 0xffff;
    uint16_t _previous_addr_y0 = 0xffff;
    uint16_t _previous_addr_y1 = 0xffff;

    uint16_t generate_output_word(uint8_t data) __attribute__((always_inline)) {
        #if !defined(ARDUINO_TEENSY40)
        return data;
        #else
        if (_bus_width != 10) return data;
        return (uint16_t)(data & 0x0F) | (uint16_t)((data & 0xF0) << 2);
        #endif
    }

    uint8_t read_shiftbuf_byte() __attribute__((always_inline)) {
        #if !defined(ARDUINO_TEENSY40)
        return p->SHIFTBUFBYS[_read_shifter];
        #else
        if (_bus_width == 8) return p->SHIFTBUFBYS[_read_shifter];
        uint16_t data = p->SHIFTBUF[_read_shifter] >> 16; // 10 bits but shifter does 16
        return ((data >> 2) & 0xf0) | (data & 0xf);
        #endif
    }

    void setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    enum { WRITE_SHIFT_TO = 20,
           READ_SHIFT_TO = 20,
           WRITE_TIMER_TO = 20 };
    void waitWriteShiftStat(int error_identifier = 0) __attribute__((always_inline)) {
        elapsedMillis em = 0;
        while (0 == (p->SHIFTSTAT & _write_shifter_mask)) {
            if (em > WRITE_SHIFT_TO) {
                Serial.printf(">>>waitWriteShiftStat(%d) TO\n", error_identifier);
                if (Serial.available()) {
                    while (Serial.read() != -1) {
                    }
                    Serial.println("*** Paused ***");
                    while (Serial.read() == -1) {
                    }
                    while (Serial.read() != -1) {
                    }
                }
                return; // bail
            }
        }
    }

    void waitReadShiftStat(int error_identifier = 0) __attribute__((always_inline)) {
        elapsedMillis em = 0;
        while (0 == (p->SHIFTSTAT & _read_shifter_mask)) {
            if (em > READ_SHIFT_TO) {
                Serial.printf(">>>waitReadShiftStat(%d) TO\n", error_identifier);
                if (Serial.available()) {
                    while (Serial.read() != -1) {
                    }
                    Serial.println("*** Paused ***");
                    while (Serial.read() == -1) {
                    }
                    while (Serial.read() != -1) {
                    }
                }
                return; // bail
            }
        }
    }

    void waitTimStat(int error_identifier = 0) __attribute__((always_inline)) {
        elapsedMillis em = 0;
        while (0 == (p->TIMSTAT & _flexio_timer_mask)) {
            if (em > WRITE_SHIFT_TO) {
                Serial.printf(">>>waitWriteShiftStat(%d) TO\n", error_identifier);
                if (Serial.available()) {
                    while (Serial.read() != -1) {
                    }
                    Serial.println("*** Paused ***");
                    while (Serial.read() == -1) {
                    }
                    while (Serial.read() != -1) {
                    }
                }
                return; // bail
            }
        }
    }

    void beginWrite16BitColors();
    void write16BitColor(uint16_t color);
    void endWrite16BitColors();
//    void write16BitColor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *pcolors, uint16_t count);
    void writeRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);
    void fillRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    typedef void (*CBF)();
    CBF _callback;
    void onCompleteCB(CBF callback);

  protected:
  private:
    uint8_t _display_name = 0;
    FlexIOHandler *pFlex;
    IMXRT_FLEXIO_t *p;
    const FlexIOHandler::FLEXIO_Hardware_t *hw;
    static DMAChannel flexDma;
    static DMASetting _dmaSettings[2];

    uint8_t _baud_div = 20;

    uint8_t _bitDepth = 16;
    uint8_t _rotation = 0;
    uint8_t MADCTL[5];

    uint8_t _frameRate = 60;

    bool _bTearingOn = false;
    uint16_t _tearingScanLine = 0;

    // int16_t _width, _height;
    int8_t _dc, _cs, _rst;

    // The Teensy IO pins used for data and Read and Write
    uint8_t _data_pins[8], _wr_pin, _rd_pin;

    uint8_t _flexio_D0, _flexio_WR, _flexio_RD; // which flexio pins do they map to
    uint8_t _write_shifter = 0;
    uint8_t _write_shifter_mask = (1 << 0);
    uint8_t _read_shifter = 3;
    uint8_t _bus_width = 8;
    uint8_t _read_shifter_mask = (1 << 3);
    uint8_t _flexio_timer = 0;
    uint8_t _flexio_timer_mask = 1 << 0;

    uint8_t _dummy;
    uint8_t _curMADCTL;

    volatile bool WR_AsyncTransferDone = true;
    uint32_t MulBeatCountRemain;
    uint16_t *MulBeatDataRemain;
    uint32_t TotalSize;

    void displayInit(uint8_t display_name);
    void CSLow();
    void CSHigh();
    void DCLow();
    void DCHigh();
    void gpioWrite();
    void gpioRead();

    void FlexIO_Init();
    typedef enum { CONFIG_CLEAR = 0,
                   CONFIG_SNGLBEAT,
                   CONFIG_MULTIBEAT,
                   CONFIG_SNGLREAD } Flexio_config_state_t;
    Flexio_config_state_t flex_config = CONFIG_CLEAR;
    void FlexIO_Config_SnglBeat();
    void FlexIO_Clear_Config_SnglBeat();
    void FlexIO_Config_MultiBeat();
    void FlexIO_Config_SnglBeat_Read();

    void writeRegM(uint16_t cmd, uint8_t len, uint8_t data[]);
    void write_command_and_data(uint32_t cmd, uint8_t val);
    void output_command_helper(uint32_t cmd);
    void SglBeatWR_nPrm_8(uint32_t const cmd, uint8_t const *value, uint32_t const length);
    void SglBeatWR_nPrm_16(uint32_t const cmd, const uint16_t *value, uint32_t const length);
    // Works on FlexIO1 and FlexIO2 but not 3 and only on Shifters 0-3
    void MulBeatWR_nPrm_DMA(uint32_t const cmd, const void *value, uint32_t const length);

    // Works on FlexIO3 and others as well
    void MulBeatWR_nPrm_IRQ(uint32_t const cmd,  const void *value, uint32_t const length);
    static void flexio_ISR();
    void flexIRQ_Callback();

    void microSecondDelay();

    static void dmaISR();
    void flexDma_Callback();
    static NT35510_t4x_p *IRQcallback;

    bool isCB = false;
    void _onCompleteCB();

    static NT35510_t4x_p *dmaCallback;

    /* variables used by ISR */
    volatile uint32_t _irq_bytes_remaining;
    volatile unsigned int _irq_bursts_to_complete;
    volatile uint32_t *_irq_readPtr;
    uint8_t  _irq_bytes_per_shifter;
    uint16_t _irq_bytes_per_burst;
    uint32_t finalBurstBuffer[SHIFTNUM];

};
#endif //__cplusplus
#endif //_INT35510_t4x_p.h_