//
// Created by richa on 06/11/2025.
//

#ifndef HOLLOW_GAME_STATE_H
#define HOLLOW_GAME_STATE_H

#define MIN_LEVEL 1
#define MAX_LEVEL 3
#include "player.h"
#include "abilities.h"
#include "item.h"

typedef struct {
    int level;
    Player player;
    Ability abilities[MAX_ABILITIES];
    int abilities_count;
    Item items[MAX_ITEMS];
    int items_count;

    char recent_ability_text[200];
    float recent_ability_timer;
} Game_state;

bool save_game_state();

bool load_game_state();

void reset_game_state();

extern Game_state game_state;
extern int game_state_size;
#endif //HOLLOW_GAME_STATE_H
