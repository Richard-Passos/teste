//
// Created by Harry on 10/11/2025.
//
#include "abilities-attacks.h"

#include "raylib.h"
#include "player.h"
#include "enemies.h"
#include "math.h"
#include "config.h"
#include "game_state.h"

Ability abilities[MAX_ABILITIES];
int abilities_count = 0;

void add_ability(int x, int y) {
    if (abilities_count < MAX_ABILITIES) {
        abilities[abilities_count++] = (Ability){{x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}};
    }
}

void draw_abilities() {
    for (int i = 0; i < abilities_count; i++)
        DrawRectangleRec(abilities[i].hitbox, BROWN);
}

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

bool DashAbility(Player *player) {
    const float DASH_SPEED = 600.0f;
    const float DASH_DURATION = 0.15f;
    const float DASH_COOLDOWN = 1.0f;

    float dt = GetFrameTime();
    bool dash_cancelled = false;

    // Atualiza cooldown
    if (player->dash.cooldown > 0.0f)
        player->dash.cooldown -= dt;

    // Inicia o dash
    if (IsKeyPressed(KEY_C) && player->dash.cooldown <= 0.0f && !player->dash.active) {
        player->dash.active = true;
        player->dash.timer = DASH_DURATION;
        player->dash.cooldown = DASH_COOLDOWN;
        player->dash.hit_confirmed = false; // novo campo: evita múltiplos hits
    }

    // Se estiver em dash
    if (player->dash.active) {
        player->dash.timer -= dt;

        float dir = player->facing_right ? 1.0f : -1.0f;
        player->speed.y = 0.0f; // ignora gravidade
        player->speed.x = dir * DASH_SPEED;
        player->hitbox.x += player->speed.x * dt;

        // Verifica colisão apenas se ainda não cancelou
        if (!player->dash.hit_confirmed) {
            for (int i = 0; i < monsters_count; i++) {
                if (monsters[i].hitbox.width == 0 || monsters[i].hitbox.height == 0) continue;

                if (CheckCollisionRecs(player->hitbox, monsters[i].hitbox)) {
                    player->dash.hit_confirmed = true;
                    dash_cancelled = true;
                    player->dash.active = false;
                    player->dash.timer = 0.0f;
                    player->speed.x = 0.0f;

                    if (dir > 0)
                        player->hitbox.x = monsters[i].hitbox.x - player->hitbox.width - 1.0f;
                    else
                        player->hitbox.x = monsters[i].hitbox.x + monsters[i].hitbox.width + 1.0f;

                    break;
                }
            }
        }

        // Encerra dash por tempo
        if (player->dash.timer <= 0.0f) {
            player->dash.active = false;
            player->speed.x = 0.0f;
        }
    }

    return dash_cancelled;
}


void HealAbility(Player *player, float delta) {
    // Atualiza cooldown
    if (player->combat.heal_cooldown > 0.0f) {
        player->combat.heal_cooldown -= delta;
        if (player->combat.heal_cooldown < 0.0f)
            player->combat.heal_cooldown = 0.0f;
    }

    // Pode começar a curar se tiver almas e cooldown zerado
    if (player->souls >= 33 && player->combat.heal_cooldown <= 0.0f) {
        // Segurando o botão A
        if (IsKeyDown(KEY_A)) {
            player->combat.is_healing = true;
            player->combat.heal_hold_time += delta;

            // Se segurou o suficiente
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
            // Soltou antes de completar
            player->combat.is_healing = false;
            player->combat.heal_hold_time = 0.0f;
        }
    } else {
        player->combat.is_healing = false;
        player->combat.heal_hold_time = 0.0f;
    }
}

void DrawHealingEffect() {
    Player *player = &game_state.player;

    if (!player->combat.is_healing) return;

    float progress = player->combat.heal_hold_time / player->combat.heal_hold_needed;
    if (progress > 1.0f) progress = 1.0f;

    int squares = 6; // quantidade de quadrados
    float radius = 40.0f * progress; // eles se afastam conforme o tempo
    float cx = player->hitbox.x + player->hitbox.width / 2;
    float cy = player->hitbox.y + player->hitbox.height / 2;

    for (int i = 0; i < squares; i++) {
        float angle = (2 * PI / squares) * i + GetTime() * 2; // gira devagar
        float x = cx + cosf(angle) * radius;
        float y = cy + sinf(angle) * radius;

        int size = 6 + 3 * progress;
        Color c = (Color){255, 255, 255, (int) (150 + 100 * sinf(GetTime() * 5 + i))};

        DrawRectangle(x - size / 2, y - size / 2, size, size, c);
    }
}
