//
// Created by Harry on 10/11/2025.
//
#include <stdio.h>
#include "abilities.h"
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
        if (abilities[i].is_acquired || !abilities[i].is_active) continue;

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

            if (!abilities[i].is_acquired)
                abilities[i].is_active = false;
        }
}

Ability *get_available_ability() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    for (int i = 0; i < *abilities_count; i++)
        if (!abilities[i].is_active && !abilities[i].is_acquired)
            return &abilities[i];

    return NULL;
}

void update_ability_acquisition() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    for (int i = 0; i < *abilities_count; i++) {
        if (abilities[i].is_acquired) continue;

        if (CheckCollisionRecs(game_state.player.hitbox, abilities[i].hitbox) && IsKeyPressed(KEY_UP)) {
            abilities[i].is_acquired = true;
            abilities[i].is_active = false;
        }
    }
}

void soul_projectile_ability() {
    Player *player = &game_state.player;
    Ability *soul_projectile = player->abilities.soul_projectile;
    float delta_time = GetFrameTime();

    if (soul_projectile == NULL || !soul_projectile->is_acquired) return;

    // ===========================================
    // HABILIDADE (F) - Projétil de almas
    // ===========================================

    if (!soul_projectile->is_active && IsKeyPressed(KEY_F)) {
        if (player->souls >= player->max_souls / 3.0) {
            player->souls -= player->max_souls / 3.0; // gasta souls
            if (player->souls < 0) player->souls = 0;

            soul_projectile->is_active = true;
            soul_projectile->lifetime = 3.0f;

            int projWidth = TILE_SIZE * 1.25;
            int projHeight = TILE_SIZE;
            float projSpeed = PLAYER_HOR_SPEED * 1.5;

            if (player->facing_right) {
                soul_projectile->hitbox = (Rectangle){
                    player->hitbox.x + player->hitbox.width,
                    player->hitbox.y + player->hitbox.height / 2 - projHeight / 2,
                    projWidth,
                    projHeight
                };
                soul_projectile->speed = (Vector2){projSpeed, 0};
            } else {
                soul_projectile->hitbox = (Rectangle){
                    player->hitbox.x - projWidth,
                    player->hitbox.y + player->hitbox.height / 2 - projHeight / 2,
                    projWidth,
                    projHeight
                };
                soul_projectile->speed = (Vector2){-projSpeed, 0};
            }
        }
    }

    // Atualiza o projétil se estiver ativo
    if (soul_projectile->is_active) {
        // Desativa depois de 3s
        soul_projectile->lifetime -= delta_time;
        if (soul_projectile->lifetime <= 0) {
            soul_projectile->is_active = false;
            return;
        }

        soul_projectile->hitbox.x += soul_projectile->speed.x * delta_time;

        bool hitSomething = false;

        // Checa colisão com parede
        for (int i = 0; i < walls_count; i++) {
            if (CheckCollisionRecs(soul_projectile->hitbox, walls[i].hitbox)) {
                hitSomething = true;
                break;
            }
        }

        // Checa colisão com monstros
        for (int j = 0; j < monsters_count; j++) {
            if (CheckCollisionRecs(soul_projectile->hitbox, monsters[j].hitbox)) {
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


        if (boss.active && soul_projectile->is_active &&
            CheckCollisionRecs(soul_projectile->hitbox, boss.hitbox)) {
            if (!boss.invulnerable) {
                boss.life -= 3;

                boss.hurt_timer = 0.12f;
                boss.invulnerable = true;
                boss.invuln_time = 0.25f;

                if (boss.life <= 0) {
                    boss.active = false;
                }
            }

            soul_projectile->is_active = false;
            return;
        }


        if (hitSomething)
            soul_projectile->is_active = false;
    }
}

void dash_ability() {
    const float DASH_SPEED = 1000.0f;
    const float DASH_DURATION = 0.15f;
    const float DASH_COOLDOWN = 1.0f;

    Player *player = &game_state.player;
    Ability *dash = player->abilities.dash;
    float delta_time = GetFrameTime();

    if (dash == NULL || !dash->is_acquired) return;

    // Atualiza cooldown
    if (dash->cooldown > 0.0f)
        dash->cooldown -= delta_time;

    // Inicia o dash
    if (IsKeyPressed(KEY_C) &&
        dash->cooldown <= 0.0f &&
        !dash->is_active) {
        dash->is_active = true;
        dash->timer = DASH_DURATION;
        dash->cooldown = DASH_COOLDOWN;
        dash->hit_confirmed = false;
    }

    // Se estiver em dash
    if (dash->is_active) {
        dash->timer -= delta_time;

        float dir = player->facing_right ? 1.0f : -1.0f;
        float dash_speed = dir * DASH_SPEED;

        player->speed.y = 0.0f;
        player->speed.x = dash_speed;

        // Movimentação pixel-a-pixel
        float dx = dash_speed * delta_time;
        int step = (dx > 0) ? 1 : -1;
        float moved = 0;

        while (fabsf(moved) < fabsf(dx)) {
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
                dash->is_active = false;
                dash->timer = 0.0f;
                player->speed.x = 0;
                break;
            }
        }

        // Se esgotou o tempo, cancela
        if (dash->timer <= 0.0f) {
            dash->is_active = false;
            player->speed.x = 0;
        }
    }
}

void heal_ability() {
    const float HEAL_COOLDOWN = 1.0f;
    const float HEAL_TIMER = 1.0f;

    Player *player = &game_state.player;
    Ability *heal = player->abilities.heal;
    float delta_time = GetFrameTime();

    if (heal == NULL || !heal->is_acquired) return;

    bool is_player_stopped = (player->speed.x == 0.0f);

    // Atualiza cooldown
    if (heal->cooldown > 0.0f) {
        heal->cooldown -= delta_time;

        if (heal->cooldown < 0.0f) {
            heal->cooldown = 0.0f;
        }
    }

    // Só pode curar se estiver PARADO e no CHÃO
    if (player->souls >= 33 &&
        heal->cooldown == 0.0f &&
        is_player_stopped &&
        player->on_ground &&
        IsKeyDown(KEY_A)) {
        heal->is_active = true;

        if (heal->timer > 0.0f) {
            heal->timer -= delta_time;

            if (heal->timer < 0.0f)
                heal->timer = 0.0f;
        }

        if (heal->timer == 0.0f) {
            player->combat.life += player->combat.life + 1 > player->combat.max_life
                                       ? 0
                                       : 1;

            player->souls -= 33;
            heal->cooldown = HEAL_COOLDOWN;
            heal->timer = HEAL_TIMER;
            heal->is_active = false;
        }
    } else {
        heal->is_active = false;
        heal->timer = HEAL_TIMER;
    }
}

void draw_healing_effect() {
    const float HEAL_TIMER = 1.0f;

    Player *player = &game_state.player;
    Ability *heal = player->abilities.heal;

    if (heal == NULL || heal->is_acquired && !heal->is_active) return;

    float progress = 1.0f - heal->timer / HEAL_TIMER;

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

void double_jump_ability() {
    Player *player = &game_state.player;
    Ability *double_jump = player->abilities.double_jump;

    if (double_jump == NULL || !double_jump->is_acquired) return;

    bool is_falling = (player->speed.y > 0 && !player->on_ground);

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) && player->combat.push_timer <= 0.0f) {
        if (!player->on_ground && player->jump_count == 1 || (player->jump_count == 0 && is_falling)) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 2;
        }
    }
}
