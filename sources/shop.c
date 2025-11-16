//
// Created by richa on 16/11/2025.
//

#include "shop.h"
#include "raylib.h"
#include "game_state.h"
#include "player.h"

Rectangle shop_hitbox = {0};

void draw_shop() {
    DrawRectangle(shop_hitbox.x, shop_hitbox.y, shop_hitbox.width, shop_hitbox.height, ORANGE);
}

bool handle_shop_interaction() {
    bool is_ok = false;

    Player *player = &game_state.player;

    if (CheckCollisionRecs(player->hitbox, shop_hitbox)) {
        if (IsKeyPressed(KEY_UP))
            is_ok = true;
    }

    return is_ok;
}