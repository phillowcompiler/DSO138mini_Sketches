// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

#ifndef _DSO138MINI_H_
#define _DSO138MINI_H_

//#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <libmaple/gpio.h>
#include "font8x8.h"

/*****************************************************************************/
// Define pins and Output Data Registers
/*****************************************************************************/
// Data port
#define TFT_DATA_PORT	GPIOB
// Data bits/pins
#define TFT_DATA_SHIFT 0 // take the lower bits/pins 0..7
//#define TFT_DATA_SHIFT 8 // take the higher bits/pins 8..15

//Control pins |RD |WR |RS |CS |RST|
#define TFT_CNTRL_PORT	GPIOC
#define TFT_RD			PB8    //PA0
#define TFT_WR			PC15   //PA1
#define TFT_RS			PC14   //PA2
#define TFT_CS			PC13   //PA3

#define TFT_RD_MASK		BIT8  // digitalPinToBitMask(TFT_RD) // 
#define TFT_WR_MASK		BIT15 // digitalPinToBitMask(TFT_WR) // 
#define TFT_RS_MASK		BIT14 // digitalPinToBitMask(TFT_RS) // 
#define TFT_CS_MASK		BIT13 // digitalPinToBitMask(TFT_CS) // 

#define TFT_RST			PB9   //PB10

#define SLOW_WRITE 0   // set to 1 for legacy slow write (using individual digitalWrite()s

/*****************************************************************************/
// LCD controller chip identifiers
//#define ID_932X    0
//#define ID_7575    1
//#define ID_9341    2
//#define ID_HX8357D    3
//#define ID_9338    4
//#define ID_UNKNOWN 0xFF

/*****************************************************************************/
// for DSO138mini
#define TFTWIDTH   320
#define TFTHEIGHT  240

// Initialization command tables for different LCD controllers
#define TFTLCD_DELAY 0xFF

// For compatibility with sketches written for older versions of library.
// Color function name was changed to 'color565' for parity with 2.2" LCD
// library.
#define Color565 color565

/*****************************************************************************/
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// New color definitions use for all my libraries
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F


	#define RD_ACTIVE    digitalWrite(TFT_RD, LOW)
	#define RD_IDLE      digitalWrite(TFT_RD, HIGH)
#if SLOW_WRITE
	// use old definition, standard bit toggling, low speed
	#define WR_ACTIVE    digitalWrite(TFT_WR, LOW)
	#define WR_IDLE      digitalWrite(TFT_WR, HIGH)
	#define CD_COMMAND   digitalWrite(TFT_RS, LOW)
	#define CD_DATA      digitalWrite(TFT_RS, HIGH)
	#define CS_ACTIVE    digitalWrite(TFT_CS, LOW)
	#define CS_IDLE      digitalWrite(TFT_CS, HIGH)
	#define CS_ACTIVE_CD_COMMAND	{ CS_ACTIVE; CD_COMMAND; }
#else
	// use fast bit toggling, very fast speed!
//extern gpio_reg_map * cntrlRegs;
//	#define WR_ACTIVE				{ cntrlRegs->BRR  = TFT_WR_MASK; }
//	#define WR_IDLE					{ cntrlRegs->BSRR = TFT_WR_MASK; }
//	#define CD_COMMAND				{ cntrlRegs->BRR  = TFT_RS_MASK; }
//	#define CD_DATA					{ cntrlRegs->BSRR = TFT_RS_MASK; }
//	#define CS_ACTIVE				{ cntrlRegs->BRR  = TFT_CS_MASK; }
//	#define CS_IDLE					{ cntrlRegs->BSRR = TFT_CS_MASK; }
//	#define CS_ACTIVE_CD_COMMAND	{ cntrlRegs->BRR  = (TFT_CS_MASK|TFT_RS_MASK); }
	
	// use fast bit toggling, very fast speed!
	#define RD_ACTIVE    { GPIOB->regs->BRR  = TFT_RD_MASK; }
	#define RD_IDLE      { GPIOB->regs->BSRR = TFT_RD_MASK; }
	#define WR_ACTIVE    { GPIOC->regs->BRR  = TFT_WR_MASK; }
	#define WR_IDLE      { GPIOC->regs->BSRR = TFT_WR_MASK; }
	#define CD_COMMAND   { GPIOC->regs->BRR  = TFT_RS_MASK; }
	#define CD_DATA      { GPIOC->regs->BSRR = TFT_RS_MASK; }
	#define CS_ACTIVE    { GPIOC->regs->BRR  = TFT_CS_MASK; }
	#define CS_IDLE      { GPIOC->regs->BSRR = TFT_CS_MASK; }
	#define CS_ACTIVE_CD_COMMAND	{ GPIOC->regs->BRR  = (TFT_CS_MASK|TFT_RS_MASK); }

#endif

#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

extern uint8_t read8_(void);
#define read8(x) ( x = read8_() )

extern gpio_reg_map * dataRegs;

#if (TFT_DATA_SHIFT==0)
  //#warning "Using lower data nibble..."
	// set the pins to input mode
	#define setReadDir() ( dataRegs->CRL = 0x88888888 )	// set the lower 8 bits as input
	// set the pins to output mode
	#define setWriteDir() ( dataRegs->CRL = 0x33333333 )	// set the lower 8 bits as output

    // set pins to output the 8 bit value
    #if SLOW_WRITE
     inline void write8(uint8_t c) { /*Serial.print(" write8: "); Serial.print(c,HEX); Serial.write(',');*/
    					digitalWrite(PB0, (c&BIT0)?HIGH:LOW);
    					digitalWrite(PB1, (c&BIT1)?HIGH:LOW);
    					digitalWrite(PB2, (c&BIT2)?HIGH:LOW);
    					digitalWrite(PB3, (c&BIT3)?HIGH:LOW);
    					digitalWrite(PB4, (c&BIT4)?HIGH:LOW);
    					digitalWrite(PB5, (c&BIT5)?HIGH:LOW);
    					digitalWrite(PB6, (c&BIT6)?HIGH:LOW);
    					digitalWrite(PB7, (c&BIT7)?HIGH:LOW);
    					WR_STROBE; }
    #else
      #define write8(c) { dataRegs->BSRR = (uint32_t)(0x00FF0000 + ((c)&0xFF)); WR_STROBE; }
      // inline void write8(uint8_t d) { dataRegs->BSRR = (uint32_t)(0x00FF0000 + d); WR_STROBE; };
    #endif
#elif (TFT_DATA_SHIFT==8)
  #warning "Using high data nibble..."
	// set the pins to input mode
	#define setReadDir() ( dataRegs->CRH = 0x88888888 )	// set the upper 8 bits as input
	// set the pins to output mode
	#define setWriteDir() ( dataRegs->CRH = 0x33333333 )	// set the lower 8 bits as output

    // set pins to output the 8 bit value
    #if SLOW_WRITE
     inline void write8(uint8_t c) { /*Serial.print(" write8: "); Serial.print(c,HEX); Serial.write(',');*/
    					digitalWrite(PB8, (c&BIT0)?HIGH:LOW);
    					digitalWrite(PB9, (c&BIT1)?HIGH:LOW);
    					digitalWrite(PB10, (c&BIT2)?HIGH:LOW);
    					digitalWrite(PB11, (c&BIT3)?HIGH:LOW);
    					digitalWrite(PB12, (c&BIT4)?HIGH:LOW);
    					digitalWrite(PB13, (c&BIT5)?HIGH:LOW);
    					digitalWrite(PB14, (c&BIT6)?HIGH:LOW);
    					digitalWrite(PB15, (c&BIT7)?HIGH:LOW);
    					WR_STROBE; }
    #else
      #define write8(c) {  dataRegs->BSRR = (uint32_t)(0xFF000000 + (((c)&0xFF)<<TFT_DATA_SHIFT)); WR_STROBE; }
      // inline void write8(uint8_t d) { dataRegs->BSRR = (uint32_t)(0x00FF0000 + d); WR_STROBE; };
    #endif
#else
  #error Invalid data shift selected! Please set to '0' for low nibble, or to '8' for high nibble
#endif


/*****************************************************************************/

#define swap(a, b) { int16_t t = a; a = b; b = t; }

/*****************************************************************************/
class DSO138mini /* : public Adafruit_GFX */{

 public:

  //Adafruit_TFTLCD_8bit_STM32(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t rst);
  DSO138mini(void);

  void     begin(void);
  void     drawPixel(int16_t x, int16_t y, uint16_t color);
  void     drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
  void     drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
  void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
  void     fillScreen(uint16_t color);
  void     reset(void);
  //void     setRegisters8(uint8_t *ptr, uint8_t n);
  //void     setRegisters16(uint16_t *ptr, uint8_t n);
  void     setRotation(uint8_t x);
       // These methods are public in order for BMP examples to work:
  void     setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
  void     invertDisplay(boolean i),
			pushColors(uint16_t *data, int16_t len),
			pushColorsEnd(void),
            drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t * bitmap),
			drawXBitmap(int16_t x, int16_t y, int16_t w, int16_t h, byte *bitmap, uint16_t color, uint16_t bgcolor),
			draw_ssd1306(int16_t x, int16_t y, int16_t w, int16_t h, byte * bitmap, uint16_t color, uint16_t bgcolor),
			drawString(uint8_t *p, uint16_t x, uint16_t y, uint16_t color, uint16_t bgcolor, uint8_t size);
  uint8_t	readBtn(void);
  
/*****************************************************************************/
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
// color coding on bits:
// high byte sill be sent first
// bit nr: 		15	14	13	12	11	 10	09	08		07	06	05	 04	03	02	01	00
// color/bit:	R5	R4	R3	R2	R1 | G5	G4	G3		G2	G1	G0 | B5	B4	B3	B2	B1
// 								R0=R5											B0=B5
/*****************************************************************************/
  uint16_t inline color565(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }

 private:

  void     init(),
           // These items may have previously been defined as macros
           // in pin_magic.h.  If not, function versions are declared:
           //setLR(void),
           flood(uint16_t color, uint32_t len);
  uint8_t  driver;
  uint16_t  _width, _height;
};

//extern uint16_t readReg(uint8_t r);
//extern uint32_t readReg32(uint8_t r);
extern void writeCommand(uint8_t c);
extern void writeRegister8(uint8_t a, uint8_t d);
//extern void writeRegister16(uint16_t a, uint16_t d);
//extern void writeRegister24(uint16_t a, uint32_t d);
//extern void writeRegister32(uint16_t a, uint32_t d);
//extern void writeRegister32(uint16_t a, uint16_t d1, uint16_t d2);
//extern void writeRegisterPair(uint16_t aH, uint16_t aL, uint16_t d);

extern DSO138mini dso;

#endif
