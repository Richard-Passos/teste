//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_SHOP_H
#define HOLLOW_SHOP_H
#include "raylib.h"

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
    bool is_active;
} Shop;

void add_shop(int, int, Texture2D);

void draw_shop();

bool handle_shop_interaction();

void add_shop_npc(int, int, Texture2D);

void draw_shop_npc();

bool handle_shop_npc_interaction();

extern Shop shop;
extern Shop shop_npc;
#endif //HOLLOW_SHOP_H
