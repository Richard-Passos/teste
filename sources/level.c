//
// Created by richa on 16/11/2025.
//

#include "raylib.h"
#include "level.h"
#include "config.h"
#include "player.h"
#include "game_state.h"
#include "screen.h"

void draw_level(Rectangle hitbox) {
    DrawRectangle(hitbox.x, hitbox.y, hitbox.width, hitbox.height, ORANGE);
}
