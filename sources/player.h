//
// Created by richa on 22/10/2025.
//

#ifndef MY_RAYLIB_GAME_PLAYER_H
#define MY_RAYLIB_GAME_PLAYER_H
#include "parede.h"

typedef struct Player
{
    Vector2 position;
    Rectangle hitbox;
    float speed;
    bool canJump;
} Player;

bool has_hit_side(Rectangle rec1, Rectangle rec2);
bool has_hit_obstacle(wall *w, Vector2 *pos, float delta, float speed);
void update_player(Player *player, wall *env_wall, int env_wall_length, float delta);
#endif //MY_RAYLIB_GAME_PLAYER_H