//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_SHOP_H
#define HOLLOW_SHOP_H
#include "raylib.h"

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
    bool should_draw;
} Shop;

void add_shop(int, int, Texture2D);

void draw_shop();

bool handle_shop_interaction();

extern Shop shop;
#endif //HOLLOW_SHOP_H
