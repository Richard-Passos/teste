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
} Player;

Rectangle get_player_rect(Player player);

void update_player(
    Player *player,
    float delta
);

#endif //MY_RAYLIB_GAME_PLAYER_H