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

    // ------------------------------------------
    // SE ESTÁ SENTADO
    // ------------------------------------------
    if (player->is_sitting) {

        if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_X) ||
            IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) ||
            IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_F) ||
            IsKeyPressed(KEY_A))
        {
            player->is_sitting = false;

            // Mantém posição X salva
            player->hitbox.x = player->sit_x;

            // --- DESCE PARA O CHÃO MAIS PRÓXIMO ---
            // Simula o player caindo até encontrar um wall abaixo
            for (int i = 0; i < walls_count; i++) {
                Wall *w = &walls[i];

                // O chão é um wall com Y maior que o jogador
                if (w->hitbox.y >= player->hitbox.y + player->hitbox.height) {

                    // Checa sobreposição horizontal
                    bool overlap =
                        player->hitbox.x + player->hitbox.width > w->hitbox.x &&
                        player->hitbox.x < w->hitbox.x + w->hitbox.width;

                    if (overlap) {
                        // Coloca o player exatamente no topo do chão
                        player->hitbox.y = w->hitbox.y - player->hitbox.height;
                        player->speed.y = 0;
                        player->on_ground = true;
                        break;
                    }
                }
            }
        }

        return;
    }


    // ------------------------------------------
    // NÃO ESTÁ SENTADO → DETECTA COLISÃO
    // ------------------------------------------
    for (int i = 0; i < benchs_count; i++) {

        if (CheckCollisionRecs(player->hitbox, benchs[i].hitbox)) {

            if (IsKeyPressed(KEY_UP)) {

                player->is_sitting = true;

                // Cura e almas
                player->combat.life = player->combat.max_life;
                player->souls = player->max_souls;

                // CENTRO DO BANCO
                float center_x = benchs[i].hitbox.x +
                    (benchs[i].hitbox.width - player->hitbox.width) / 2;

                // CORREÇÃO DO Y (usa TILE_SIZE!)
                float sit_y = benchs[i].hitbox.y - player->hitbox.height + TILE_SIZE/2;

                // Aplica
                player->hitbox.x = center_x;
                player->hitbox.y = sit_y;

                // Salva
                player->sit_x = center_x;
                player->sit_y = sit_y;
            }

            return;
        }
    }
}