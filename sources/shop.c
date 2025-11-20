//
// Created by richa on 16/11/2025.
//

#include "raylib.h"
#include "config.h"
#include "shop.h"
#include "game_state.h"

Shop SHOP = {0};

void add_shop(int x, int y) {
    SHOP.hitbox = (Rectangle){
        x * TILE_SIZE, y * TILE_SIZE - TILE_SIZE * 2, TILE_SIZE * 3, TILE_SIZE * 3
    };
    SHOP.should_draw = true;
}

void draw_shop() {
    if (SHOP.should_draw) {
        DrawRectangle(SHOP.hitbox.x, SHOP.hitbox.y, SHOP.hitbox.width, SHOP.hitbox.height, ORANGE);

        if (CheckCollisionRecs(game_state.player.hitbox, SHOP.hitbox)) {
            DrawText("Entrar",
                     SHOP.hitbox.x + 40,
                     SHOP.hitbox.y - 50,
                     32,
                     WHITE);
        }
    }
}

bool handle_shop_interaction() {
    bool is_ok = false;

    if (CheckCollisionRecs(game_state.player.hitbox, SHOP.hitbox)) {
        if (IsKeyPressed(KEY_UP))
            is_ok = true;
    }

    return is_ok;
}
