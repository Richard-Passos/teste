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

typedef enum {
    menu,
    paused,
    inventory,
    start,
    win,
    game_over,
    help
} Screen;

int handle_screens();

void handle_menu_screen();

void handle_paused_screen();

void handle_inventory_screen();

void handle_help_screen();

void set_screen(Screen);

bool is_button_pressed(Asset);

void draw_asset(Asset);

Asset add_asset(char [], Rectangle);

extern Screen screen;
extern bool is_screen_loaded;
#endif //HOLLOW_SCREEN_H
