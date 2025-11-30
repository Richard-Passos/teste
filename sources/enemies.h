#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "main.h"

#define MAX_MONSTERS 10
#define LAND_MONSTER_HOR_SPEED 150.0f
#define LAND_MONSTER_LIFE 5
#define FLYING_MONSTER_LIFE 3
#define MONSTER_DAMAGE 1
#define MONSTER_HURT_TIMER 0.15f
#define MONSTER_INVULN_TIMER 0.3f

typedef struct {
    Vector2 speed;
    Rectangle hitbox;
    Direction direction;
    int life;
    int damage;
    float hurt_timer;
    float invuln_timer;
    bool invulnerable;
    bool on_ground;
    bool is_flying;
    bool is_active;
} Monster;

void add_monster(int x, int y);

void draw_monsters();

void handle_monster_variants();

void update_monsters();

void handle_monster_horizontal_movement(Monster *);

void handle_monster_vertical_movement(Monster *);

void handle_monster_monsters_collision(Monster *);

Monster *handle_collision_with_monster(Rectangle, int);

void handle_monster_death(Monster *);

void toggle_monster_direction(Monster *);

extern Monster monsters[MAX_MONSTERS];
extern int monsters_count;
#endif
