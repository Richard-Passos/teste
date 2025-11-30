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

        spawn_player_on_bench();

        UnloadFileData(file_data);

        is_ok = true;
    }

    return is_ok;
}

void reset_game_state() {
    Player player = {0};

    game_state.abilities[0] = (Ability){"../assets/dash.png",false, false};
    game_state.abilities[1] = (Ability){"../assets/soul_projectile.png",false, false};
    game_state.abilities[2] = (Ability){"../assets/double_jump.png",false, false};
    game_state.abilities[3] = (Ability){"", true, false};
    game_state.abilities_count = 4;
    player.abilities.dash = &game_state.abilities[0];
    player.abilities.soul_projectile = &game_state.abilities[1];
    player.abilities.double_jump = &game_state.abilities[2];
    player.abilities.heal = &game_state.abilities[3];

    game_state.items[0] = (Item){"Mais Vida", "../assets/ability.png",false, false, false};
    game_state.items[1] = (Item){"Mais Dano", "../assets/ability.png", false, false, false, .cost = 50};
    game_state.items[2] = (Item){"Mais Velocidade", "../assets/ability.png", false, false, false};
    game_state.items[3] = (Item){"Mais dinheiro", "../assets/ability.png", false, false, false};
    game_state.items[4] = (Item){"Dash Invulneravel", "../assets/ability.png", false, false, false, .cost = 10};
    game_state.items_count = 5;
    player.items.add_life = &game_state.items[0];
    player.items.add_damage = &game_state.items[1];
    player.items.add_speed = &game_state.items[2];
    player.items.add_money = &game_state.items[3];
    player.items.add_invuln_dash = &game_state.items[4];

    player.hitbox = (Rectangle){0, 0,TILE_SIZE, TILE_SIZE};
    player.speed = (Vector2){0, 0};
    player.on_ground = false;
    player.money = 0;

    player.combat.life = 5;
    player.combat.max_life = 5;
    player.combat.damage = 1;
    player.combat.invulnerable = false;
    player.combat.invuln_timer = 0;
    player.combat.hurt_timer = 0;
    player.combat.is_attacking = false;
    player.combat.attack_timer = 0.0f;
    player.combat.souls = 0;
    player.combat.max_souls = 100;

    player.is_sitting = false;
    player.has_spawn = false;
    player.multipliers.speed = 1;
    player.multipliers.money = 1;

    game_state.level = MIN_LEVEL;
    game_state.player = player;
}
