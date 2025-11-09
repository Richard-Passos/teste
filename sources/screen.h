//
// Created by richa on 05/11/2025.
//

#ifndef HOLLOW_SCREEN_H
#define HOLLOW_SCREEN_H
#include "raylib.h"

typedef struct {
    Texture texture;
    Rectangle hitbox;
} Asset;

int handle_screens();

void handle_menu_screen();

void handle_paused_screen();

void handle_inventory_screen();

void handle_help_screen();

void set_screen(char);

bool is_button_pressed(Asset);

void draw_asset(Asset);

Asset add_asset(char [], Rectangle);

extern char screen; /* m = menu | p = paused | i = inventory | s = start */;
extern bool is_screen_loaded;
#endif //HOLLOW_SCREEN_H
