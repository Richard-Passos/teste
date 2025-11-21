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

        // Spawn player on last bench
        //--------------------------------------------------------------------------------------
        Player *player = &game_state.player;

        if (player->has_spawn) {
            player->hitbox = (Rectangle){
                player->spawn_pos.x, player->spawn_pos.y, player->hitbox.width, player->hitbox.height
            };
            player->is_sitting = true;
            player->speed.y = 0;
            player->combat.life = player->combat.max_life;
            player->souls = player->max_souls;
            player->should_keep_pos = true;
        }
        //--------------------------------------------------------------------------------------

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
    game_state.abilities[0] = (Ability){"../assets/ability.png",false, false};
    game_state.abilities_count = 1;

    game_state.items[0] = (Item){"Charm 1", "../assets/ability.png",false, false, false};
    game_state.items[1] = (Item){"Charm 2", "../assets/ability.png", true, false, false, .cost = 50};
    game_state.items[2] = (Item){"Charm 3", "../assets/ability.png", false, false, false};
    game_state.items[3] = (Item){"Charm 4", "../assets/ability.png", false, false, false};
    game_state.items[4] = (Item){"Charm 5", "../assets/ability.png", true, false, false, .cost = 10};
    game_state.items_count = 5;

    Player player = {0};
    player.hitbox = (Rectangle){0, 0,TILE_SIZE, TILE_SIZE};
    player.speed = (Vector2){0, 0};
    player.can_jump = false;
    player.is_attacking = false;
    player.attack_timer = 0.0f;
    player.on_ground = false;
    player.money = 0;
    player.souls = 0;
    player.max_souls = 100;
    player.abilitySoulProjectile = &game_state.abilities[0];
    player.combat.life = 5;
    player.combat.max_life = 5;
    player.combat.invulnerable = false;
    player.combat.invuln_timer = 0;
    player.combat.hurt_timer = 0;
    player.combat.heal_hold_needed = 1.0f;
    player.is_sitting = false;
    player.has_spawn = false;

    game_state.level = MIN_LEVEL;
    game_state.player = player;
}
