//
// Created by richa on 16/11/2025.
//

#include "bench.h"
#include "config.h"
#include "game_state.h"

Bench benches[MAX_BENCHES];
int benches_count = 0;

void add_bench(int x, int y, Texture2D texture) {
    if (benches_count < MAX_BENCHES) {
        benches[benches_count++] = (Bench){
            texture, {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
        };
    }
}

void draw_benches() {
    bool is_colliding = game_state.player.is_sitting;

    for (int i = 0; i < benches_count; i++) {
        DrawTexturePro(
            benches[i].texture,
            (Rectangle){0, 0, benches[i].texture.width, benches[i].texture.height},
            benches[i].hitbox,
            (Vector2){0, 0},
            0.0f,
            GRAY
        );

        if (!is_colliding && CheckCollisionRecs(game_state.player.hitbox, benches[i].hitbox)) {
            DrawText("Descansar",
                     benches[i].hitbox.x - 50,
                     benches[i].hitbox.y - 50,
                     32, WHITE);

            is_colliding = true;
        }
    }
}

void handle_benches_interaction() {
    Player *player = &game_state.player;

    // Stop sitting
    //----------------------------------------------------------------------------------
    if (player->is_sitting) {
        // Qualquer input de ação cancela o descanso
        if ((IsKeyPressed(KEY_C) || IsKeyPressed(KEY_X) || IsKeyPressed(KEY_LEFT) ||
             IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_F) ||
             IsKeyPressed(KEY_A))) {
            player->is_sitting = false;
        }

        return; // quando sentado, não precisa checar novos bancos
    }
    //----------------------------------------------------------------------------------

    // Sit
    //----------------------------------------------------------------------------------
    for (int i = 0; i < benches_count; i++) {
        if (CheckCollisionRecs(player->hitbox, benches[i].hitbox) && IsKeyPressed(KEY_UP)) {
            player->is_sitting = true;
            player->speed.y = 0;
            player->hitbox.y = benches[i].hitbox.y - benches[i].hitbox.height / 2;
            player->hitbox.x = benches[i].hitbox.x;
            player->combat.life = player->combat.max_life;
            player->souls = player->max_souls;
            player->spawn_pos = (Vector2){benches[i].hitbox.x, benches[i].hitbox.y - benches[i].hitbox.height / 2};
            player->has_spawn = true;

            break; // já achou um banco com o qual colidiu; não precisa checar os outros
        }
    }
    //----------------------------------------------------------------------------------
}
