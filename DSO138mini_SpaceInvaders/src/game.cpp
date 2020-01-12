#include <Math.h>
#include "DSO138mini_SpInv.h"
#include "sprite.h"

// Variables
uint16_t  sLineBuffer[GAMEWIDTH];
T_ENEMIES sEnemies;
T_UFO sUFO;
T_SHIP    sShip;
uint8_t   sShield[4][3*16];


// Wait "PRESS SHOOT"
void gameWaitPressShoot()
{
  dso.drawString((uint8_t*)"PRESS [SHOOT]",GAME_X0+(GAMEWIDTH-13*8)/2, 220, TFT_WHITE,TFT_BLACK,1);
  for(;;){
    uint8_t key = dso.readBtn();
    //Serial.println(key,HEX);
    if(!(key & KEY_SHOOT)){break;}
    delay(50);
  }
}

// Game Title
void gameTitle()
{
  const char * scoreText[] = {"= ? MISTERY", "= 30 POINTS", "= 20 POINTS", "= 10 POINTS" };
  dso.fillRect(0,0,GAMEWIDTH,240,TFT_BLACK);
  for(uint8_t i = 0; i < 4; i++){
  switch(i){
    case 0:
      dso.drawXBitmap(30,40+i*40,UFOWIDTH,7,(byte *)enemyD_data,TFT_RED,TFT_BLACK);
      break;
    default:
      dso.drawXBitmap(30,40+i*40,UFOWIDTH,7,(byte *)p_enemydata[2*i],enemycolor[i],TFT_BLACK);
      break;
  }
  dso.drawString((uint8_t*)scoreText[i],50,40+i*40,TFT_WHITE,TFT_BLACK,1);
  delay(500);
  }
  gameWaitPressShoot();
  dso.fillRect(0,0,GAMEWIDTH,240,TFT_BLACK);
}

// Game Over
void gameLose()
{
  for(uint16_t i = 0; i  < 4; i++){
    dso.drawXBitmap(sShip.x, GAME_Y0+SHIP_Y, SHIPWIDTH, SHIPHEIGHT, (byte *)player_explosion_data[0], TFT_RED, BLACK);
    delay(200);
    dso.drawXBitmap(sShip.x, GAME_Y0+SHIP_Y, SHIPWIDTH, SHIPHEIGHT, (byte *)player_explosion_data[1], TFT_RED, BLACK);
    delay(200);
  }

  for(uint8_t i = 0; i < 5; i++){
    dso.fillRect(0, 120-i*5, GAMEWIDTH, (i+1)*10, TFT_RED);
    delay(100);
  }
  dso.drawString((uint8_t*)"GAME OVER", GAME_X0+(GAMEWIDTH-16*9)/2, 120,TFT_BLACK,TFT_RED,2);
  gameWaitPressShoot();
  gameSetup();
}

// Game Win
void gameWin()
{
  for(uint8_t i = 0; i < 5; i++){
    dso.fillRect(0, 120-i*5, GAMEWIDTH, (i+1)*10, TFT_BLUE);
    delay(300);
  }
  dso.drawString((uint8_t*)"CLEAR!", GAME_X0+(GAMEWIDTH-16*6)/2, 120,TFT_WHITE,TFT_BLUE,2);
  gameWaitPressShoot();
  gameSetup();
}


// UFO
void gameUFOInitial()
{
  sUFO.x = -1;
  sUFO.start = millis() + (20 + random(5))*1000;
}

void gameUFOMove()
{
  if(sUFO.x < 0){
    // Check Triger
    if(sEnemies.num > 7 && millis() > sUFO.start){
      sUFO.dir = (random(2) ? 1:-1);
      sUFO.x = (sUFO.dir > 0) ? 0: (GAMEWIDTH - UFOWIDTH -1);
    }
    return;
  }
  // Move
  sUFO.x += (sUFO.dir * 4);
  if((sUFO.dir < 0 && sUFO.x < UFOWIDTH) || (sUFO.dir &&  sUFO.x >= GAMEWIDTH - UFOWIDTH)){
    gameUFOInitial();
  }
}

// Bullets
void gameShipBulletRemove(uint16_t i)
{
  if(sShip.bltN > 1 && i < sShip.bltN - 1){
    memcpy((void*)&sShip.bullet[i], (void*)&sShip.bullet[sShip.bltN - 1], sizeof(T_BULLET));
  }
  --sShip.bltN;
}

int8_t gameCollisionEnemy(int16_t x, int16_t y)
{
  const uint16_t sc[] = {0,30,20,10};
  int8_t ret = 0;
  uint16_t i, j;
  T_ENEMY  *e;
  for(i = 0; i < sEnemies.num; i++){
    e = &sEnemies.enemy[i];
    if(e->sts >= 2 &&
      (x >= e->x && x < e->x + ENEMYWIDTH) &&
      (y >= e->y && y < e->y + ENEMYHEIGHT)){
      // Remove
      dso.drawXBitmap(e->x, e->y+GAME_Y0, ENEMYWIDTH, ENEMYHEIGHT, 
                          (byte *)enemy_explosion_data, enemycolor[e->sts/2], BLACK);
      delay(200);
      sShip.score += sc[e->sts/2];
      for(j = i; j < sEnemies.num - 1; j++){
        memcpy((void *)&sEnemies.enemy[j], (void *)&sEnemies.enemy[j+1], sizeof(T_ENEMY));
      }
      if(sEnemies.current > i){--sEnemies.current;}
      --sEnemies.num;
      
      ret = i+1; break;
    }
  }
  return ret;
}

int8_t gameCollisionShield(int16_t x, int16_t y, int16_t d)
// d = 0:Ship, 1:Enemy
{
  if((y < SHIELD_Y - SHIPFIREH) || (y >= SHIELD_Y + SHIELDHEIGHT)){return 0;}
  y -= SHIELD_Y;

  int8_t ret = 0;
  int8_t n = 0;
  uint8_t *p;
  int16_t xx, yy, yyy;
  int16_t l, r;
  int16_t h = (d ? ENEMYFIREH + random(3,7):SHIPFIREH);

  for(n = 0; n < 4; n++){
    if((x < shieldX[n]) || (x >= shieldX[n] + SHIELDWIDTH)){continue;}
    x -= shieldX[n];
    p = sShield[n];
  for(yy = 0; yy < h; yy++){
    yyy = y + yy;
    if( yyy < 0 || yyy >= SHIELDHEIGHT){continue;}
    if(p[x/8 + yyy*3] & (0x80>>(x%8))){ret = 1;}
    l = x + (int16_t)random(-2,1);
    if(l < 0){l = 0;}
    r = x + (int16_t)random(2,4);
    if(r >= SHIELDWIDTH){r = SHIELDWIDTH;}
    for(xx = l; xx < r; xx++){
      p[xx/8 + (yyy)*3] &= ~(0x80>>(xx%8));
    }
  }}
  return ret;
}

void gameShipFireMove()
{
  uint16_t i, j;
  T_BULLET *pb;

  for(i = 0; i < sShip.bltN; i++){
    pb = &sShip.bullet[i];
    // UFO Check
    if((pb->x >= sUFO.x) && (pb->x < sUFO.x + UFOWIDTH) && (pb->y < UFOHEIGHT)){
      dso.drawXBitmap(sUFO.x, GAME_Y0, UFOWIDTH, UFOHEIGHT, (byte *)player_explosion_data, TFT_RED, TFT_BLACK);
      delay(200);
      sShip.score += (100 + random(3) * 100);
      gameUFOInitial();
      gameShipBulletRemove(i);continue;  
    }

    // Enemies Check
    if(gameCollisionEnemy(pb->x, pb->y)){
      gameShipBulletRemove(i);continue;
    }
    // Shield Check
    if(gameCollisionShield(pb->x, pb->y, 0)){
      gameShipBulletRemove(i);continue;  
    }
    // Move
    pb->y -= SHIPFIREH;
    if(pb->y < 0){gameShipBulletRemove(i);}
  }
}

void gameEnemyFireTrig(T_ENEMY *e)
{
  if((sEnemies.bltN >= 4) || (e->y > SHIP_Y - ENEMYHEIGHT - 2)){return;}
  T_BULLET *pb = &sEnemies.bullet[sEnemies.bltN];
  pb->x = e->x + ENEMYWIDTH/2;
  pb->y = e->y + ENEMYHEIGHT + 2;
  ++sEnemies.bltN;
  sEnemies.trigFire = random(0, sEnemies.num + sEnemies.bltN*2);
}

void gameEnemyFireRemove(uint16_t i)
{
  if(sEnemies.bltN > 1 && i < sEnemies.bltN - 1){
    memcpy((void*)&sEnemies.bullet[i], (void*)&sEnemies.bullet[sEnemies.bltN - 1], sizeof(T_BULLET));
  }
  --sEnemies.bltN;
}

void gameEnemyFireMove()
{
  uint16_t i, j;
  T_BULLET *pb;
  for(i = 0; i < sEnemies.bltN; i++){
    pb = &sEnemies.bullet[i];
    // Shield
    if(gameCollisionShield(pb->x, pb->y, 1)){
      gameEnemyFireRemove(i);continue;  
    }
    
    // Ship
    if((pb->x >= sShip.x) && (pb->x < sShip.x + SHIPWIDTH) &&
       (pb->y >= SHIP_Y - ENEMYFIREH) && (pb->y < SHIP_Y + SHIPHEIGHT)){
      sShip.dead = 1;
      gameEnemyFireRemove(i);
    }
    // falling
    pb->y += 3;
    if(pb->y > GAMEHEIGHT-ENEMYFIREH){
      gameEnemyFireRemove(i);
    }
  }
}

// Enemies
void gameEnemyInitial()
{
  const uint8_t enemykind[] = {7, 5, 3};
  uint16_t col, raw;
  T_ENEMY *e;
  for(raw = 0; raw < ENEMYRAW; raw++){
    for(col = 0; col < ENEMYCOL; col++){
      e = &sEnemies.enemy[raw*ENEMYCOL + col];
      e->sts = enemykind[raw/2];
      e->x = 16*col;
      e->y = 100 - 16*raw;   
    }
  }
  sEnemies.num = ENEMYNUM;
  sEnemies.current = 0;
  sEnemies.turn = 0;
  sEnemies.down = 0;
  sEnemies.maxy = 0;
  sEnemies.dir = 1;
  sEnemies.trigFire = random(0,sEnemies.num);
  sEnemies.bltN = 0;
}


void gameEnemyMove()
{
  if(sEnemies.current >= sEnemies.num){
    if(sEnemies.turn){
      sEnemies.dir *= -1;
      if(sEnemies.maxy < SHIP_Y - ENEMYHEIGHT){sEnemies.down = 1;}
      sEnemies.maxy = 0;
      sEnemies.turn = 0;
    }
    else if(sEnemies.down){sEnemies.down = 0;}
    sEnemies.trigFire = random(0, sEnemies.num + sEnemies.bltN*2);
    sEnemies.current = 0;
  }
  T_ENEMY *e = &sEnemies.enemy[sEnemies.current];

  e->x += (ENEMYDX * sEnemies.dir);
  if((e->x < ENEMYDX) || (e->x >= GAMEWIDTH - ENEMYWIDTH)){sEnemies.turn = 1;}
  if(sEnemies.down){e->y += 8;}
  if(e->y > sEnemies.maxy){
    sEnemies.maxy = e->y;
    if(e->y + ENEMYHEIGHT > SHIELD_Y){sShip.existShield = 0;};
  }
  if(sEnemies.current == sEnemies.trigFire){gameEnemyFireTrig(e);}
  if((e->x >= sShip.x) && (e->x < sShip.x + SHIPWIDTH) &&
     (e->y > SHIP_Y - ENEMYHEIGHT) && (e->y < SHIP_Y + SHIPHEIGHT)){
    sShip.dead = 1;
  }
  if(constrain(e->sts, 2, 7) == e->sts ){e->sts ^= 1;}
  ++sEnemies.current;
}


// Ship
void gameShipInitial()
{
  sShip.x = (GAMEWIDTH - SHIPWIDTH)/2;
  sShip.bltN = 0;
  sShip.shoot = 0;
}

void gameShipMove()
{
  uint8_t key = dso.readBtn();

  // Left
  if(!(key & KEY_RIGHT) && sShip.x > 0){--sShip.x;}
  // Right
  else if(!(key & KEY_LEFT) && sShip.x < GAMEWIDTH - SHIPWIDTH){++sShip.x;}
  // Shoot
  if(!(key & KEY_SHOOT)){
    if(!sShip.shoot){
      if(sShip.bltN < 4){
        T_BULLET *p = &sShip.bullet[sShip.bltN];
        p->x = sShip.x + SHIPHEIGHT/2;
        p->y = SHIP_Y;
        sShip.bltN++;
      }
      sShip.shoot = 1;
    }
  }else{sShip.shoot = 0;}
}

// Shield
void gameShieldInitial()
{
  for(uint16_t i = 0; i < 4; i++){
    memcpy((void *)&sShield[i], (const void *)shield_data, sizeof(shield_data));
  }
  sShip.existShield = 1;
}

// Dsp
void gameStoreLineBuff(uint16_t *p, uint8_t *in, uint16_t w, uint16_t color)
{
  uint16_t xx = 0;
  while(xx < w){
    if(*in & (0x80 >> (xx%8))){*(p + xx) = color;}
    ++xx; if(!(xx%8)){in++;}
  }
}

void gameDspEnemy(uint16_t *p, uint16_t y)
{
  uint16_t i;
  uint8_t *in;
  T_ENEMY *e;
  for(i = 0; i < sEnemies.num; i++){
    e = &sEnemies.enemy[i];
    if(e->sts){
      if(y >= e->y && y < (e->y + ENEMYHEIGHT)){
        in = (uint8_t *)p_enemydata[e->sts];
        in += ((ENEMYWIDTH/8 + 1) * (y - e->y));
        gameStoreLineBuff(p + e->x, in, ENEMYWIDTH, enemycolor[e->sts/2]);
      }
    }
  }  
}

void gameDspUFO(uint16_t *p, uint16_t y)
{
  if(y >= UFOHEIGHT || sUFO.x < 0){return;} 
  uint8_t *in = (uint8_t *)(enemyD_data + 2 * y);   // UFOWIDTH = 16
  gameStoreLineBuff(p + sUFO.x, in, UFOWIDTH, TFT_RED);
}

void gameDspShip(uint16_t *p, uint16_t y)
{
  if(y < SHIP_Y || y >= SHIP_Y + SHIPHEIGHT){return;} 
  uint8_t *in = (uint8_t *)(player_data + (SHIPWIDTH/8 + 1) * (y - SHIP_Y));
  gameStoreLineBuff(p + sShip.x, in, SHIPWIDTH, TFT_DARKCYAN);
}

void gameDspShield(uint16_t *p, uint16_t y)
{
  if(y < SHIELD_Y || y >= SHIELD_Y + SHIELDHEIGHT || !sShip.existShield){return;}
  for(uint16_t i = 0; i < 4; i++){ 
    uint8_t *in = (uint8_t *)(&sShield[i][(SHIELDWIDTH/8 + 1) * (y - SHIELD_Y)]);
    gameStoreLineBuff(p + shieldX[i], in, SHIELDWIDTH, TFT_RED);
  }
}

void gameDspBullet(uint16_t *p, uint16_t y)
{
  static uint8_t pattern = 0;
  uint16_t i;
  T_BULLET *pb;
  uint8_t *in;
  for(i = 0; i < sShip.bltN; i++){
    pb = &sShip.bullet[i];
    if(y < pb->y || y >= pb->y + SHIPFIREH ){continue;}
    in = (uint8_t *)(&player_fire_data[0][y - pb->y]);
    gameStoreLineBuff(p + pb->x, in, 1, TFT_WHITE);
  }
  for(i = 0; i < sEnemies.bltN; i++){
    pb = &sEnemies.bullet[i];
    if(y < pb->y || y >= pb->y + ENEMYFIREH ){continue;}
    in = (uint8_t *)(&enemy_fire_data[pattern][y - pb->y]);
    pattern ^= 1;
    gameStoreLineBuff(p + pb->x, in, 2, TFT_ORANGE);
  }
}

void gameDsp()
{
  uint16_t  y = 0;
  dso.setAddrWindow(GAME_X0, GAME_Y0, GAME_X0 + GAMEWIDTH-1, GAME_Y0 + GAMEHEIGHT-1);
  while(y < GAMEHEIGHT){
    memset(sLineBuffer, 0, sizeof(sLineBuffer));
    gameDspUFO(sLineBuffer, y);
    gameDspEnemy(sLineBuffer, y);
    gameDspShip(sLineBuffer, y);
    gameDspShield(sLineBuffer, y);
    gameDspBullet(sLineBuffer, y);
    dso.pushColors(sLineBuffer, GAMEWIDTH);
    y++;
  }
  dso.pushColorsEnd();
}


void gameStart()
{
  memset((void *)&sEnemies, 0, sizeof(T_ENEMIES));
  memset((void *)&sUFO, 0, sizeof(T_UFO));
  memset((void *)&sShip, 0, sizeof(T_SHIP));

  gameUFOInitial();
  gameEnemyInitial();
  gameShipInitial();
  gameShieldInitial();

  gameDsp();
}

void gameSetup()
{
/***
	for(int i = 0; i < 256; i++){
    dso.drawXBitmap(192+(i%16)*8,i/16*8,8,8,(byte *)&_vga_font8x8[i*8], WHITE, BLACK);
  }
  for(int i = 0; i < 6; i++){
    dso.drawXBitmap(192+12*i,200,12,8,(byte *)p_enemydata[i], WHITE, BLACK);
  }
***/
  //randomSeed(analogRead());
  dso.fillRect(192, 0, 128, 240, TFT_LIGHTGREY);
  dso.draw_ssd1306(192, 16, 128, 64, (byte *)intro, TFT_LIGHTGREY, BLACK);
  dso.drawFastHLine(GAME_X0, GAME_Y0 + GAMEHEIGHT, GAMEWIDTH, TFT_DARKGREEN);
  gameTitle();
  dso.drawString((uint8_t *)"SCORE", GAMEWIDTH + 20, 120, TFT_BLACK, TFT_LIGHTGREY, 2);
  gameStart();
}

void gameLoop()
{
  static uint8_t sc[8] = "";
  // Move
  gameShipMove();
  gameShipFireMove();
  gameUFOMove();
  gameEnemyMove();
  gameEnemyFireMove();
  // Dsp
  gameDsp();
  // Dsp Score
  sprintf((char *)sc, "%05d", sShip.score);
  sc[6] = 0;
  dso.drawString(sc, GAMEWIDTH + 20, 140, TFT_BLACK, TFT_LIGHTGREY, 2);
  // Game Win?
  if(sEnemies.num < 1){gameWin();}
  if(sShip.dead){gameLose();}
  delay(10);
}


