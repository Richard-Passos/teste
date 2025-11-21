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
        DrawTexturePro(
            walls[i].texture,
            (Rectangle){0, 0, walls[i].texture.width, walls[i].texture.height},
            walls[i].hitbox,
            (Vector2){0, 0},
            0.0f,
            BROWN
        );
}
