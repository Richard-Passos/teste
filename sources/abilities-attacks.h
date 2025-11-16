//
// Created by Harry on 10/11/2025.
//

#ifndef HOLLOW_ABILITIES_ATTACKS_H
#define HOLLOW_ABILITIES_ATTACKS_H
#include "player.h"

#define MAX_ABILITIES 99

typedef struct {
    Rectangle hitbox;
    bool acquired;
} Ability;

void add_ability(int, int);

void draw_abilities();

void AbilitiesProjectile(Player *player, float delta);

void UpdateAbilityAcquisition(Player *player);

bool DashAbility(Player *player);

void HealAbility(Player *player, float delta);

void DrawHealingEffect();

extern Ability abilities[MAX_ABILITIES];
extern int abilities_count;
#endif //HOLLOW_ABILITIES_ATTACKS_H
