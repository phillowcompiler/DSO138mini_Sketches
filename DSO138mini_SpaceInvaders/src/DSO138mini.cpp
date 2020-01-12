// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

#include "DSO138mini.h"

gpio_reg_map * cntrlRegs;
gpio_reg_map * dataRegs;

/*****************************************************************************/
// Constructor
/*****************************************************************************/
DSO138mini :: DSO138mini(void)
//: Adafruit_GFX(TFTWIDTH, TFTHEIGHT)
{
  this->_width = TFTWIDTH;
  this->_height = TFTHEIGHT;
}

/*****************************************************************************/
// for TFT_LCD on DSO138mini(ST7787)
//
static const uint8_t DSO138mini_regValues[] PROGMEM = {
	0x11, 0,
	TFTLCD_DELAY, 125,

  // for DSO138mini(MY,MX,MV,ML,RGB,MH,0,0)
	0x36, 1, ((1<<7)|(0<<6)|(1<<5)|(0<<4)|(0<<3)|(0<<2)),
  // RGB565 color order
	0x3A, 1, 0x05,
	0x29, 0,
};

void DSO138mini::begin(void)
{
	/*** For DSO138mini ***/
  afio_cfg_debug_ports(AFIO_DEBUG_NONE);
  // for LED
  pinMode(PA15, OUTPUT);
  // for USB D+
  pinMode(PA7, OUTPUT);
  digitalWrite(PA7,HIGH);
	
	/* Initialize LCD(ST7787) */
	reset();
	
	uint8_t i = 0;
  CS_ACTIVE;
  while(i < sizeof(DSO138mini_regValues)) {
    uint8_t r = pgm_read_byte(&DSO138mini_regValues[i++]);
    uint8_t len = pgm_read_byte(&DSO138mini_regValues[i++]);
    if(r == TFTLCD_DELAY) {
      delay(len);
    } else {
		  //CS_ACTIVE;
		  CD_COMMAND;
		  write8(r);
		  CD_DATA;
		  for (uint8_t d=0; d<len; d++) {
		    uint8_t x = pgm_read_byte(&DSO138mini_regValues[i++]);
		    write8(x);
		  }
    }
  }
  CS_IDLE;
}

/*****************************************************************************/
void  DSO138mini::reset(void)
{
	cntrlRegs = TFT_CNTRL_PORT->regs;
	dataRegs = TFT_DATA_PORT->regs;
	//Set control lines as output
	//cntrlRegs->CRL = (cntrlRegs->CRL & 0xFFFF0000) | 0x00003333;
	pinMode(TFT_RD, OUTPUT);
	pinMode(TFT_WR, OUTPUT);
	pinMode(TFT_RS, OUTPUT);
	pinMode(TFT_CS, OUTPUT);
	CS_IDLE; // Set all control bits to HIGH (idle)
	CD_DATA; // Signals are ACTIVE LOW
	WR_IDLE;
	RD_IDLE;

	//set up 8 bit parallel port to write mode.
	setWriteDir();

	// toggle RST low to reset
	if (TFT_RST > 0) {
		pinMode(TFT_RST, OUTPUT);
		digitalWrite(TFT_RST, HIGH);
		delay(100);
		digitalWrite(TFT_RST, LOW);
		delay(100);
		digitalWrite(TFT_RST, HIGH);
		delay(100);
	}
}

/*****************************************************************************/
// Sets the LCD address window (and address counter, on 932X).
// Relevant to rect/screen fills and H/V lines.  Input coordinates are
// assumed pre-sorted (e.g. x2 >= x1).
/*****************************************************************************/
void DSO138mini::setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	writeCommand(0x2A);
	CD_DATA;
	write8(x1>>8);
	write8(x1);
	write8(x2>>8);
	write8(x2);
	//CS_IDLE;
	
	//CS_ACTIVE
	writeCommand(0x2B);
	CD_DATA;
	write8(y1>>8);
	write8(y1);
	write8(y2>>8);
	write8(y2);
	//CS_IDLE;

  writeCommand(0x2c);
  CD_DATA;
}

/*****************************************************************************/
// Fast block fill operation for fillScreen, fillRect, H/V line, etc.
// Requires setAddrWindow() has previously been called to set the fill
// bounds.  'len' is inclusive, MUST be >= 1.
/*****************************************************************************/
void DSO138mini::flood(uint16_t color, uint32_t len)
{
  while (len--) { // write here the remaining data
    write8(color >> 8);
    write8(color);
  }
}

/*****************************************************************************/
void DSO138mini::drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t x2;

  // Initial off-screen clipping
  if((length <= 0     ) ||
     (y      <  0     ) || ( y                  >= _height) ||
     (x      >= _width) || ((x2 = (x+length-1)) <  0      )) return;

  if(x < 0) {        // Clip left
    length += x;
    x       = 0;
  }
  if(x2 >= _width) { // Clip right
    x2      = _width - 1;
    length  = x2 - x + 1;
  }

  setAddrWindow(x, y, x2, y);
  flood(color, length);
  CS_IDLE;
}

/*****************************************************************************/
void DSO138mini::drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t y2;

  // Initial off-screen clipping
  if((length <= 0      ) ||
     (x      <  0      ) || ( x                  >= _width) ||
     (y      >= _height) || ((y2 = (y+length-1)) <  0     )) return;
  if(y < 0) {         // Clip top
    length += y;
    y       = 0;
  }
  if(y2 >= _height) { // Clip bottom
    y2      = _height - 1;
    length  = y2 - y + 1;
  }

  setAddrWindow(x, y, x, y2);
  flood(color, length);
  CS_IDLE;
}

/*****************************************************************************/
void DSO138mini::fillRect(int16_t x1, int16_t y1, int16_t w, int16_t h, uint16_t fillcolor)
{
	//Serial.println("\n::fillRect...");
  int16_t  x2, y2;

  // Initial off-screen clipping
  if( (w            <= 0     ) ||  (h             <= 0      ) ||
      (x1           >= _width) ||  (y1            >= _height) ||
     ((x2 = x1+w-1) <  0     ) || ((y2  = y1+h-1) <  0      )) return;
  if(x1 < 0) { // Clip left
    w += x1;
    x1 = 0;
  }
  if(y1 < 0) { // Clip top
    h += y1;
    y1 = 0;
  }
  if(x2 >= _width) { // Clip right
    x2 = _width - 1;
    w  = x2 - x1 + 1;
  }
  if(y2 >= _height) { // Clip bottom
    y2 = _height - 1;
    h  = y2 - y1 + 1;
  }

  setAddrWindow(x1, y1, x2, y2);
  flood(fillcolor, (uint32_t)w * (uint32_t)h);
  CS_IDLE;
}

/*****************************************************************************/
void DSO138mini::fillScreen(uint16_t color)
{
  fillRect(0, 0, TFTWIDTH, TFTHEIGHT, color);
}

/*****************************************************************************/
void DSO138mini::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  // Clip
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  setAddrWindow(x, y, x+1, y+1);
  write8(color>>8);
  write8(color);
}

/*****************************************************************************/
// Draw an image bitmap (16bits per color) at the specified position from the provided buffer.
/*****************************************************************************/
void DSO138mini::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t * bitmap)
{
	if ( x>=0 && (x+w)<_width && y>=0 && (y+h)<_height ) {
		// all pixel visible, do it in the fast way
		setAddrWindow(x,y,x+w-1,y+h-1);
		pushColors((uint16_t*)bitmap, w*h);
	} else {
		// some pixels outside visible area, do it in the classical way to disable off-screen points
		int16_t i, j;
		uint16_t * colorP = (uint16_t*)bitmap;
		for(j=0; j<h; j++) {
			for(i=0; i<w; i++ ) {
				drawPixel(x+i, y+j, *colorP++);
			}
		}
	}
  CS_IDLE;
}

void DSO138mini::drawXBitmap(int16_t x, int16_t y, int16_t w, int16_t h, byte *bitmap, uint16_t color, uint16_t bgcolor)
{
  uint16_t xx, yy, c;

  if ( x < 0 || (x+w) > _width || y < 0 || (y+h) > _height ) {return;}
  setAddrWindow(x, y, x+w-1, y+h-1);
  for(yy = 0; yy < h; yy++){
    for(xx = 0; xx < w; xx++){
      c = (*(bitmap + (xx/8) + yy*(w/8)) & 0x80>>(xx%8)) ? color:bgcolor;
      write8(c>>8);
      write8(c);
    }
  }
  CS_IDLE;
}

void DSO138mini::draw_ssd1306(int16_t x, int16_t y, int16_t w, int16_t h, byte *bitmap, uint16_t color, uint16_t bgcolor)
{
  uint16_t c, xx, yy;
  if ( x < 0 || (x+w) > _width || y < 0 || (y+h) > _height ) {return;}
  setAddrWindow(x, y, x+w-1, y+h-1);
  for(yy = 0; yy < h; yy++){
    for(xx = 0; xx < w; xx++){
      c = (*(bitmap + xx + (yy/8)*w) & (1<<(yy%8))) ? color:bgcolor;
      write8(c>>8);
      write8(c);
    }
  }
  CS_IDLE;
}

void DSO138mini::drawString(uint8_t *str, uint16_t x, uint16_t y, uint16_t color, uint16_t bgcolor, uint8_t size)
// draw 8x8 String
{
  uint8 xx,yy,xs,ys;
  uint8_t *p;
  uint16_t c;
  if(!size){size = 1;}
  while(*str){
    if ( x < 0 || x > _width || y < 0 || y > _height ) {break;}
    p = (uint8_t *)&_vga_font8x8[*str * 8];
    setAddrWindow(x, y, x+8*size-1, y+8*size-1);
    for(yy = 0; yy < 8; yy++){
      for(ys = 0; ys < size; ys++){
        for(xx = 0; xx < 8; xx++){
          c = (*p & (0x80 >> xx)) ? color:bgcolor;
          for(xs = 0; xs < size; xs++){write8(c>>8);write8(c);}
        }
      }
      p++; 
    }
    CS_IDLE;
    x += 8*size;
    str++;
  }
}

/*****************************************************************************/
// Issues 'raw' an array of 16-bit color values to the LCD; used
// externally by BMP examples.  Assumes that setWindowAddr() has
// previously been set to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).
/*****************************************************************************/
void DSO138mini::pushColors(uint16_t *data, int16_t len)
{
  while(len--) {
    uint16_t color = *data++;
    write8(color >> 8);         // going on.
    write8(color);
  }
}

void DSO138mini::pushColorsEnd(void)
{
  CS_IDLE;
}

/*****************************************************************************/
void DSO138mini::invertDisplay(boolean i)
{
	
}
/*****************************************************************************/
void DSO138mini::setRotation(uint8_t x)
{
  // Call parent rotation func first -- sets up rotation flags, etc.
  //Adafruit_GFX::setRotation(x);
  // Then perform hardware-specific rotation operations...
}

/*****************************************************************************/
uint8_t read8_(void)
{
  RD_ACTIVE;
  delayMicroseconds(10);
  uint8_t temp = ( (dataRegs->IDR>>TFT_DATA_SHIFT) & 0x00FF);
  delayMicroseconds(10);
  RD_IDLE;
  delayMicroseconds(10);
  return temp;
}

//
// For DSO138mini Button(PB4-7)
//
uint8_t DSO138mini::readBtn(void)
{
  uint8_t x;
  CS_IDLE;
  setReadDir();
  delay(1);
  read8(x);
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration
  return (x & 0xf0);
}


/*****************************************************************************/
inline void writeCommand(uint8_t c)
{
	CS_ACTIVE_CD_COMMAND;
	write8(c);
}

/*****************************************************************************/
void writeRegister8(uint8_t a, uint8_t d)
{
  writeCommand(a);
  CD_DATA;
  write8(d);
  //CS_IDLE;
}
