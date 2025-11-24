//
// Created by Harry on 10/11/2025.
//

#ifndef HOLLOW_ABILITIES_ATTACKS_H
#define HOLLOW_ABILITIES_ATTACKS_H
#include "raylib.h"

#define MAX_ABILITIES 99

typedef struct {
    char texture_path[100];
    bool is_acquired;
    bool is_active;
    bool hit_confirmed;
    float lifetime;
    float timer;
    float cooldown;
    Texture2D texture;
    Rectangle hitbox;
    Vector2 speed;
} Ability;

void add_ability(int, int);

void draw_abilities();

void unload_abilities();

Ability *get_available_ability();

void soul_projectile_ability();

void update_ability_acquisition();

void dash_ability();

void heal_ability();

void draw_healing_effect();

void double_jump_ability();
#endif //HOLLOW_ABILITIES_ATTACKS_H
