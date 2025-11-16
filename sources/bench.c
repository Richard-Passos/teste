//
// Created by richa on 16/11/2025.
//

#include "bench.h"
#include "config.h"
#include "game_state.h"

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


void handle_benchs_interaction() {
    Player *player = &game_state.player;


    for (int i = 0; i < benchs_count; i++)
        if (CheckCollisionRecs(player->hitbox, benchs[i].hitbox) && !player->is_sitting) {
            DrawText("Descansar", benchs[i].hitbox.x + 220, benchs[i].hitbox.y - 60, 32, WHITE);

            if (IsKeyPressed(KEY_UP)) {
                player->is_sitting = true;
                player->hitbox.y -= 32;
                player->combat.life = player->combat.max_life;
                player->souls = player->max_souls;
            }
        }

    if (player->is_sitting) {
        player->speed.y -= GRAVITY * GetFrameTime();

        if ((IsKeyPressed(KEY_C) || IsKeyPressed(KEY_X) || IsKeyPressed(KEY_LEFT) ||
             IsKeyPressed(KEY_RIGHT)
             || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_F) || IsKeyPressed(KEY_A))) {
            player->is_sitting = false;
            player->hitbox.y += 32;
        }
    }
}
