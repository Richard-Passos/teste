//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_PLAYER_H
#define MY_RAYLIB_GAME_PLAYER_H
#include "enemies.h"

typedef struct AbilityProjectile {
    Rectangle hitbox;
    Vector2 speed;
    bool active;
    bool acquired;
} AbilityProjectile;

typedef struct {
    bool active;
    float timer;
    float cooldown;
    bool hit_confirmed;
    float recovery;
} DashState;

typedef struct {
    int life;
    int max_life;

    bool invulnerable;
    float invuln_timer;
    float hurt_timer;

    float push_timer;
    float push_dir;
    float push_speed;

    float heal_cooldown;
    float heal_cooldown_max;
    bool is_healing;
    float heal_hold_time;
    float heal_hold_needed;
} PlayerCombat;

typedef struct {
    Rectangle hitbox;
    Vector2 speed;
    bool can_jump;
    bool on_ground;
    int jump_count;
    bool is_attacking;
    float attack_timer;
    Rectangle attack_box;
    bool facing_right;
    bool monsters_hit[MAX_MONSTERS];
    int attack_dir; // 1 = cima, 0 = frente, -1 = baixo
    float attack_cooldown;
    int money;
    float money_gain_timer;
    int last_money_gain;
    int souls;
    int max_souls;
    AbilityProjectile abilitySoulProjectile;
    DashState dash;
    PlayerCombat combat;
    bool is_sitting;
    bool boss_hit;

    bool should_keep_pos;
} Player;

void add_player(int, int);

void draw_player();

void update_player(float delta);

#endif //MY_RAYLIB_GAME_PLAYER_H
