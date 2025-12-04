//
// Created by richa on 05/11/2025.
//

#ifndef HOLLOW_SCREEN_H
#define HOLLOW_SCREEN_H
#include "raylib.h"

#define MAX_LABEL_SIZE 51

typedef enum { AXIS_X, AXIS_Y } Axis;

typedef struct {
    Texture texture;
    Rectangle hitbox;
} Asset;

typedef struct {
    int index;
    char id[50];
    char label[150];
    char texture_path[150];
    Rectangle hitbox;
    Texture2D texture;
} Action;

typedef enum {
    SCREEN_MENU,
    SCREEN_PAUSED,
    SCREEN_CONFIRM_MENU,
    SCREEN_INVENTORY,
    SCREEN_START,
    SCREEN_WIN,
    SCREEN_GAME_OVER,
    SCREEN_HELP,
    SCREEN_VILLAGE,
    SCREEN_SHOP,
    SCREEN_SHOP_NPC
} Screen_name;

typedef struct {
    Screen_name name;
    bool is_loaded;
} Screen;

int handle_screens();

void handle_menu_screen();

void handle_paused_screen();

void handle_confirm_menu_screen();

void handle_inventory_screen();

void handle_help_screen();

void handle_win_screen();

void handle_game_over_screen();

void handle_village_screen();

void handle_shop_screen();

void handle_shop_npc_screen();

void set_screen(Screen_name);

void add_asset(char [], Rectangle);

void draw_assets();

void add_action(char [], Rectangle, char []);

void draw_actions();

void draw_action(Action, bool);

bool is_action_pressed(Action);

float center_on_screen(float, Axis);

void draw_popover(char [], char [], Vector2);

extern Screen screen;
extern Screen_name last_screen;
#endif //HOLLOW_SCREEN_H
