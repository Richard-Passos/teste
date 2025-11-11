#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "raylib.h"
#include "enemies.h"

#define MAX_MAP_ROWS 512
#define MAX_MAP_COLS 512

#define MAX_WALLS 1024
#define MAX_ITEMS 128
#define TILE_SIZE 32

typedef struct {
    Texture2D Wall;
} Textures;



typedef struct {
    Rectangle hitbox;
} Item;

typedef struct {
    Rectangle hitbox;
    bool acquired;
} Ability;

extern Rectangle player_start;
extern Rectangle boss_start;

extern Wall walls[MAX_WALLS];

/* Usar o tipo definido em enemies.h */
extern Monster monsters[MAX_MONSTERS];
extern int monsters_count;

extern Item items[MAX_ITEMS];
extern int items_count;

extern Ability abilities[MAX_ITEMS];
extern int abilities_count;

int load_map(char path[], Textures textures);

void draw_map();

#endif
