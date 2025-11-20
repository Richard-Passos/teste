//
// Created by richa on 16/11/2025.
//

#include "raylib.h"
#include "config.h"
#include "shop.h"
#include "game_state.h"

Shop shop = {0};

void add_shop(int x, int y, Texture2D texture) {
    shop.texture = texture;
    shop.hitbox = (Rectangle){
        x * TILE_SIZE, y * TILE_SIZE - TILE_SIZE * 2, TILE_SIZE * 3, TILE_SIZE * 3
    };
    shop.should_draw = true;
}

void draw_shop() {
    if (!shop.should_draw) return;

    DrawTexturePro(
        shop.texture,
        (Rectangle){0, 0, shop.texture.width, shop.texture.height},
        shop.hitbox,
        (Vector2){0, 0},
        0.0f,
        ORANGE
    );

    if (CheckCollisionRecs(game_state.player.hitbox, shop.hitbox)) {
        DrawText("Entrar",
                 shop.hitbox.x + 40,
                 shop.hitbox.y - 50,
                 32,
                 WHITE);
    }
}

bool handle_shop_interaction() {
    bool is_ok = false;

    if (CheckCollisionRecs(game_state.player.hitbox, shop.hitbox)) {
        if (IsKeyPressed(KEY_UP))
            is_ok = true;
    }

    return is_ok;
}
