//
// Created by richa on 05/11/2025.
//

#ifndef HOLLOW_SCREEN_H
#define HOLLOW_SCREEN_H
#include "raylib.h"

#define MAX_LABEL_SIZE 51

typedef struct {
    Texture texture;
    Rectangle hitbox;
} Asset;

typedef struct {
    char label[MAX_LABEL_SIZE];
    Rectangle hitbox;
    bool is_active;
} Action;

typedef enum {
    menu,
    paused,
    inventory,
    start,
    win,
    game_over,
    help
} Screen_name;

typedef struct {
    Screen_name name;
    bool is_loaded;
} Screen;

int handle_screens();

void handle_menu_screen();

void handle_paused_screen();

void handle_inventory_screen();

void handle_help_screen();

void set_screen(Screen_name);

bool is_button_pressed(Asset);

void draw_asset(Asset);

void add_asset(char [], Rectangle);

extern Screen screen;
#endif //HOLLOW_SCREEN_H
