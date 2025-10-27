//
// Created by richa on 26/10/2025.
//

//
// Created by richa on 22/10/2025.
//
#include "raylib.h"
#include "player.h"

void update_camera_center(Camera2D *camera, Player *player, int width, int height)
{
    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    camera->target = player->position;
}