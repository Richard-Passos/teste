//
// Created by richa on 30/11/2025.
//

#ifndef HOLLOW_MAIN_H
#define HOLLOW_MAIN_H

typedef enum {
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
} Direction;

#define NAME "Hollow"

#define GRAVITY 800

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 700
#define TILE_SIZE 96

#define MAP_DEFAULT_PATH "../tunel"
#define VILLAGE_FILE_PATH "../village.txt"
#define SHOP_FILE_PATH "../shop.txt"
#define GAME_STATE_FILE_PATH "../game_state.bin"

extern float DELTA_TIME;
#endif //HOLLOW_MAIN_H
