//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_PLAYER_H
#define MY_RAYLIB_GAME_PLAYER_H
#include "enemies.h"
#include "abilities.h"
#include "item.h"

typedef struct {
    float speed;
    float money;
} Multipliers;

typedef struct {
    bool active;
    float timer;
    float cooldown;
    bool hit_confirmed;
    float recovery;
} Dash_state;

typedef struct {
    int life;
    int max_life;
    int damage;

    bool invulnerable;
    float invuln_timer;
    float hurt_timer;

    float push_timer;
    float push_dir;
    float push_speed;
} PlayerCombat;

typedef struct {
    Ability *soul_projectile;
    Ability *dash;
    Ability *double_jump;
    Ability *heal;
} Abilities;

typedef struct {
    Item *add_life;
    Item *add_damage;
    Item *add_speed;
    Item *add_money;
    Item *add_invuln_dash;
} Items;

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
    PlayerCombat combat;
    bool ignore_next_monster_hit;
    bool is_sitting;
    bool boss_hit;

    Abilities abilities;
    Items items;
    Multipliers multipliers;

    bool should_keep_pos;
    Vector2 spawn_pos;
    bool has_spawn;
} Player;

void add_player(int, int);

void draw_player();

bool has_collided(Player *, Wall *);

void update_player_items();

void update_player();

void handle_timers();

void handle_horizontal_movement();

void handle_vertical_movement();

void handle_attack();

void handle_monsters_collision();

void spawn_player_on_bench();

void add_player_money(int);

void add_player_souls(int);

#endif //MY_RAYLIB_GAME_PLAYER_H
