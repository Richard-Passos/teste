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

Screen screen = menu;
bool is_screen_loaded = false;

Asset screen_assets[MAX_SCREEN_ASSETS];
int screen_assets_size = 0;

int handle_screens() {
    bool found_screen = 1;

    if (screen == menu)
        handle_menu_screen();
    else if (screen == help)
        handle_help_screen();
    else if (screen == paused)
        handle_paused_screen();
    else if (screen == inventory)
        handle_inventory_screen();
    else {
        found_screen = 0;
    }

    return found_screen;
}

void handle_menu_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!is_screen_loaded) {
        add_asset(
            "../assets/menu_background.png",
            (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
        );
        add_asset(
            "../assets/menu_logo.png",
            (Rectangle){SCREEN_WIDTH / 2.0 - 319.5, 25, 639, 241}
        );
        add_asset("../assets/wall.png", (Rectangle){300, 250, 32, 32});
        add_asset("../assets/wall.png", (Rectangle){300, 300, 32, 32});
        add_asset("../assets/wall.png", (Rectangle){300, 350, 32, 32});
        add_asset("../assets/wall.png", (Rectangle){300, 400, 32, 32});

        is_screen_loaded = true;
    }

    //----------------------------------------------------------------------------------

    // Init
    //----------------------------------------------------------------------------------
    Asset background = screen_assets[0],
            logo = screen_assets[1],
            play_button = screen_assets[2],
            load_button = screen_assets[3],
            help_button = screen_assets[4],
            exit_button = screen_assets[5];
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
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
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if
    (is_button_pressed(play_button)) {
        save_game_state();
        set_screen(start);
    } else if
    (is_button_pressed(load_button)) {
        load_game_state();
        set_screen(start);
    } else if
    (is_button_pressed(help_button)) {
        set_screen(help);
    } else if
    (is_button_pressed(exit_button)) {
        CloseWindow();
    }

    //----------------------------------------------------------------------------------
}

void handle_paused_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!is_screen_loaded) {
        add_asset(("../assets/wall.png"), (Rectangle){300, 150, 32, 32});
        add_asset("../assets/wall.png", (Rectangle){300, 200, 32, 32});
        add_asset("../assets/wall.png", (Rectangle){300, 250, 32, 32});
        add_asset("../assets/wall.png", (Rectangle){300, 300, 32, 32});

        is_screen_loaded = true;
    }
    //----------------------------------------------------------------------------------

    // Init
    //----------------------------------------------------------------------------------
    Asset continue_button = screen_assets[0],
            inventory_button = screen_assets[1],
            save_button = screen_assets[2],
            back_menu_button = screen_assets[3];
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(GREEN);

    DrawText("PAUSED", 16, 16, 32, WHITE);

    draw_asset(continue_button);
    draw_asset(inventory_button);
    draw_asset(save_button);
    draw_asset(back_menu_button);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (is_button_pressed(continue_button) || IsKeyPressed(KEY_ESCAPE)) {
        set_screen(start);
    } else if (is_button_pressed(inventory_button)) {
        set_screen(inventory);
    } else if (is_button_pressed(save_button)) {
        save_game_state();
    } else if (is_button_pressed(back_menu_button)) {
        save_game_state();
        set_screen(menu);
    }
    //----------------------------------------------------------------------------------
}

void handle_inventory_screen() {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(RED);

    DrawText("INVENTORY", 16, 16, 32, WHITE);

    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE)) {
        set_screen(start);
    }
    //----------------------------------------------------------------------------------
}

void handle_help_screen() {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(PINK);

    DrawText("HELP", 16, 16, 32, WHITE);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE)) {
        set_screen(menu);
    }
    //----------------------------------------------------------------------------------
}

void handle_win_screen() {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(PINK);

    DrawText("WIN", 16, 16, 32, WHITE);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE)) {
        set_screen(menu);
    }
    //----------------------------------------------------------------------------------
}

void handle_game_over_screen() {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("GAME OVER", 16, 16, 32, WHITE);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE)) {
        set_screen(menu);
    }
    //----------------------------------------------------------------------------------
}

void set_screen(Screen s) {
    for (int i = 0; i < screen_assets_size; i++)
        UnloadTexture(screen_assets[i].texture);

    screen_assets_size = 0;
    is_screen_loaded = false;
    screen = s;
}

bool is_button_pressed(Asset button) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), button.hitbox);
}

void draw_asset(Asset button) {
    DrawTexture(button.texture, button.hitbox.x, button.hitbox.y, WHITE);
}

Asset add_asset(char texture_path[], Rectangle hitbox) {
    screen_assets[screen_assets_size] = (Asset){LoadTexture(texture_path), hitbox};

    return screen_assets[screen_assets_size++];
}
