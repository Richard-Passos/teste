//
// Created by Harry on 10/11/2025.
//
#include "raylib.h"
#include "player.h"
#include "map.h"




void AbilitiesProjectile(Player *player, float delta) {

    // ===========================================
    // HABILIDADE (F) - Projétil de almas
    // ===========================================

    if (player->abilitySoulProjectile.acquired && IsKeyPressed(KEY_F) && !player->abilitySoulProjectile.active) {
        if (player->souls >= 33) {
            player->souls -= 33; // gasta souls
            if (player->souls < 0) player->souls = 0;

            player->abilitySoulProjectile.active = true;

            int projWidth = 50;
            int projHeight = 32;
            float projSpeed = 400;

            if (player->facing_right) {
                player->abilitySoulProjectile.hitbox = (Rectangle){
                    player->hitbox.x + player->hitbox.width,
                    player->hitbox.y + player->hitbox.height / 2 - projHeight / 2,
                    projWidth,
                    projHeight
                };
                player->abilitySoulProjectile.speed = (Vector2){projSpeed, 0};
            } else {
                player->abilitySoulProjectile.hitbox = (Rectangle){
                    player->hitbox.x - projWidth,
                    player->hitbox.y + player->hitbox.height / 2 - projHeight / 2,
                    projWidth,
                    projHeight
                };
                player->abilitySoulProjectile.speed = (Vector2){-projSpeed, 0};
            }
        } else {
            // Opcional: feedback se não tiver souls
            // printf("Sem souls suficientes!\n");
        }
    }

    // Atualiza o projétil se estiver ativo
    if (player->abilitySoulProjectile.active) {
        player->abilitySoulProjectile.hitbox.x += player->abilitySoulProjectile.speed.x * delta;

        bool hitSomething = false;

        // Checa colisão com parede
        for (int i = 0; i < walls_count; i++) {
            if (CheckCollisionRecs(player->abilitySoulProjectile.hitbox, walls[i].hitbox)) {
                hitSomething = true;
                break;
            }
        }

        // Checa colisão com monstros
        for (int j = 0; j < monsters_count; j++) {
            if (CheckCollisionRecs(player->abilitySoulProjectile.hitbox, monsters[j].hitbox)) {
                monsters[j].life -= 3; // 3 de dano fixo
                monsters[j].hurt_timer = 0.1f;
                monsters[j].invulnerable = true;
                monsters[j].invuln_time = 0.3f;

                if (monsters[j].life <= 0) {
                    monsters[j].hitbox.width = 0;
                    monsters[j].hitbox.height = 0;

                    int gained = GetRandomValue(35, 45);
                    player->money += gained;

                    player->last_money_gain = gained;
                    player->money_gain_timer = 1.5f;
                }

                hitSomething = true;
                break; // projétil some após acertar
            }
        }

        if (hitSomething)
            player->abilitySoulProjectile.active = false;
    }
}


void UpdateAbilityAcquisition(Player *player) {
    if (!player->abilitySoulProjectile.acquired &&
        CheckCollisionRecs(player->hitbox, player->abilitySoulProjectile.hitbox)) {

        // Mostra um prompt para o jogador
        DrawText("Inspecionar",
                 player->abilitySoulProjectile.hitbox.x - 30,
                 player->abilitySoulProjectile.hitbox.y - 40, 20, WHITE);

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            player->abilitySoulProjectile.acquired = true;
            player->abilitySoulProjectile.hitbox = (Rectangle){0, 0, 0, 0};
            abilities[0].hitbox = (Rectangle){0, 0, 0, 0};

        }
    }
}

void DrawAbilities() {
    for (int i = 0; i < abilities_count; i++)
        DrawRectangleRec(abilities[i].hitbox, BROWN);
}
