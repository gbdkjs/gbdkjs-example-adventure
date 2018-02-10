#include "game.h"
#include "assets.h"

UBYTE prev_joy;
UBYTE time;
UBYTE running;
POS player_pos;
VEC2D player_vel;
VEC2D player_dir;
ENEMY enemies[NUM_ENEMIES];
BULLET bullets[NUM_BULLETS];
UBYTE frame;

void main()
{
  // Init LCD
  LCDC_REG = 0x67;
  set_interrupts(VBL_IFLAG | LCD_IFLAG);
  STAT_REG = 0x45;
 
  // Set palettes
  BGP_REG = 0xE4U;
  OBP0_REG = 0xD2U;

  // Hide Window
  WX_REG = MAXWNDPOSX;
  WY_REG = MAXWNDPOSY;

  // Initialize the background
  set_bkg_data(0, 184, tileset);
  set_bkg_tiles(0, 0, 32, 32, screen_tiles);

  init_sprites();

  player_pos.x = 48;
  player_pos.y = 48;
  player_dir.y = 1;

  init_enemies();

  running = TRUE;

  position_player();

  SHOW_SPRITES;
  DISPLAY_ON;  
    
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(game_loop, 60, 1);
#else
  while(1)
  {
    game_loop();
  }
#endif
}

void init_sprites()
{
  UBYTE i;

  // Initialize the sprite graphics
  set_sprite_data(0, 28, player_sprites);
  set_sprite_data(28, 24, monster_sprites);
  set_sprite_data(BULLET_SPRITE_TILE, 8, bullet_sprites);

  // Player
  set_sprite_tile(PLAYER_SPRITE_INDEX, 0);
  set_sprite_tile(PLAYER_SPRITE_INDEX + 1, 2);  
  set_sprite_tile(PLAYER_SPRITE_INDEX + 2, 8);  

  // Enemies
  for(i=0; i<NUM_ENEMIES; i++) {
    set_sprite_tile(FIRST_ENEMY_SPRITE_INDEX + (i*2), 28);
    set_sprite_tile(FIRST_ENEMY_SPRITE_INDEX + (i*2) + 1, 30);    
  }

  // Bullets
  for(i=0; i<NUM_ENEMIES; i++) {
    set_sprite_tile(FIRST_BULLET_SPRITE_INDEX + i, 53);
  }
}

void init_enemies()
{
  enemies[0].enabled = TRUE;
  enemies[0].hp = ENEMY_HP;
  enemies[0].pos.x = 128;
  enemies[0].pos.y = 128;  

  enemies[1].enabled = TRUE;
  enemies[1].hp = ENEMY_HP;
  enemies[1].pos.x = 182;
  enemies[1].pos.y = 182;

  enemies[2].enabled = TRUE;
  enemies[2].hp = ENEMY_HP;
  enemies[2].pos.x = 56;
  enemies[2].pos.y = 192;

  enemies[3].enabled = TRUE;
  enemies[3].hp = ENEMY_HP;
  enemies[3].pos.x = 182;
  enemies[3].pos.y = 56;    

  enemies[4].enabled = TRUE;
  enemies[4].hp = ENEMY_HP;
  enemies[4].pos.x = 182;
  enemies[4].pos.y = 120;
}

void shoot()
{
  UBYTE i;

  for(i=0; i<=NUM_BULLETS; i++) {
    if(!bullets[i].enabled) {
      break;
    }
  }

  // No available enemies (all on screen)
  if(i==NUM_BULLETS) {
    return;
  }

  bullets[i].enabled = TRUE;
  bullets[i].pos.x = player_pos.x;
  bullets[i].pos.y = player_pos.y;
  bullets[i].dir.x = 2*player_dir.x;
  bullets[i].dir.y = 2*player_dir.y;   
}

void update_sprites()
{
  UBYTE i;

  frame++;

  if(player_dir.x == 0) {
    if(player_dir.y > 0) {
      set_sprite_prop(0, 0);
      set_sprite_prop(1, 0);
      set_sprite_tile(0, 12 + 4*(frame%2));
      set_sprite_tile(1, 14 + 4*(frame%2));  
    } else if(player_dir.y < 0) {
      set_sprite_prop(0, 0);
      set_sprite_prop(1, 0);
      set_sprite_tile(0, 20 + 4*(frame%2));
      set_sprite_tile(1, 22 + 4*(frame%2));              
    } else {
      set_sprite_prop(0, 0);
      set_sprite_prop(1, 0);
      set_sprite_tile(0, 0);
      set_sprite_tile(1, 2);
    }           
  } else {
    if(player_dir.x >= 0) {
      set_sprite_prop(0, 0);
      set_sprite_prop(1, 0);
      set_sprite_tile(0, 4 + 4*(frame%2));
      set_sprite_tile(1, 6 + 4*(frame%2));  
    } else {
      set_sprite_prop(0, S_FLIPX);
      set_sprite_prop(1, S_FLIPX);
      set_sprite_tile(0, 6 + 4*(frame%2));  
      set_sprite_tile(1, 4 + 4*(frame%2));
    }         
  } 

  for(i=0; i<NUM_ENEMIES; i++) {
    if(!enemies[i].enabled) {
      continue;
    }
    set_sprite_tile(FIRST_ENEMY_SPRITE_INDEX + (i*2), 28 + 4*(frame%2));  
    set_sprite_tile(FIRST_ENEMY_SPRITE_INDEX + (i*2) + 1, 30 + 4 * (frame%2));
  }
}

void update_enemies()
{
  UBYTE i, b;

  for(i=0; i<NUM_ENEMIES; i++) {
    if(!enemies[i].enabled) {
      continue;
    }

    // Update position
    if(((time+i)&15)==0) {
      if(enemies[i].pos.x > player_pos.x) {
        enemies[i].pos.x -= 1;    
      } else if(enemies[i].pos.x < player_pos.x) {
        enemies[i].pos.x += 1;    
      }
      if(enemies[i].pos.y > player_pos.y) {
        enemies[i].pos.y -= 1;    
      } else if(enemies[i].pos.y < player_pos.y) {
        enemies[i].pos.y += 1;    
      }
    }

    // Check bullet collision
    for(b=0; b<NUM_BULLETS; b++) {
      if(!bullets[b].enabled) {
        continue;
      }    
      if(is_collision(&enemies[i].pos, &bullets[b].pos, 8)) {
        LOG("HIT ENEMY %d\n", i);

        if(enemies[i].hp > 1) {
          enemies[i].hp--;
        } else {
          enemies[i].enabled = FALSE;
          hide_sprite(FIRST_ENEMY_SPRITE_INDEX + (i*2));          
        }

        bullets[b].enabled = FALSE;
        move_sprite(FIRST_BULLET_SPRITE_INDEX + b, 0, 0);
      }
    }

    // Check player collision
    if(is_collision(&enemies[i].pos, &player_pos, 12)) {
      game_over();
    }            
    
  }    
}

void update_bullets()
{
  UBYTE i;
  for(i=0; i<NUM_BULLETS; i++) {
    if(!bullets[i].enabled) {
      continue;
    }
    bullets[i].pos.x+=bullets[i].dir.x;
    bullets[i].pos.y+=bullets[i].dir.y;

    // Check bullet hasn't left stage
    if(bullets[i].pos.x + 8 < STAGE_LEFT 
      || bullets[i].pos.x > STAGE_RIGHT
      || bullets[i].pos.y < STAGE_TOP
      || bullets[i].pos.y - 8 > STAGE_BOTTOM) {
        LOG("HIDE BULLET\n");
        bullets[i].enabled = FALSE;
        move_sprite(FIRST_BULLET_SPRITE_INDEX + i, 0, 0);
    }    
  }  
}

void position_camera()
{
  if(player_pos.x < 80) {
    SCX_REG = 0;
  } else if (player_pos.x >= 175) {
    SCX_REG = 95;
  } else {
    SCX_REG = player_pos.x - 80;    
  }

  if(player_pos.y < 72) {
    SCY_REG = 0;
  } else if (player_pos.y >= 183) {
    SCY_REG = 111;
  } else {
    SCY_REG = player_pos.y - 72;    
  }
}

void position_player()
{
  move_sprite(0, player_pos.x - SCX_REG, player_pos.y - SCY_REG);  
  move_sprite(1, player_pos.x + 8 - SCX_REG, player_pos.y - SCY_REG);
}

void position_enemies()
{
  UBYTE i;
  for(i=0; i<NUM_ENEMIES; i++) {
    if(!enemies[i].enabled) {
      continue;
    }
    // Move sprite
    move_sprite(FIRST_ENEMY_SPRITE_INDEX + (i*2), enemies[i].pos.x - SCX_REG, enemies[i].pos.y - SCY_REG);
    move_sprite(FIRST_ENEMY_SPRITE_INDEX + (i*2) + 1, enemies[i].pos.x + 8 - SCX_REG, enemies[i].pos.y - SCY_REG);    
  }
}

void position_bullets()
{
  UBYTE i;
  for(i=0; i<NUM_BULLETS; i++) {
    if(!bullets[i].enabled) {
      continue;
    }
    // Move sprite
    move_sprite(
      FIRST_BULLET_SPRITE_INDEX + i,
      bullets[i].pos.x + 4 - SCX_REG,
      bullets[i].pos.y - SCY_REG
    );
  }  
}

void hide_sprite(UBYTE i)
{
  move_sprite(i, 0, 0);
  move_sprite(i + 1, 0, 0);      
}

UBYTE is_collision(POS *a, POS *b, UBYTE size)
{
  return (a->x > b->x - size)
    && (a->x < b->x + size)
    && (a->y > b->y - size)
    && (a->y < b->y + size);
}

void check_for_win()
{
  UBYTE i, did_win;
  did_win = TRUE;
  for(i=0; i<NUM_ENEMIES; i++) {
    if(enemies[i].enabled) {
      did_win = FALSE;
      break;
    }
  } 
  if(did_win) {
    game_win();
  } 
}

void game_win() {
  DISPLAY_OFF;
  HIDE_SPRITES;
  running = FALSE;
  set_bkg_tiles(0, 0, 20, 18, win_tiles);
  SCX_REG = 0;
  SCY_REG = 0;
  DISPLAY_ON;    
}

void game_over()
{
  DISPLAY_OFF;
  HIDE_SPRITES;
  running = FALSE;
  set_bkg_tiles(0, 0, 20, 18, gameover_tiles);
  SCX_REG = 0;
  SCY_REG = 0;
  DISPLAY_ON;
}

void game_loop()
{
  UBYTE joy;
  
#ifdef __EMSCRIPTEN__    
  update_registers(SCX_REG, SCY_REG, WX_REG, WY_REG, LYC_REG, BGP_REG, OBP0_REG, OBP1_REG);
#endif
  wait_vbl_done();

  joy = joypad();

  if(running) {

    // Input
    if(joy & J_LEFT) {
      player_vel.x = -1;
      player_dir.x = -1;
    } else if (joy & J_RIGHT) {
      player_vel.x = 1;
      player_dir.x = 1; 
    } else {
      player_vel.x = 0;
      if((joy & J_UP) || (joy & J_DOWN)) {
        player_dir.x = 0;
      }            
    }

    if(joy & J_UP) {
      player_vel.y = -1;
      player_dir.y = -1;          
    } else if (joy & J_DOWN) {
      player_vel.y = 1;
      player_dir.y = 1;
    } else {
      player_vel.y = 0;
      if((joy & J_LEFT) || (joy & J_RIGHT)) {
        player_dir.y = 0;
      }            
    }

    // Position
    player_pos.x += player_vel.x;
    player_pos.y += player_vel.y;

    // Stage Bounds
    if(player_pos.x < STAGE_LEFT) {
      player_pos.x = STAGE_LEFT;
    } else if (player_pos.x + 8 > STAGE_RIGHT) {
      player_pos.x = STAGE_RIGHT - 8;
    }

    if(player_pos.y < STAGE_TOP) {
      player_pos.y = STAGE_TOP;
    } else if (player_pos.y > STAGE_BOTTOM) {
      player_pos.y = STAGE_BOTTOM;
    }

    if(joy & J_A && !(prev_joy & J_A)) { // If pressing A but wasn't last frame
      shoot();
    }

    // Update sprites (every 8 frames)
    if((time&0x7)==0) {
      update_sprites();
    }

    update_enemies();
    update_bullets();

    if(running) {
      position_camera();
      position_player();
      position_enemies();
      position_bullets();
      check_for_win();  
    }      
  }

  prev_joy = joy;
  time++;
}
