//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_ITEM_H
#define HOLLOW_ITEM_H
#include "raylib.h"

#define MAX_ITEMS 128

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
} Item;

void add_item(int, int, Texture2D);

void draw_items();

extern Item items[MAX_ITEMS];
extern int items_count;

#endif //HOLLOW_ITEM_H
