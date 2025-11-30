#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include "player.h"
#include "wall.h"

#define BOSS_LIFE (LAND_MONSTER_LIFE * 4)
#define BOSS_DAMAGE 2
#define BOSS_SPEED 225
#define BOSS_JUMP_TIME 0.9f
#define BOSS_INVULN_TIMER 0.3f
#define BOSS_HURT_TIMER 0.15f
#define BOSS_ENGAGE_RANGE 400

typedef enum {
    BOSS_IDLE,
    BOSS_CHASE_ATTACK,
    BOSS_JUMP_ATTACK
} BossState;

typedef struct {
    Rectangle hitbox;
    Vector2 speed;

    int life;
    int max_life;
    int damage;

    bool is_active;
    bool is_engaged;

    // Invulnerabilidade
    bool invulnerable;
    float invuln_timer;
    float hurt_timer;

    // Máquina de estados
    BossState state;
    float state_timer;
} Boss;

extern Boss boss;

void add_boss(int, int);

void update_boss();

void draw_boss();

Boss *handle_collision_with_boss(Rectangle, int);

void handle_boss_death();

#endif
