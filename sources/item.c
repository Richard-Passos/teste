//
// Created by richa on 16/11/2025.
//

#include <minwindef.h>
#include "item.h"
#include "config.h"
#include "game_state.h"

void add_item(int x, int y) {
    Item *items = game_state.items;

    Item *item = get_available_item();

    if (!item) return;

    item->texture = LoadTexture(item->texture_path);
    item->hitbox = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    items->is_active = true;
}

void draw_items() {
    Item *items = game_state.items;
    int *items_count = &game_state.items_count;

    bool is_colliding;

    for (int i = 0; i < *items_count; i++) {
        if (items[i].is_acquired || !items[i].is_active) continue;

        DrawTexturePro(
            items[i].texture,
            (Rectangle){0, 0, items[i].texture.width, items[i].texture.height},
            items[i].hitbox,
            (Vector2){0, 0},
            0.0f,
            ORANGE
        );

        if (!is_colliding && CheckCollisionRecs(game_state.player.hitbox, items[i].hitbox)) {
            DrawText(
                "Inspecionar",
                items[i].hitbox.x - 50,
                items[i].hitbox.y - 50,
                30,
                WHITE
            );

            is_colliding = true;
        }
    }
}

void unload_items() {
    Item *items = game_state.items;
    int *items_count = &game_state.items_count;

    for (int i = 0; i < *items_count; i++)
        if (items[i].is_active) {
            UnloadTexture(items[i].texture);
            items[i].is_active = false;
        }
}

Item *get_available_item() {
    Item *items = game_state.items;
    int *items_count = &game_state.items_count;

    for (int i = 0; i < *items_count; i++)
        if (!items[i].is_active && !items[i].is_acquired && !items[i].is_buyable)
            return &items[i];

    return NULL;
}

