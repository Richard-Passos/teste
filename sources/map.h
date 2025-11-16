#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "raylib.h"
#include "player.h"

#define MAX_MAP_ROWS 512
#define MAX_MAP_COLS 512

void add_texture(char texture_path[]);

/* Returns 0 when not loaded, 1 when loaded for the first time, 2 when already loaded */
int load_map(char path[]);

void unload_map();

void draw_map();

extern Rectangle boss_start;
extern Rectangle shop_hitbox;
extern Rectangle level_hitbox;

#endif
