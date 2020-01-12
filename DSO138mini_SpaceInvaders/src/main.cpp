#include "DSO138mini_SpInv.h"

DSO138mini dso;

void setup() {
  // put your setup code here, to run once:
  /*** For DSO138mini ***/
  afio_cfg_debug_ports(AFIO_DEBUG_NONE);
  // for LED
  pinMode(PA15, OUTPUT);
  // for USB D+
  pinMode(PA7, OUTPUT);
  digitalWrite(PA7,HIGH);
  /**********************/
  Serial.begin(115200);
  //while ( !Serial );
  
  dso.begin();
  dso.fillScreen(BLACK);
  //delay(1000);

  gameSetup();

/*****
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  // test1
  //tft.drawXBitmap(0,0,8,8,(byte *)&_vga_font8x8[8], WHITE, BLACK);
*****/
}

void loop() {
  gameLoop();
  // put your main code here, to run repeatedly:
  //digitalWrite(15,!digitalRead(15));// Turn the LED from off to on, or on to off
  //delay(1000);          // Wait for 1 second (1000 milliseconds)
}