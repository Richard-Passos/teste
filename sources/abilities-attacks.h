//
// Created by Harry on 10/11/2025.
//

#ifndef HOLLOW_ABILITIES_ATTACKS_H
#define HOLLOW_ABILITIES_ATTACKS_H

#include "player.h"


void AbilitiesProjectile(Player *player, float delta);

void UpdateAbilityAcquisition(Player *player);

bool DashAbility(Player *player, Monster *monsters, int monsters_count, float delta);

void HealAbility(Player *player, float delta);
void DrawHealingEffect(Player *player);

void DrawAbilities();


#endif //HOLLOW_ABILITIES_ATTACKS_H