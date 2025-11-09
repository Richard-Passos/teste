#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "config.h"

#define WALL_SIZE 32
#define MAX_MONSTERS 100

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
} Wall;


typedef struct {
    Vector2 speed;     // velocidade x/y
    Rectangle hitbox;  // posição e tamanho
    int direction;     // 1 = direita, -1 = esquerda
    Rectangle floor_checker;    // checar se tem chao na diagonal
    bool on_ground;
    bool is_flying;
    int life;
    float hurt_timer;
    bool invulnerable;
    float invuln_time;
} Monster;

bool check_collision_with_wall(Monster *m, Wall *wall);

extern Monster monsters[MAX_MONSTERS];
extern int monsters_count;

extern int walls_count;

Monster flying(Wall *walls);
void update_monsters(float delta, Wall *walls, int walls_count);
void draw_monsters(void);

#endif
