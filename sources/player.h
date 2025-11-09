//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_PLAYER_H
#define MY_RAYLIB_GAME_PLAYER_H
#include "map.h"

typedef struct {
    float x;
    float y;
} Speed;

typedef struct {
    Rectangle hitbox;
    Speed speed;
    bool can_jump;
    bool on_ground;
    int jump_count;
    bool is_attacking;
    float attack_timer;
    Rectangle attack_box;
    bool facing_right;
    bool monsters_hit[MAX_MONSTERS];
    int attack_dir; // 1 = cima, 0 = frente, -1 = baixo
    float attack_cooldown;


} Player;

Rectangle get_player_rect(Player player);


void update_player(
    Player *player,
    float delta
);

#endif //MY_RAYLIB_GAME_PLAYER_H