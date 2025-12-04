//
// Created by Harry on 16/11/2025.
//

#include "boss.h"
#include "raylib.h"
#include <math.h>
#include <minwindef.h>
#include <stdio.h>

#include "game_state.h"
#include "screen.h"

Boss boss = {0};

void add_boss(int x, int y) {
    boss.hitbox = (Rectangle){
        x * TILE_SIZE, y * TILE_SIZE - 2 * TILE_SIZE, TILE_SIZE * 3, TILE_SIZE * 3
    };
    boss.speed = (Vector2){0, 0};
    boss.max_life = BOSS_LIFE;
    boss.life = boss.max_life;
    boss.damage = BOSS_DAMAGE;
    boss.is_engaged = false;
    boss.state = BOSS_IDLE;
    boss.state_timer = 0;
    boss.speed.y = 0;
    boss.speed.x = 0;

    boss.is_active = true;
}

void draw_boss() {
    if (!boss.is_active) return;

    Color color = PURPLE;
    if (boss.hurt_timer > 0) color = WHITE;

    DrawRectangleRec(boss.hitbox, color);

    // Life bar
    DrawRectangle(boss.hitbox.x,
                  boss.hitbox.y - 20,
                  (boss.life * boss.hitbox.width) / boss.max_life,
                  10,
                  BLACK);
}

void start_chase_attack() {
    boss.state = BOSS_CHASE_ATTACK;
    boss.state_timer = 2.0f;
}

void boss_chase() {
    Player *player = &game_state.player;;

    float px = player->hitbox.x + player->hitbox.width / 2;
    float bx = boss.hitbox.x + boss.hitbox.width / 2;

    float diff = px - bx;

    if (fabsf(diff) < 15)
        boss.speed.x = 0;
    else if (diff < 0)
        boss.speed.x = -BOSS_SPEED;
    else
        boss.speed.x = BOSS_SPEED;

    if (boss.state_timer > 0)
        boss.state_timer -= DELTA_TIME;
    else
        boss.state = BOSS_IDLE;
}

void start_jump_attack() {
    Player *player = &game_state.player;

    float dx = player->hitbox.x + player->hitbox.width / 2 - boss.hitbox.x + boss.hitbox.width / 2;
    float dy = player->hitbox.y - boss.hitbox.y;

    boss.speed.x = dx / BOSS_JUMP_TIME;
    boss.speed.y = dy - 0.5f * GRAVITY * BOSS_JUMP_TIME * 1.5;

    boss.state = BOSS_JUMP_ATTACK;
    boss.state_timer = BOSS_JUMP_TIME;
}

void boss_jump() {
    // Vertical Movement
    boss.speed.y += GRAVITY * DELTA_TIME;
    float dy = boss.speed.y * DELTA_TIME;
    float step_y = (dy > 0) ? 1 : -1;

    for (float y = 0; y < fabsf(dy); y++) {
        boss.hitbox.y += step_y;

        if (handle_collision_with_walls(boss.hitbox)) {
            boss.hitbox.y -= step_y;
            boss.speed.x = 0;
            boss.speed.y = 0;

            if (step_y > 0) {
                boss.state = BOSS_IDLE;
                boss.state_timer = 0.8f;
            }

            break;
        }
    }
}

void boss_idle() {
    Player *player = &game_state.player;

    // Engage when player is nearby
    float dx = player->hitbox.x - boss.hitbox.x;
    float dy = player->hitbox.y - boss.hitbox.y;
    float d = sqrtf(dx * dx + dy * dy);

    if (d < BOSS_ENGAGE_RANGE) boss.is_engaged = true;
    if (!boss.is_engaged) return;

    if (boss.state_timer > 0)
        boss.state_timer -= DELTA_TIME;
    else {
        int attack = GetRandomValue(BOSS_CHASE_ATTACK, BOSS_JUMP_ATTACK);

        if (attack == BOSS_CHASE_ATTACK)
            start_chase_attack();
        else start_jump_attack();

        boss.state_timer = 1.5f;
    }
}

void update_boss() {
    if (!boss.is_active) return;

    switch (boss.state) {
        case BOSS_IDLE:
            boss_idle();
            break;
        case BOSS_CHASE_ATTACK:
            boss_chase();
            break;
        case BOSS_JUMP_ATTACK:
            boss_jump();
            break;
    }

    float dx = boss.speed.x * DELTA_TIME;
    float step = (dx > 0) ? 1 : -1;

    for (int x = 0; x < fabsf(dx); x++) {
        boss.hitbox.x += step;

        if (handle_collision_with_walls(boss.hitbox)) {
            boss.hitbox.x -= step;
            boss.speed.x = 0;
            break;
        }
    }

    if (boss.invuln_timer > 0)
        boss.invuln_timer -= DELTA_TIME;
    else
        boss.invulnerable = false;

    if (boss.hurt_timer > 0)
        boss.hurt_timer -= DELTA_TIME;
}

Boss *handle_collision_with_boss(Rectangle hitbox, int damage) {
    Boss *hit_boss = NULL;

    if (boss.is_active && !boss.invulnerable && CheckCollisionRecs(hitbox, boss.hitbox)) {
        if (damage > 0) {
            boss.life -= damage;
            handle_boss_death();

            boss.invulnerable = true;
            boss.invuln_timer = BOSS_INVULN_TIMER;
            boss.hurt_timer = BOSS_HURT_TIMER;
        }

        hit_boss = &boss;
    }
    return hit_boss;
}

void handle_boss_death() {
    if (boss.life > 0) return;

    boss.life = 0;
    boss.is_active = false;
    game_state.level++;
    set_screen(SCREEN_VILLAGE);

    add_player_money(GetRandomValue(150, 250));
}
