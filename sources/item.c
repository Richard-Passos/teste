//
// Created by richa on 16/11/2025.
//

#include "item.h"
#include "config.h"

Item items[MAX_ITEMS];
int items_count = 0;

void add_item(int x, int y) {
    if (items_count < MAX_ITEMS) {
        items[items_count++] = (Item){{x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}};
    }
}

void draw_items() {
    for (int i = 0; i < items_count; i++)
        DrawRectangleRec(items[i].hitbox, GOLD);
}
