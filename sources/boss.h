#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include "player.h"
#include "wall.h"

typedef enum {
    BOSS_IDLE,
    BOSS_CHASE_ATTACK,
    BOSS_JUMP_ATTACK
} BossState;

typedef struct {
    Rectangle hitbox;
    Vector2 velocity;

    int life;
    int max_life;

    bool active;
    bool engaged;

    // Invulnerabilidade
    bool invulnerable;
    float invuln_time;
    float hurt_timer;

    // Máquina de estados
    BossState state;
    float state_timer;

    // Pulo
    bool is_jumping;
    float jump_velocity;
    float target_x;
    float target_y;
    bool just_landed;
    float landing_delay;

} Boss;

extern Boss boss;

void spawn_boss(Rectangle start);
void update_boss(Player *player, float delta, Wall *walls, int wall_count);
void draw_boss();

#endif
