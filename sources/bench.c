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
    for (int i = 0; i < benches_count; i++)
        DrawTexture(benches[i].texture, benches[i].hitbox.x, benches[i].hitbox.y, WHITE);
}


void handle_benches_interaction() {
    Player *player = &game_state.player;

    // Se já está sentado, processa inputs para levantar
    if (player->is_sitting) {
        // aplica um pouco de física vertical enquanto sentado (igual ao que tinha)
        player->speed.y -= GRAVITY * GetFrameTime();
        player->hitbox.y -= TILE_SIZE;

        // Qualquer input de ação cancela o descanso
        if ((IsKeyPressed(KEY_C) || IsKeyPressed(KEY_X) || IsKeyPressed(KEY_LEFT) ||
             IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_F) ||
             IsKeyPressed(KEY_A))) {
            player->is_sitting = false;
        }

        return; // quando sentado, não precisa checar novos bancos
    }

    // Se não está sentado, verifica se o player aperta UP enquanto está em cima de um banco
    for (int i = 0; i < benches_count; i++) {
        if (CheckCollisionRecs(player->hitbox, benches[i].hitbox)) {
            if (IsKeyPressed(KEY_UP)) {
                player->is_sitting = true;
                player->combat.life = player->combat.max_life;
                player->souls = player->max_souls;
            }
            break; // já achou um banco com o qual colidiu; não precisa checar os outros
        }
    }
}
