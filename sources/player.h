//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_PLAYER_H
#define MY_RAYLIB_GAME_PLAYER_H

typedef struct Player
{
    Vector2 position;
    Rectangle hitbox;
    float speed;
    bool canJump;
} Player;
#endif //MY_RAYLIB_GAME_PLAYER_H