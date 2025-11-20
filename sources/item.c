//
// Created by richa on 16/11/2025.
//

#include "item.h"
#include "config.h"

Item items[MAX_ITEMS];
int items_count = 0;

void add_item(int x, int y, Texture2D texture) {
    if (items_count < MAX_ITEMS) {
        items[items_count++] = (Item){texture, {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}};
    }
}

void draw_items() {
    for (int i = 0; i < items_count; i++)
        DrawTexturePro(
            items[i].texture,
            (Rectangle){0, 0, items[i].texture.width, items[i].texture.height},
            items[i].hitbox,
            (Vector2){0, 0},
            0.0f,
            ORANGE
        );
}
