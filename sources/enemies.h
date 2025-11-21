#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "wall.h"

#define MAX_MONSTERS 100

typedef struct {
    Vector2 speed; // velocidade x/y
    Rectangle hitbox; // posição e tamanho
    int direction; // 1 = direita, -1 = esquerda
    Rectangle floor_checker; // checar se tem chao na diagonal
    bool on_ground;
    bool is_flying;
    int life;
    float hurt_timer;
    bool invulnerable;
    float invuln_time;
} Monster;

void add_monster(int x, int y, float z, float w);

bool check_collision_with_wall(Monster *m, Wall *wall);

extern Monster monsters[MAX_MONSTERS];
extern int monsters_count;

void flying();

void update_monsters(float delta);

void draw_monsters();
#endif
