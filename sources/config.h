//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_CONFIG_H
#define MY_RAYLIB_GAME_CONFIG_H

#define NAME "Hollow"

#define GRAVITY 400
#define PLAYER_JUMP_SPEED 350.0f
#define PLAYER_HOR_SPEED 300.0f

#define LAND_MONSTER_HOR_SPEED 150.0f

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define TILE_SIZE 64

#define MAP_FILE_PATH "../map.txt"
#define VILLAGE_FILE_PATH "../village.txt"
#define SHOP_FILE_PATH "../shop.txt"
#define GAME_STATE_FILE_PATH "../game_state.bin"

#define BOSS_JUMP_PEAK_Y 50    // altura mínima (teto do mapa)
#define BOSS_JUMP_HOLD_TIME 2.0f   // tempo parado no ar
#define BOSS_JUMP_SPEED 900
#define BOSS_JUMP_FALL_SPEED 1400

#endif //MY_RAYLIB_GAME_CONFIG_H
