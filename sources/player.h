//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_PLAYER_H
#define MY_RAYLIB_GAME_PLAYER_H
#include "enemies.h"
#include "abilities.h"
#include "item.h"

#define PLAYER_JUMP_SPEED 500.0f
#define PLAYER_HOR_SPEED 600.0f
#define PLAYER_INVULN_TIMER 1.0f
#define PLAYER_HURT_TIMER 0.15f
#define PLAYER_ATTACK_TIMER 0.12f
#define PLAYER_ATTACK_COOLDOWN 0.4f

typedef struct {
    float speed;
    float money;
} Multipliers;

typedef struct {
    Rectangle attack_box;
    int life;
    int max_life;
    int damage;
    int souls;
    int max_souls;
    float invuln_timer;
    float hurt_timer;
    float attack_timer;
    float attack_cooldown;
    bool is_attacking;
    bool monsters_hit[MAX_MONSTERS];
    bool invulnerable;
} Player_combat;

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
    Direction direction;
    Player_combat combat;
    Abilities abilities;
    Items items;
    Multipliers multipliers;
    Vector2 spawn_pos;
    int jump_count;
    int money;
    int last_money_gain;
    float money_gain_timer;
    bool ignore_next_monster_hit;
    bool on_ground;
    bool should_keep_pos;
    bool has_spawn;
    bool is_sitting;
} Player;

void add_player(int, int);

void draw_player();

void update_player_items();

void update_player();

void handle_player_horizontal_movement();

void handle_player_vertical_movement();

void handle_player_attack();

void handle_player_monsters_collision();

void spawn_player_on_bench();

void add_player_money(int);

void add_player_souls(int);

#endif //MY_RAYLIB_GAME_PLAYER_H
