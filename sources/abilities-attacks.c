//
// Created by Harry on 10/11/2025.
//
#include <stdio.h>
#include "abilities-attacks.h"
#include "raylib.h"
#include "enemies.h"
#include "math.h"
#include "config.h"
#include "game_state.h"
#include "boss.h"

void add_ability(int x, int y) {
    Ability *ability = get_available_ability();

    if (!ability) return;

    ability->texture = LoadTexture(ability->texture_path);
    ability->hitbox = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    ability->is_active = true;
}

void draw_abilities() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    bool is_colliding = false;

    for (int i = 0; i < *abilities_count; i++) {
        if (abilities[i].acquired || !abilities[i].is_active) continue;

        DrawTexturePro(
            abilities[i].texture,
            (Rectangle){0, 0, abilities[i].texture.width, abilities[i].texture.height},
            abilities[i].hitbox,
            (Vector2){0, 0},
            0.0f,
            BLUE
        );

        if (!is_colliding && CheckCollisionRecs(game_state.player.hitbox, abilities[i].hitbox)) {
            DrawText(
                "Inspecionar",
                abilities[i].hitbox.x - 50,
                abilities[i].hitbox.y - 50,
                30,
                WHITE
            );

            is_colliding = true;
        }
    }
}

void unload_abilities() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    for (int i = 0; i < *abilities_count; i++)
        if (abilities[i].is_active) {
            UnloadTexture(abilities[i].texture);
            abilities[i].is_active = false;
        }
}

Ability *get_available_ability() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    for (int i = 0; i < *abilities_count; i++)
        if (!abilities[i].is_active && !abilities[i].acquired)
            return &abilities[i];

    return NULL;
}

void AbilitiesProjectile(float delta) {
    Player *player = &game_state.player;

    if (!player->abilitySoulProjectile->acquired) return;

    // ===========================================
    // HABILIDADE (F) - Projétil de almas
    // ===========================================

    if (!player->abilitySoulProjectile->is_active && IsKeyPressed(KEY_F)) {
        if (player->souls >= player->max_souls / 3.0) {
            player->souls -= player->max_souls / 3.0; // gasta souls
            if (player->souls < 0) player->souls = 0;

            player->abilitySoulProjectile->is_active = true;
            player->abilitySoulProjectile->lifetime = 3.0f;

            int projWidth = TILE_SIZE * 1.25;
            int projHeight = TILE_SIZE;
            float projSpeed = PLAYER_HOR_SPEED * 1.5;

            if (player->facing_right) {
                player->abilitySoulProjectile->hitbox = (Rectangle){
                    player->hitbox.x + player->hitbox.width,
                    player->hitbox.y + player->hitbox.height / 2 - projHeight / 2,
                    projWidth,
                    projHeight
                };
                player->abilitySoulProjectile->speed = (Vector2){projSpeed, 0};
            } else {
                player->abilitySoulProjectile->hitbox = (Rectangle){
                    player->hitbox.x - projWidth,
                    player->hitbox.y + player->hitbox.height / 2 - projHeight / 2,
                    projWidth,
                    projHeight
                };
                player->abilitySoulProjectile->speed = (Vector2){-projSpeed, 0};
            }
        }
    }

    // Atualiza o projétil se estiver ativo
    if (player->abilitySoulProjectile->is_active) {
        // Desativa depois de 3s
        player->abilitySoulProjectile->lifetime -= delta;
        if (player->abilitySoulProjectile->lifetime <= 0) {
            player->abilitySoulProjectile->is_active = false;
            return;
        }

        player->abilitySoulProjectile->hitbox.x += player->abilitySoulProjectile->speed.x * delta;

        bool hitSomething = false;

        // Checa colisão com parede
        for (int i = 0; i < walls_count; i++) {
            if (CheckCollisionRecs(player->abilitySoulProjectile->hitbox, walls[i].hitbox)) {
                hitSomething = true;
                break;
            }
        }

        // Checa colisão com monstros
        for (int j = 0; j < monsters_count; j++) {
            if (CheckCollisionRecs(player->abilitySoulProjectile->hitbox, monsters[j].hitbox)) {
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


        if (boss.active && player->abilitySoulProjectile->is_active &&
            CheckCollisionRecs(player->abilitySoulProjectile->hitbox, boss.hitbox)) {
            if (!boss.invulnerable) {
                boss.life -= 3;

                boss.hurt_timer = 0.12f;
                boss.invulnerable = true;
                boss.invuln_time = 0.25f;

                if (boss.life <= 0) {
                    boss.active = false;
                }
            }

            player->abilitySoulProjectile->is_active = false;
            return;
        }


        if (hitSomething)
            player->abilitySoulProjectile->is_active = false;
    }
}

void update_ability_acquisition() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    for (int i = 0; i < *abilities_count; i++) {
        if (abilities[i].acquired) continue;

        if (CheckCollisionRecs(game_state.player.hitbox, abilities[i].hitbox) && IsKeyPressed(KEY_UP)) {
            abilities[i].acquired = true;
            abilities[i].is_active = false;
        }

    }
}
bool DashAbility(float delta) {
    const float DASH_SPEED = 1000.0f;
    const float DASH_DURATION = 0.15f;
    const float DASH_COOLDOWN = 1.0f;

    Player *player = &game_state.player;

    // Atualiza cooldown
    if (player->dash.cooldown > 0.0f)
        player->dash.cooldown -= delta;

    // Inicia o dash
    if (IsKeyPressed(KEY_C) &&
        player->dash.cooldown <= 0.0f &&
        !player->dash.active)
    {
        player->dash.active = true;
        player->dash.timer = DASH_DURATION;
        player->dash.cooldown = DASH_COOLDOWN;
        player->dash.hit_confirmed = false;
    }

    // Se estiver em dash
    if (player->dash.active)
    {
        player->dash.timer -= delta;

        float dir = player->facing_right ? 1.0f : -1.0f;
        float dash_speed = dir * DASH_SPEED;

        player->speed.y = 0.0f;
        player->speed.x = dash_speed;

        // Movimentação pixel-a-pixel
        float dx = dash_speed * delta;
        int step = (dx > 0) ? 1 : -1;
        float moved = 0;

        while (fabsf(moved) < fabsf(dx))
        {
            player->hitbox.x += step;
            moved += step;

            // Colisão com paredes
            bool collided = false;
            for (int w = 0; w < walls_count; w++) {
                if (CheckCollisionRecs(player->hitbox, walls[w].hitbox)) {
                    collided = true;
                    break;
                }
            }

            if (collided) {
                // Volta 1 pixel
                player->hitbox.x -= step;

                // Cancela dash
                player->dash.active = false;
                player->dash.timer = 0.0f;
                player->speed.x = 0;
                break;
            }
        }

        // Se esgotou o tempo, cancela
        if (player->dash.timer <= 0.0f) {
            player->dash.active = false;
            player->speed.x = 0;
        }
    }

    return player->dash.active;
}



void HealAbility(float delta) {
    Player *player = &game_state.player;

    bool is_player_stopped = (player->speed.x == 0.0f);

    // Atualiza cooldown
    if (player->combat.heal_cooldown > 0.0f) {
        player->combat.heal_cooldown -= delta;
        if (player->combat.heal_cooldown < 0.0f)
            player->combat.heal_cooldown = 0.0f;
    }

    // Só pode curar se estiver PARADO e no CHÃO
    if (player->souls >= 33 &&
        player->combat.heal_cooldown <= 0.0f &&
        is_player_stopped &&
        player->on_ground) {
        if (IsKeyDown(KEY_A)) {
            player->combat.is_healing = true;
            player->combat.heal_hold_time += delta;

            if (player->combat.heal_hold_time >= player->combat.heal_hold_needed) {
                player->combat.life += 1;
                if (player->combat.life > player->combat.max_life)
                    player->combat.life = player->combat.max_life;

                player->souls -= 33;
                player->combat.heal_cooldown = player->combat.heal_cooldown_max;
                player->combat.is_healing = false;
                player->combat.heal_hold_time = 0.0f;
            }
        } else {
            player->combat.is_healing = false;
            player->combat.heal_hold_time = 0.0f;
        }
    } else {
        player->combat.is_healing = false;
        player->combat.heal_hold_time = 0.0f;
    }
}


void draw_healing_effect() {
    Player *player = &game_state.player;

    if (!player->combat.is_healing) return;

    float progress = player->combat.heal_hold_time / player->combat.heal_hold_needed;
    if (progress > 1.0f) progress = 1.0f;

    int squares = 6; // quantidade de quadrados
    float radius = 80.0f * progress; // eles se afastam conforme o tempo
    float cx = player->hitbox.x + player->hitbox.width / 2;
    float cy = player->hitbox.y + player->hitbox.height / 2;

    for (int i = 0; i < squares; i++) {
        float angle = (2 * PI / squares) * i + GetTime() * 2; // gira devagar
        float x = cx + cosf(angle) * radius;
        float y = cy + sinf(angle) * radius;

        int size = 12 + 6 * progress;
        Color c = (Color){255, 255, 255, (int) (150 + 100 * sinf(GetTime() * 5 + i))};

        DrawRectangle(x - size / 2, y - size / 2, size, size, c);
    }
}
