//
// Created by richa on 06/11/2025.
//

#ifndef HOLLOW_GAME_STATE_H
#define HOLLOW_GAME_STATE_H

#define MIN_LEVEL 1
#define MAX_LEVEL 3
#include "player.h"
#include "abilities-attacks.h"
#include "item.h"

typedef struct {
    int level;
    Player player;
    Ability abilities[MAX_ABILITIES];
    int abilities_count;
    Item items[MAX_ITEMS];
    int items_count;
} Game_state;

bool save_game_state();

bool load_game_state();

void set_game_state(Game_state);

void reset_game_state();

extern Game_state game_state;
extern int game_state_size;
#endif //HOLLOW_GAME_STATE_H
