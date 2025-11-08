//
// Created by richa on 06/11/2025.
//

#ifndef HOLLOW_GAME_STATE_H
#define HOLLOW_GAME_STATE_H

#define MIN_LEVEL 1
#define MAX_LEVEL 3

typedef struct {
    int level;
} Game_state;

bool save_game_state();

bool load_game_state();

extern Game_state game_state;
extern int game_state_size;
#endif //HOLLOW_GAME_STATE_H
