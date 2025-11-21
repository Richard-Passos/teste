//
// Created by Harry on 10/11/2025.
//

#ifndef HOLLOW_ABILITIES_ATTACKS_H
#define HOLLOW_ABILITIES_ATTACKS_H
#define MAX_ABILITIES 99
#include "raylib.h"

typedef struct {
    char texture_path[100];
    bool acquired;
    bool is_active;
    Texture2D texture;
    Rectangle hitbox;
    Vector2 speed;
    float lifetime;
} Ability;

void add_ability(int, int);

void draw_abilities();

void unload_abilities();

Ability *get_available_ability();

void AbilitiesProjectile(float);

void update_ability_acquisition();

bool DashAbility(float);

void HealAbility(float);

void draw_healing_effect();
#endif //HOLLOW_ABILITIES_ATTACKS_H
