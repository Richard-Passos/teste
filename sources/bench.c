//
// Created by richa on 16/11/2025.
//

#include "bench.h"
#include "config.h"

Bench benchs[MAX_BENCHS];
int benchs_count = 0;

void add_bench(int x, int y, Texture2D texture) {
    if (benchs_count < MAX_BENCHS) {
        benchs[benchs_count++] = (Bench){
            texture, {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
        };
    }
}

void draw_benchs() {
    for (int i = 0; i < benchs_count; i++)
        DrawTexture(benchs[i].texture, benchs[i].hitbox.x, benchs[i].hitbox.y, WHITE);
}
