//
// Created by richa on 06/11/2025.
//

#include <string.h>
#include "raylib.h"
#include "game_state.h"
#include "config.h"

Game_state game_state;
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

void set_game_state(Game_state gs) {
    if (gs.level > MAX_LEVEL)
        game_state.level = MAX_LEVEL;
    else if (gs.level < MIN_LEVEL)
        game_state.level = MIN_LEVEL;
    else
        game_state.level = gs.level;
}

void reset_game_state() {
    Player player = {0};
    player.hitbox = (Rectangle){0, 0,TILE_SIZE, TILE_SIZE};
    player.speed = (Vector2){0, 0};
    player.can_jump = false;
    player.is_attacking = false;
    player.attack_timer = 0.0f;
    player.on_ground = false;
    player.money = 0;
    player.souls = 0;
    player.abilitySoulProjectile.active = false;
    player.abilitySoulProjectile.acquired = false;
    player.combat.life = 5;
    player.combat.max_life = 5;
    player.combat.invulnerable = false;
    player.combat.invuln_timer = 0;
    player.combat.hurt_timer = 0;
    player.combat.heal_hold_needed = 1.0f;

    game_state.level = MIN_LEVEL;
    game_state.player = player;
}
