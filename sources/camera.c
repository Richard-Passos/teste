//
// Created by richa on 26/10/2025.
//

#include "raylib.h"
#include "player.h"

void update_camera_center(Camera2D *camera, Player *player, float width, float height)
{
    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
     camera->target = (Vector2){player->hitbox.x, player->hitbox.y};
}