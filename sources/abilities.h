//
// Created by Harry on 10/11/2025.
//

#ifndef HOLLOW_ABILITIES_ATTACKS_H
#define HOLLOW_ABILITIES_ATTACKS_H
#include "raylib.h"

#define MAX_ABILITIES 99

#define DASH_SPEED 1000.0f
#define DASH_DURATION 0.15f
#define DASH_COOLDOWN 1.0f

typedef struct {
    char label[100];
    char texture_path[100];
    char description[200];
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

void draw_ability_popup();

void unload_abilities();

Ability *get_available_ability();

bool is_ability_active(Ability *);

void soul_projectile_ability();

void update_ability_acquisition();

void dash_ability();

void heal_ability();

void draw_healing_effect();

void double_jump_ability();
#endif //HOLLOW_ABILITIES_ATTACKS_H
