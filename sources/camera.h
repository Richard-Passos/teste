//
// Created by richa on 26/10/2025.
//

#ifndef HOLLOW_CAMERA_H
#define HOLLOW_CAMERA_H
#include "raylib.h"
#include "player.h"

Camera2D init_camera(Vector2);

void update_camera_center(Vector2);

extern Camera2D camera;
#endif //HOLLOW_CAMERA_H
