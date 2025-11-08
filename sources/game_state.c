//
// Created by richa on 06/11/2025.
//

#include <string.h>
#include "raylib.h"
#include "config.h"
#include "game_state.h"

Game_state game_state = {MIN_LEVEL};
int game_state_size = sizeof(Game_state);

bool save_game_state() {
    return SaveFileData(GAME_STATE_FILE_PATH, &game_state, game_state_size);
}

bool load_game_state() {
    bool is_ok = false;

    unsigned char *file_data = LoadFileData(GAME_STATE_FILE_PATH, &game_state_size);

    if (file_data != NULL) {
        // Copy bytes into game_state
        memcpy(&game_state, file_data, game_state_size);

        UnloadFileData(file_data);

        is_ok = true;
    }

    return is_ok;
}
