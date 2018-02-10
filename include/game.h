#ifndef GAME_H
#define GAME_H

#include <gbdkjs.h>
#include <gb/gb.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define ENEMY_HP 8
#define NUM_ENEMIES 8
#define NUM_BULLETS 4
#define PLAYER_SPRITE_I 0
#define ENEMY_SPRITE_I 2
#define BULLET_SPRITE_I 18
#define BULLET_SPRITE_TILE 52
#define STAGE_LEFT 40
#define STAGE_TOP 40
#define STAGE_RIGHT 224
#define STAGE_BOTTOM 208
#define STAGE_TILES_WIDTH 32
#define STAGE_TILES_HEIGHT 32

#define PLAYER_SPRITE_INDEX 0
#define FIRST_ENEMY_SPRITE_INDEX 3
#define FIRST_BULLET_SPRITE_INDEX 13

typedef struct _POS {
  UBYTE x;
  UBYTE y;
} POS;

typedef struct _VEC2D {
  BYTE x;
  BYTE y;
} VEC2D;

typedef struct _ENEMY {
  POS pos;
  UBYTE hp;  
  UBYTE enabled;
} ENEMY;

typedef struct _BULLET {
  POS pos;
  VEC2D dir;
  UBYTE enabled;
} BULLET;

void game_loop();
void check_for_win();
void game_win();
void game_over();
void init_sprites();
void init_enemies();
void shoot();
void update_sprites();
void update_bullets();
void update_enemies();
void hide_sprite(UBYTE i);
UBYTE is_collision(POS *a, POS *b, UBYTE size);
void position_player();
void position_enemies();
void position_bullets();

#endif
