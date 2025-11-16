//
// Created by richa on 26/10/2025.
//

#include "raylib.h"
#include "config.h"

Camera2D camera = {0};

void init_camera(Vector2 target) {
    camera.target = target;
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void update_camera_center(Vector2 target) {
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.target = target;
}


