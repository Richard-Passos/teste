//
// Created by Harry on 16/11/2025.
//

#include "boss.h"
#include "raylib.h"
#include "config.h"
#include <math.h>

Boss boss = {0};

void spawn_boss(Rectangle start) {
    boss.hitbox = (Rectangle){
        start.x,
        start.y - 64,
        TILE_SIZE * 2,
        TILE_SIZE * 2
    };

    boss.velocity = (Vector2){0};

    boss.max_life = 25;
    boss.life = boss.max_life;

    boss.active = true;
    boss.engaged = false;

    boss.state = BOSS_IDLE;
    boss.state_timer = 1.0f;

    boss.is_jumping = false;
    boss.jump_velocity = 0;

    boss.just_landed = false;
}

// ------------------------------
// ATAQUES
// ------------------------------

void start_chase_attack() {
    boss.state = BOSS_CHASE_ATTACK;
    boss.state_timer = 2.0f;
}

void boss_chase(Player *player, float delta) {

    float speed = 225;

    if (player->hitbox.x < boss.hitbox.x)
        boss.velocity.x = -speed;
    else if (player->hitbox.x > boss.hitbox.x)
        boss.velocity.x = speed;

    boss.velocity.y = 0;

    boss.state_timer -= delta;
    if (boss.state_timer <= 0) {
        boss.state = BOSS_IDLE;
        boss.velocity = (Vector2){0};
    }
}

// =======================================================
// NOVO SISTEMA DE PULO
// =======================================================

bool boss_has_ground(Wall *walls, int wall_count) {
    Rectangle feet = boss.hitbox;
    feet.y += feet.height;
    feet.height = 1;

    for (int i = 0; i < wall_count; i++) {
        if (CheckCollisionRecs(feet, walls[i].hitbox)) {
            return true;
        }
    }
    return false;
}

void start_jump_attack(Player *player) {
    boss.state = BOSS_JUMP_ATTACK;

    // TARGET
    float dx = (player->hitbox.x + player->hitbox.width/2) - (boss.hitbox.x + boss.hitbox.width/2);
    float dy = (player->hitbox.y) - boss.hitbox.y;

    float jump_time = 0.9f; // tempo total da parábola

    // VELOCIDADE HORIZONTAL → parabólica real
    boss.velocity.x = dx / jump_time;

    float base_vel = (dy - 0.5f * GRAVITY * jump_time * jump_time) / jump_time;

    float jump_boost = 1.5f; //
    boss.jump_velocity = base_vel * jump_boost;

    // RESET
    boss.state_timer = jump_time;
    boss.is_jumping = true;

    boss.just_landed = false;
    boss.landing_delay = 0.0f;
}


void boss_jump(float delta, Wall *walls, int wall_count) {

    // ----- MOVIMENTO HORIZONTAL PARABÓLICO -----
    float dx = boss.velocity.x * delta;
    float step_x = (dx > 0) ? 1 : -1;

    for (float x = 0; x < fabsf(dx); x += 1) {
        boss.hitbox.x += step_x;

        for (int w = 0; w < wall_count; w++) {
            if (CheckCollisionRecs(boss.hitbox, walls[w].hitbox)) {
                boss.hitbox.x -= step_x;
                boss.velocity.x = 0; // bateu na parede → para
                x = fabsf(dx);
                break;
            }
        }
    }

    // ----- MOVIMENTO VERTICAL PARABÓLICO -----
    boss.jump_velocity += GRAVITY * delta;
    float dy = boss.jump_velocity * delta;
    float step_y = (dy > 0) ? 1 : -1;

    for (float y = 0; y < fabsf(dy); y += 1.0f) {

        Rectangle next = boss.hitbox;
        next.y += step_y;

        // COLISÃO COM O CHÃO
        if (step_y > 0) {
            for (int w = 0; w < wall_count; w++) {
                if (CheckCollisionRecs(next, walls[w].hitbox)) {

                    // ENCOSTOU NO CHÃO
                    boss.hitbox.y = walls[w].hitbox.y - boss.hitbox.height;

                    boss.state = BOSS_IDLE;

                    boss.velocity.x = 0;
                    boss.jump_velocity = 0;

                    boss.just_landed = true;
                    boss.landing_delay = 0.3f; // boss não anda nos próximos 300ms

                    boss.state_timer = 1.2f; // tempo até escolher outro ataque

                    return;
                }
            }
        }

        // COLISÃO COM O TETO
        if (step_y < 0) {
            for (int w = 0; w < wall_count; w++) {
                if (CheckCollisionRecs(next, walls[w].hitbox)) {

                    // bateu no teto → gruda logo abaixo dele
                    boss.hitbox.y = walls[w].hitbox.y + walls[w].hitbox.height;

                    boss.jump_velocity = 0; // corta subida

                    return; // para de subir
                }
            }
        }


        boss.hitbox.y += step_y;
    }
}


// --------------------------------
// MÁQUINA DE ESTADOS
// --------------------------------

void boss_idle(Player *player, float delta) {
    float dx = player->hitbox.x - boss.hitbox.x;
    float dy = player->hitbox.y - boss.hitbox.y;
    float d = sqrtf(dx*dx + dy*dy);

    if (d < 400) boss.engaged = true;
    if (!boss.engaged) return;

    boss.state_timer -= delta;
    if (boss.state_timer > 0) return;

    int attack = GetRandomValue(1, 2);

    if (attack == 1) start_chase_attack();
    else start_jump_attack(player);

    boss.state_timer = 1.5f;
}

// --------------------------------
// UPDATE GERAL
// --------------------------------

void update_boss(Player *player, float delta, Wall *walls, int wall_count) {
    if (!boss.active)
        return;

    if (boss.just_landed) {
        boss.landing_delay -= delta;
        if (boss.landing_delay > 0) {
            return; // impede boss de se mover ou escolher ataque
        }
        boss.just_landed = false;
    }


    switch (boss.state) {
        case BOSS_IDLE:
            boss_idle(player, delta);
            break;

        case BOSS_CHASE_ATTACK:
            boss_chase(player, delta);
            break;

        case BOSS_JUMP_ATTACK:
            boss_jump(delta, walls, wall_count);
            break;
    }

    // Se acabou de pousar do pulo: NÃO MOVIMENTAR NESSE FRAME
    if (boss.just_landed) {
        boss.just_landed = false;
        return;
    }

    // Movimento automático APENAS se não estiver pulando
    if (boss.state != BOSS_JUMP_ATTACK) {

        float dx = boss.velocity.x * delta;
        float step = (dx > 0) ? 1 : -1;

        for (float x = 0; x < fabsf(dx); x += 1.0f) {
            boss.hitbox.x += step;

            for (int w = 0; w < wall_count; w++) {
                if (CheckCollisionRecs(boss.hitbox, walls[w].hitbox)) {
                    boss.hitbox.x -= step;
                    boss.velocity.x = 0;
                    x = fabsf(dx);
                    break;
                }
            }
        }

        boss.hitbox.y += boss.velocity.y * delta;
    }

    // COLISÃO GERAL
    for (int i = 0; i < wall_count; i++) {
        if (CheckCollisionRecs(boss.hitbox, walls[i].hitbox)) {
            boss.hitbox.x -= boss.velocity.x * delta;
            boss.hitbox.y -= boss.velocity.y * delta;
            break;
        }
    }

    if (boss.invulnerable) {
        boss.invuln_time -= delta;
        if (boss.invuln_time <= 0) {
            boss.invulnerable = false;
            boss.invuln_time = 0;
        }
    }

    if (boss.hurt_timer > 0) {
        boss.hurt_timer -= delta;
        if (boss.hurt_timer < 0) boss.hurt_timer = 0;
    }
}

// --------------------------------
// DRAW
// --------------------------------

void draw_boss() {
    if (!boss.active) return;

    Color color = PURPLE;
    if (boss.hurt_timer > 0) color = WHITE;

    DrawRectangleRec(boss.hitbox, color);

    DrawRectangle(
        boss.hitbox.x,
        boss.hitbox.y - 20,
        (boss.life * boss.hitbox.width) / boss.max_life,
        10,
        BLACK
    );
}
