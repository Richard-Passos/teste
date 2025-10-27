#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "raylib.h"

#define MAX_MAP_ROWS 512
#define MAX_MAP_COLS 512

#define MAX_MONSTERS 10
#define MAX_WALLS 1024
#define MAX_ITEMS 128
#define TILE_SIZE 32

typedef struct {
    Texture2D Wall;
} Textures;

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
} Wall;

typedef struct {
    Rectangle hitbox;
} Monster;

typedef struct {
    Rectangle hitbox;
} Item;

extern Rectangle player_start;
extern Rectangle boss_start;

extern Wall walls[MAX_WALLS];
extern int walls_count;

extern Monster monsters[MAX_MONSTERS];
extern int monsters_count;

extern Item items[MAX_ITEMS];
extern int items_count;

int load_map(char path[], Textures textures);

void draw_map();

#endif
