#include <Arduino.h>
//#include <SPI.h>
#include "DSO138mini.h"


// define
#define GAMEWIDTH 192  /* 8x24 */
#define GAMEHEIGHT 180
#define GAME_X0 0
#define GAME_Y0 30

#define KEY_SHOOT 0x20
#define KEY_LEFT 0x40
#define KEY_RIGHT 0x80

#define ENEMYCOL 11
#define ENEMYRAW 5
#define ENEMYNUM ENEMYCOL*ENEMYRAW
#define ENEMYBULLETMAX 4
#define ENEMYDX 4
#define ENEMYWIDTH 12
#define ENEMYHEIGHT 8
#define ENEMYFIREH  7
#define UFOWIDTH 16
#define UFOHEIGHT 7
#define SHIPNUM 3
#define SHIPWIDTH 14
#define SHIPHEIGHT 8
#define SHIP_Y 170
#define SHIELDWIDTH 22
#define SHIELDHEIGHT 16
#define SHIELD_Y 150
#define SHIPFIREH 6
const uint16_t shieldX[] = {20, 62, 106, 148};

// typedef
typedef struct{
  int16_t x;
  int16_t y;  
}T_BULLET;

typedef struct{
  uint16_t x;
  uint16_t y;
  uint8_t sts;
}T_ENEMY;

typedef struct{
  uint8_t num;
  uint8_t current;
  int8_t  dir;
  uint8_t turn;
  uint8_t down;
  uint16_t maxy;
  T_ENEMY enemy[ENEMYNUM];
  uint8_t trigFire;
  uint8_t bltN;
  T_BULLET bullet[ENEMYBULLETMAX];    
}T_ENEMIES;

typedef struct{
    int16_t x;
    int8_t  dir;
    unsigned long start;
}T_UFO;

#define SHIPBULLETMAX 4
typedef struct{
  uint8_t  num;
  uint16_t x;
  uint8_t shoot;
  uint8_t bltN;
  T_BULLET bullet[SHIPBULLETMAX];
  uint8_t existShield;
  uint8_t dead;
  uint16_t score;
}T_SHIP;

extern DSO138mini dso;

void gameSetup();
void gameLoop();