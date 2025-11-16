//
// Created by richa on 16/11/2025.
//

#include "wall.h"
#include "config.h"

Wall walls[MAX_WALLS];
int walls_count = 0;

void add_wall(int x, int y, Texture2D texture) {
    if (walls_count < MAX_WALLS) {
        walls[walls_count++] = (Wall){
            texture, {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
        };
    }
}

void draw_walls() {
    for (int i = 0; i < walls_count; i++)
        DrawTexture(walls[i].texture, walls[i].hitbox.x, walls[i].hitbox.y, WHITE);
}
