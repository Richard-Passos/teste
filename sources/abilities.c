//
// Created by Harry on 10/11/2025.
//
#include <stdio.h>
#include "abilities.h"
#include "raylib.h"
#include "enemies.h"
#include "math.h"
#include "main.h"
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


    for (int i = 0; i < *abilities_count; i++) {
        Ability ability = abilities[i];

        if (!ability.is_acquired) {
            DrawTexturePro
            (
                ability.texture,
                (Rectangle){0, 0, ability.texture.width, ability.texture.height},
                ability.hitbox,
                (Vector2){0, 0},
                0.0f,
                BLUE
            );

            if (CheckCollisionRecs(game_state.player.hitbox, ability.hitbox)) {
                DrawText(
                    "Inspecionar",
                    ability.hitbox.x - 50,
                    ability.hitbox.y - 50,
                    30,
                    WHITE
                );
            }
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

bool is_ability_active(Ability *ability) {
    return ability != NULL && ability->is_acquired && ability->is_active;
}

void update_ability_acquisition() {
    Ability *abilities = game_state.abilities;
    int *abilities_count = &game_state.abilities_count;

    for (int i = 0; i < *abilities_count; i++) {
        if (!abilities[i].is_acquired) {
            if (CheckCollisionRecs(game_state.player.hitbox, abilities[i].hitbox) && IsKeyPressed(KEY_UP)) {
                abilities[i].is_acquired = true;
                abilities[i].is_active = false;

                snprintf(
                    game_state.recent_ability_text,
                    sizeof(game_state.recent_ability_text),
                    "%s: %s",
                    abilities[i].label,
                    abilities[i].description
                );

                game_state.recent_ability_timer = RECENT_ABILITY_TIMER;
            }
        }
    }
}

void draw_ability_popup() {
    if (game_state.recent_ability_timer <= 0.0f) return;

    game_state.recent_ability_timer -= DELTA_TIME;

    // Fundo
    int w = MeasureText(game_state.recent_ability_text, 22) + 40;
    int h = 40;
    int x = SCREEN_WIDTH / 2 - w / 2;
    int y = 100;

    DrawRectangle(x, y, w, h, (Color){0, 0, 0, 180});
    DrawRectangleLines(x, y, w, h, WHITE);
    DrawText(game_state.recent_ability_text, x + 20, y + 10, 22, YELLOW);
}


void soul_projectile_ability() {
    Player *player = &game_state.player;
    Ability *soul_projectile = player->abilities.soul_projectile;

    if (soul_projectile == NULL || !soul_projectile->is_acquired) return;

    // Init soul projectile
    if (IsKeyPressed(KEY_F) && !soul_projectile->is_active) {
        int souls_cost = player->combat.max_souls / 3;

        if (player->combat.souls >= souls_cost) {
            player->combat.souls -= souls_cost;
            if (player->combat.souls < 0) player->combat.souls = 0;

            int proj_width = TILE_SIZE * 1.25;
            int proj_height = TILE_SIZE;
            float proj_speed = (player->direction == DIR_LEFT ? -PLAYER_SPEED : PLAYER_SPEED) * 1.5;

            soul_projectile->is_active = true;
            soul_projectile->cooldown = SOUL_PROJECTILE_COOLDOWN;
            soul_projectile->hitbox = (Rectangle){
                player->hitbox.x + (player->direction == DIR_RIGHT ? player->hitbox.width : proj_width * -1),
                player->hitbox.y + player->hitbox.height / 2.0 - proj_height / 2.0,
                proj_width,
                proj_height
            };
            soul_projectile->speed = (Vector2){proj_speed, 0};
        }
    }

    // Update soul projectile
    if (soul_projectile->is_active) {
        soul_projectile->hitbox.x += soul_projectile->speed.x * DELTA_TIME;

        // Pass through
        handle_collision_with_monster(soul_projectile->hitbox, SOUL_PROJECTILE_DAMAGE);
        handle_collision_with_boss(soul_projectile->hitbox, SOUL_PROJECTILE_DAMAGE);

        if (handle_collision_with_walls(soul_projectile->hitbox))
            soul_projectile->is_active = false;

        if (soul_projectile->cooldown > 0)
            soul_projectile->cooldown -= DELTA_TIME;
        else
            soul_projectile->is_active = false;
    }
}

void dash_ability() {
    Player *player = &game_state.player;
    Ability *dash = player->abilities.dash;

    if (dash == NULL || !dash->is_acquired) return;

    if (dash->cooldown > 0.0f)
        dash->cooldown -= DELTA_TIME;
    else if (IsKeyPressed(KEY_C) && !dash->is_active) {
        dash->is_active = true;
        dash->timer = DASH_DURATION;
        dash->cooldown = DASH_COOLDOWN;
        dash->hit_confirmed = false;
    }

    if (!dash->is_active) return;

    float dash_speed = DASH_SPEED * (player->direction == DIR_RIGHT ? 1.0f : -1.0f);

    player->speed.y = 0.0f;

    // Pixel Movement
    float dx = dash_speed * DELTA_TIME;
    int step = (dx > 0) ? 1 : -1;
    float moved = 0;

    while (fabsf(moved) < fabsf(dx)) {
        player->hitbox.x += step;
        moved += step;

        if (handle_collision_with_walls(player->hitbox) || handle_collision_with_monster(player->hitbox, 0)) {
            player->hitbox.x -= step;
            dash->timer = 0.0f;
            // Knockback
            break;
        }
    }

    if (dash->timer > 0)
        dash->timer -= DELTA_TIME;
    else
        dash->is_active = false;
}

void heal_ability() {
    Player *player = &game_state.player;
    Ability *heal = player->abilities.heal;

    if (heal == NULL || !heal->is_acquired) return;

    bool is_player_stopped = (player->speed.x == 0.0f);

    // Atualiza cooldown
    if (heal->cooldown > 0.0f) {
        heal->cooldown -= DELTA_TIME;

        if (heal->cooldown < 0.0f) {
            heal->cooldown = 0.0f;
        }
    }

    // Só pode curar se estiver PARADO e no CHÃO
    if (player->combat.souls >= 33 &&
        heal->cooldown == 0.0f &&
        is_player_stopped &&
        player->on_ground &&
        IsKeyDown(KEY_A)) {
        heal->is_active = true;

        if (heal->timer > 0.0f) {
            heal->timer -= DELTA_TIME;

            if (heal->timer < 0.0f)
                heal->timer = 0.0f;
        }

        if (heal->timer == 0.0f) {
            player->combat.life += player->combat.life + 1 > player->combat.max_life
                                       ? 0
                                       : 1;

            player->combat.souls -= 33;
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

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z))) {
        if (!player->on_ground
            && (player->jump_count == 1 || (player->jump_count == 0 && player->speed.y > 0))) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 2;
        }
    }
}
