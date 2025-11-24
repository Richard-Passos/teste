#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "raylib.h"
#include "player.h"

#define MAX_MAP_ROWS 512
#define MAX_MAP_COLS 512

void add_texture(char texture_path[]);

bool load_map(char path[]);

void unload_map();

void handle_map();

void draw_map();

void update_map();

void set_map_path(int);

extern char map_path[100];
#endif
