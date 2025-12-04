//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_ITEM_H
#define HOLLOW_ITEM_H
#include "raylib.h"

#define MAX_ITEMS 99

#define MAX_ITEMS_ACTIVATED 3
#define RECENT_ITEM_TIMER 2.5f


typedef struct {
    char label[100];
    char texture_path[100];
    char description[200];
    bool is_buyable;
    bool is_acquired;
    bool is_active;
    Texture2D texture;
    Rectangle hitbox;
    int cost;
} Item;

void add_item(int, int);

void draw_items();

void unload_items();

Item *get_available_item();

bool is_item_active(Item *);

void update_item_acquisition();

#endif //HOLLOW_ITEM_H
