//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_WALL_H
#define HOLLOW_WALL_H
#define MAX_WALLS 1024
#include "raylib.h"

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
} Wall;

void add_wall(int, int, Texture2D);

void draw_walls();

extern Wall walls[MAX_WALLS];
extern int walls_count;

#endif
