//
// Created by richa on 05/11/2025.
//
#include <stdio.h>
#include <ctype.h>
#include "raylib.h"
#include "screen.h"
#include "game_state.h"
#include "config.h"

#define MAX_SCREEN_ASSETS 6

char screen = 'm';
bool is_screen_loaded = false;

Asset screen_assets[MAX_SCREEN_ASSETS];
int screen_assets_size = 0;

int handle_screens() {
    bool found_screen = 1;

    if (screen == 'm')
        handle_menu_screen();
    else if (screen == 'h')
        handle_help_screen();
    else if (screen == 'p')
        handle_paused_screen();
    else if (screen == 'i')
        handle_inventory_screen();
    else {
        found_screen = 0;
    }

    return found_screen;
}

void handle_menu_screen() {
    if (!is_screen_loaded) {
        screen_assets[0] = (Asset){
            LoadTexture("../assets/menu_background.png"), {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
        };
        screen_assets[1] = (Asset){LoadTexture("../assets/menu_logo.png"), {SCREEN_WIDTH / 2 - 319.5, 25, 639, 241}};
        screen_assets[2] = (Asset){LoadTexture("../assets/wall.png"), {300, 250, 32, 32}};
        screen_assets[3] = (Asset){LoadTexture("../assets/wall.png"), {300, 300, 32, 32}};
        screen_assets[4] = (Asset){LoadTexture("../assets/wall.png"), {300, 350, 32, 32}};
        screen_assets[5] = (Asset){LoadTexture("../assets/wall.png"), {300, 400, 32, 32}};

        screen_assets_size = 6;
        is_screen_loaded = true;

        printf("Level: %d", game_state.level);
    }

    Asset background = screen_assets[0];
    Asset logo = screen_assets[1];
    Asset play_button = screen_assets[2];
    Asset load_button = screen_assets[3];
    Asset help_button = screen_assets[4];
    Asset exit_button = screen_assets[5];

    BeginDrawing();
    ClearBackground(BLUE);

    DrawText("MENU", 16, 16, 32, WHITE);

    draw_asset(background);
    draw_asset(logo);
    draw_asset(play_button);
    draw_asset(load_button);
    draw_asset(help_button);
    draw_asset(exit_button);
    EndDrawing();

    if (is_button_pressed(play_button)) {
        save_game_state();
        set_screen('s');
    } else if (is_button_pressed(load_button)) {
        load_game_state();
        set_screen('s');
    } else if (is_button_pressed(help_button)) {
        set_screen('h');
    } else if (is_button_pressed(exit_button)) {
        CloseWindow();
    }
}

void handle_paused_screen() {
    if (!is_screen_loaded) {
        screen_assets[0] = (Asset)
        {
            LoadTexture("../assets/wall.png"),
            {
                300, 150, 32, 32
            }
        };
        screen_assets[1] = (Asset)
        {
            LoadTexture("../assets/wall.png"),
            {
                300, 200, 32, 32
            }
        };
        screen_assets[2] = (Asset)
        {
            LoadTexture("../assets/wall.png"),
            {
                300, 250, 32, 32
            }
        };
        screen_assets[3] = (Asset)
        {
            LoadTexture("../assets/wall.png"),
            {
                300, 300, 32, 32
            }
        };

        screen_assets_size = 4;
        is_screen_loaded = true;
    }

    Asset continue_button = screen_assets[0];
    Asset inventory_button = screen_assets[1];
    Asset save_button = screen_assets[2];
    Asset back_menu_button = screen_assets[3];

    BeginDrawing();
    ClearBackground(GREEN);

    DrawText("PAUSED", 16, 16, 32, WHITE);

    draw_asset(continue_button);
    draw_asset(inventory_button);
    draw_asset(save_button);
    draw_asset(back_menu_button);
    EndDrawing();

    if (is_button_pressed(continue_button) || IsKeyPressed(KEY_ESCAPE)) {
        set_screen('s');
    } else if (is_button_pressed(inventory_button)) {
        set_screen('i');
    } else if (is_button_pressed(save_button)) {
        game_state.level = game_state.level == 3 ? 2 : 3;
        save_game_state();
    } else if (is_button_pressed(back_menu_button)) {
        save_game_state();
        set_screen('m');
    }
}

void handle_inventory_screen() {
    BeginDrawing();
    ClearBackground(RED);

    DrawText("INVENTORY", 16, 16, 32, WHITE);

    EndDrawing();

    if (IsKeyPressed(KEY_ESCAPE)) {
        set_screen('s');
    }
}

void handle_help_screen() {
    BeginDrawing();
    ClearBackground(PINK);

    DrawText("HELP", 16, 16, 32, WHITE);
    EndDrawing();

    if (IsKeyPressed(KEY_ESCAPE)) {
        set_screen('m');
    }
}

void set_screen(char s) {
    for (int i = 0; i < screen_assets_size; i++)
        UnloadTexture(screen_assets[i].texture);

    screen_assets_size = 0;
    is_screen_loaded = false;
    screen = (char) tolower(s);
}

bool is_button_pressed(Asset button) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), button.hitbox);
}

void draw_asset(Asset button) {
    DrawTexture(button.texture, button.hitbox.x, button.hitbox.y, WHITE);
}
