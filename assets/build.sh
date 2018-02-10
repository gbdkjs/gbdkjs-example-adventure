#!/bin/bash

ASSET_FILE=../include/assets.h

# Build tilesets
ggbgfx tileset screen.png win.png gameover.png -o tileset.png

# Build sprites
ggbgfx sprite player_sprites.png > $ASSET_FILE
ggbgfx sprite monster_sprites.png >> $ASSET_FILE
ggbgfx sprite bullet_sprites.png >> $ASSET_FILE

# Build tiledata
ggbgfx tiledata tileset.png >> $ASSET_FILE

# Build tilemaps
ggbgfx tilemap -n screen_tiles screen.png tileset.png >> $ASSET_FILE
ggbgfx tilemap -n win_tiles win.png tileset.png >> $ASSET_FILE
ggbgfx tilemap -n gameover_tiles gameover.png tileset.png >> $ASSET_FILE
