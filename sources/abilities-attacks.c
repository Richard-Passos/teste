//
// Created by Harry on 10/11/2025.
//
#include <stdio.h>
#include "abilities-attacks.h"
#include "raylib.h"
#include "player.h"
#include "enemies.h"
#include "math.h"
#include "config.h"
#include "game_state.h"
#include "boss.h"

Ability abilities[MAX_ABILITIES];
int abilities_count = 0;

void add_ability(int x, int y) {
    Player *player = &game_state.player;

    if (abilities_count < MAX_ABILITIES)
        abilities[abilities_count++] = (Ability){{x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}};

    if (abilities_count > 0)
        player->abilitySoulProjectile.hitbox = abilities[0].hitbox;
}

void draw_abilities() {
    bool is_colliding = false;

    for (int i = 0; i < abilities_count; i++) {
        DrawRectangleRec(abilities[i].hitbox, BROWN);

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

void AbilitiesProjectile(Player *player, float delta) {
    // ===========================================
    // HABILIDADE (F) - Projétil de almas
    // ===========================================

    if (player->abilitySoulProjectile.acquired && IsKeyPressed(KEY_F) && !player->abilitySoulProjectile.active) {
        if (player->souls >= player->max_souls / 3.0) {
            player->souls -= player->max_souls / 3.0; // gasta souls
            if (player->souls < 0) player->souls = 0;

            player->abilitySoulProjectile.active = true;
            player->abilitySoulProjectile.lifetime = 3.0f;

            int projWidth = TILE_SIZE * 1.25;
            int projHeight = TILE_SIZE;
            float projSpeed = PLAYER_HOR_SPEED * 1.5;

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
        }
    }

    // Atualiza o projétil se estiver ativo
    if (player->abilitySoulProjectile.active) {

        // Desativa depois de 3s
        player->abilitySoulProjectile.lifetime -= delta;
        if (player->abilitySoulProjectile.lifetime <= 0) {
            player->abilitySoulProjectile.active = false;
            return;
        }

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


        if (boss.active && player->abilitySoulProjectile.active &&
            CheckCollisionRecs(player->abilitySoulProjectile.hitbox, boss.hitbox)) {

            if (!boss.invulnerable) {
                boss.life -= 3;

                boss.hurt_timer = 0.12f;
                boss.invulnerable = true;
                boss.invuln_time = 0.25f;

                if (boss.life <= 0) {
                    boss.active = false;
                }
            }

            player->abilitySoulProjectile.active = false;
            return;
            }


        if (hitSomething)
            player->abilitySoulProjectile.active = false;
    }
}


void update_ability_acquisition() {
    Player *player = &game_state.player;

    // Já pegou a habilidade? então nem verifica mais.
    if (player->abilitySoulProjectile.acquired) return;

    // Colidiu com a habilidade no mapa?
    if (CheckCollisionRecs(player->hitbox, player->abilitySoulProjectile.hitbox)) {
        // Só pega se apertar para cima
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            player->abilitySoulProjectile.acquired = true;

            // Remove o objeto do mapa
            player->abilitySoulProjectile.hitbox = (Rectangle){0, 0, 0, 0};
            abilities[0].hitbox = (Rectangle){0, 0, 0, 0};
        }
    }
}


bool DashAbility(Player *player, float delta) {
    const float DASH_SPEED = 600.0f;
    const float DASH_DURATION = 0.15f;
    const float DASH_COOLDOWN = 1.0f;

    bool dash_cancelled = false;

    // Atualiza cooldown
    if (player->dash.cooldown > 0.0f)
        player->dash.cooldown -= delta;

    // Inicia o dash
    if (IsKeyPressed(KEY_C) && player->dash.cooldown <= 0.0f && !player->dash.active) {
        player->dash.active = true;
        player->dash.timer = DASH_DURATION;
        player->dash.cooldown = DASH_COOLDOWN;
        player->dash.hit_confirmed = false; // novo campo: evita múltiplos hits
    }

    // Se estiver em dash
    if (player->dash.active) {

        player->dash.timer -= delta;

        float dir = player->facing_right ? 1.0f : -1.0f;
        player->speed.y = 0.0f;
        player->speed.x = dir * DASH_SPEED;

        float dx = player->speed.x * delta;
        float step = (dx > 0) ? 1 : -1;

        for (float x = 0; x < fabsf(dx); x += 1.0f) {
            player->hitbox.x += step;

            // COLISÃO COM PAREDE
            for (int w = 0; w < walls_count; w++) {
                if (CheckCollisionRecs(player->hitbox, walls[w].hitbox)) {
                    // Volta 1 pixel e cancela o dash
                    player->hitbox.x -= step;
                    player->dash.active = false;
                    player->dash.timer = 0.0f;
                    player->speed.x = 0;
                    goto dash_after_movement;
                }
            }
        }

        dash_after_movement:

            // Colisão com monstros
            if (!player->dash.hit_confirmed) {
                for (int i = 0; i < monsters_count; i++) {
                    if (monsters[i].hitbox.width == 0) continue;

                    if (CheckCollisionRecs(player->hitbox, monsters[i].hitbox)) {
                        player->dash.hit_confirmed = true;
                        player->dash.active = false;
                        player->dash.timer = 0.0f;
                        player->speed.x = 0;

                        // PREVINE KNOCKBACK IMEDIATO
                        player->ignore_next_monster_hit = true;

                        if (dir > 0)
                            player->hitbox.x = monsters[i].hitbox.x - player->hitbox.width - 1.0f;
                        else
                            player->hitbox.x = monsters[i].hitbox.x + monsters[i].hitbox.width + 1.0f;

                        break;
                    }
                }
            }

        // Acabou o tempo do dash
        if (player->dash.timer <= 0.0f) {
            player->dash.active = false;
            player->speed.x = 0.0f;
        }
    }


    return dash_cancelled;
}


void HealAbility(Player *player, float delta) {
    bool is_player_stopped = (player->speed.x == 0.0f);

    // Atualiza cooldown
    if (player->combat.heal_cooldown > 0.0f) {
        player->combat.heal_cooldown -= delta;
        if (player->combat.heal_cooldown < 0.0f)
            player->combat.heal_cooldown = 0.0f;
    }

    // Só pode curar se estiver PARADO
    if (player->souls >= 33 &&
        player->combat.heal_cooldown <= 0.0f &&
        is_player_stopped) {
        if (IsKeyDown(KEY_A)) {
            player->combat.is_healing = true;
            player->combat.heal_hold_time += delta;

            // Cura completa
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
