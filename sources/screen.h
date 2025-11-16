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
    char *label;
    Rectangle hitbox;
    int index;
} Action;

typedef enum {
    menu,
    paused,
    inventory,
    start,
    win,
    game_over,
    help,
    village,
    shop
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

void handle_win_screen();

void handle_game_over_screen();

void handle_village_screen();

void handle_shop_screen();

void set_screen(Screen_name);

void draw_asset(Asset *);

void add_asset(char [], Rectangle);

void draw_action(Action *);

void add_action(char [], Rectangle);

bool is_action_pressed(Action *);

float center_on_screen(float);

extern Screen screen;
extern Screen_name last_screen;
#endif //HOLLOW_SCREEN_H
