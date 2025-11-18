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
}

// ------------------------------
// ATAQUES
// ------------------------------

void start_chase_attack() {
    boss.state = BOSS_CHASE_ATTACK;
    boss.state_timer = 2.0f;      // dura 2 segundos
}

void boss_chase(Player *player, float delta) {
    float dx = player->hitbox.x - boss.hitbox.x;
    float dy = player->hitbox.y - boss.hitbox.y;
    float d = sqrtf(dx * dx + dy * dy);

    if (d > 0) {
        float speed = 250;
        boss.velocity.x = (dx / d) * speed;
        boss.velocity.y = (dy / d) * speed;
    }

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
    feet.y += feet.height + 1; // 1 pixel abaixo dos pés
    feet.height = 2;

    for (int i = 0; i < wall_count; i++) {
        if (CheckCollisionRecs(feet, walls[i].hitbox)) {
            return true;
        }
    }
    return false;
}

void start_jump_attack(Player *player) {
    boss.state = BOSS_JUMP_ATTACK;

    boss.is_jumping = true;
    boss.jump_velocity = -BOSS_JUMP_SPEED;   // sobe

    // posição onde o player estava quando o ataque começou
    boss.target_x = player->hitbox.x;
    boss.target_y = player->hitbox.y;

    // velocidade horizontal em direção ao alvo
    float dx = boss.target_x - boss.hitbox.x;
    boss.velocity.x = (dx > 0) ? 250 : -250;

    // tempo que vai ficar lá em cima
    boss.state_timer = BOSS_JUMP_HOLD_TIME;
}

void boss_jump(Player *player, float delta, Wall *walls, int wall_count) {
    // =======================================================
    // 1. SUBIDA ATÉ O TETO
    // =======================================================
    if (boss.is_jumping) {
        boss.hitbox.y += boss.jump_velocity * delta;

        // gravidade invertida durante subida
        boss.jump_velocity += 2000 * delta;

        // bateu no teto (pico)
        if (boss.hitbox.y <= BOSS_JUMP_PEAK_Y) {
            boss.hitbox.y = BOSS_JUMP_PEAK_Y;
            boss.jump_velocity = 0;
            boss.is_jumping = false;   // fim da subida
        }

        // movimento horizontal durante o pulo também
        boss.hitbox.x += boss.velocity.x * delta;

        return;
    }

    // =======================================================
    // 2. SEGURANDO NO TOPO
    // =======================================================
    if (boss.state_timer > 0) {
        boss.state_timer -= delta;
        return;
    }

    // ========================
    // 3. QUEDA DIRECIONADA
    // ========================

    boss.jump_velocity += 3000 * delta;

    // movimento vertical
    float dy = boss.jump_velocity * delta;

    // aplica dy passo-a-passo para garantir colisão perfeita
    float step = (dy > 0) ? 1 : -1;
    for (float i = 0; i < fabsf(dy); i += 1.0f) {
        boss.hitbox.y += step;

        if (boss_has_ground(walls, wall_count)) {
            // recua 1 pixel até ficar imediatamente acima do chão
            boss.hitbox.y -= step;

            // boss aterrissou
            boss.jump_velocity = 0;

            // dano no jogador
            if (CheckCollisionRecs(boss.hitbox, player->hitbox)) {
                player->speed.x = (player->hitbox.x < boss.hitbox.x) ? -500 : 500;
                player->speed.y = -250;
            }

            boss.state = BOSS_IDLE;
            boss.velocity = (Vector2){0};
            boss.state_timer = 1.5f;

            return;
        }
    }
}

// --------------------------------
// MÁQUINA DE ESTADOS
// --------------------------------

void boss_idle(Player *player, float delta) {

    // Engaja quando o player se aproxima
    float dx = player->hitbox.x - boss.hitbox.x;
    float dy = player->hitbox.y - boss.hitbox.y;
    float d = sqrtf(dx*dx + dy*dy);

    if (d < 400) boss.engaged = true;
    if (!boss.engaged) return;

    boss.state_timer -= delta;
    if (boss.state_timer > 0) return;

    // escolhe ataque random
    int attack = GetRandomValue(1, 2);

    if (attack == 1) start_chase_attack();
    else start_jump_attack(player);

    boss.state_timer = 1.5f;
}

// --------------------------------
// UPDATE GERAL
// --------------------------------

void update_boss(Player *player, float delta, Wall *walls, int wall_count) {
    if (!boss.active) return;

    switch (boss.state) {
        case BOSS_IDLE:
            boss_idle(player, delta);
            break;

        case BOSS_CHASE_ATTACK:
            boss_chase(player, delta);
            break;

        case BOSS_JUMP_ATTACK:
            boss_jump(player, delta, walls, wall_count);
            // EVITA MOVER DUAS VEZES
            break;
    }

    // NÃO mover automaticamente durante o pulo!
    if (boss.state != BOSS_JUMP_ATTACK) {
        boss.hitbox.x += boss.velocity.x * delta;
        boss.hitbox.y += boss.velocity.y * delta;
    }

    // colisão geral
    for (int i = 0; i < wall_count; i++) {
        if (CheckCollisionRecs(boss.hitbox, walls[i].hitbox)) {
            boss.hitbox.x -= boss.velocity.x * delta;
            boss.hitbox.y -= boss.velocity.y * delta;
            break;
        }
    }

    // invulnerabilidade
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

    // barra de vida
    DrawRectangle(boss.hitbox.x,
                  boss.hitbox.y - 20,
                  (boss.life * boss.hitbox.width) / boss.max_life,
                  10,
                  BLACK);
}
