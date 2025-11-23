//
// Created by richa on 16/11/2025.
//

#include "raylib.h"
#include "config.h"
#include "shop.h"

#include <stdio.h>

#include "game_state.h"

Shop shop = {0};
Shop shop_npc = {0};

void add_shop(int x, int y, Texture2D texture) {
    shop.texture = texture;
    shop.hitbox = (Rectangle){
        x * TILE_SIZE, y * TILE_SIZE - TILE_SIZE * 2, TILE_SIZE * 3, TILE_SIZE * 3
    };
    shop.is_active = true;
}

void draw_shop() {
    if (!shop.is_active) return;

    DrawTexturePro(
        shop.texture,
        (Rectangle){0, 0, shop.texture.width, shop.texture.height},
        shop.hitbox,
        (Vector2){0, 0},
        0.0f,
        WHITE
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

    if (shop.is_active && CheckCollisionRecs(game_state.player.hitbox, shop.hitbox) && IsKeyPressed(KEY_UP))
        is_ok = true;

    return is_ok;
}

void add_shop_npc(int x, int y, Texture2D texture) {
    shop_npc.texture = texture;
    shop_npc.hitbox = (Rectangle){
        x * TILE_SIZE, y * TILE_SIZE - TILE_SIZE, TILE_SIZE * 2, TILE_SIZE * 2
    };
    shop_npc.is_active = true;
}

void draw_shop_npc() {
    if (!shop_npc.is_active) return;

    DrawTexturePro(
        shop_npc.texture,
        (Rectangle){0, 0, shop_npc.texture.width, shop_npc.texture.height},
        shop_npc.hitbox,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

    if (CheckCollisionRecs(game_state.player.hitbox, shop_npc.hitbox)) {
        DrawText("Comprar",
                 shop_npc.hitbox.x,
                 shop_npc.hitbox.y - 50,
                 32,
                 WHITE);
    }
}

bool handle_shop_npc_interaction() {
    bool is_ok = false;

    if (shop_npc.is_active && CheckCollisionRecs(game_state.player.hitbox, shop_npc.hitbox) && IsKeyPressed(KEY_UP))
        is_ok = true;

    return is_ok;
}
