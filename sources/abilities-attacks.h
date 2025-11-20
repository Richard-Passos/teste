//
// Created by Harry on 10/11/2025.
//

#ifndef HOLLOW_ABILITIES_ATTACKS_H
#define HOLLOW_ABILITIES_ATTACKS_H
#include "player.h"

#define MAX_ABILITIES 1

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
    bool acquired;
} Ability;

void add_ability(int, int, Texture2D);

void draw_abilities();

void AbilitiesProjectile(Player *player, float delta);

void update_ability_acquisition();

bool DashAbility(Player *player, float delta);

void HealAbility(Player *player, float delta);

void draw_healing_effect();

extern Ability abilities[MAX_ABILITIES];
extern int abilities_count;
#endif //HOLLOW_ABILITIES_ATTACKS_H
