//
// Created by richa on 05/11/2025.
//
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "screen.h"
#include "game_state.h"
#include "config.h"
#include "camera.h"
#include "bench.h"
#include "map.h"
#include "shop.h"
#include "teleport.h"

#define MAX_SCREEN_ASSETS 2
#define MAX_SCREEN_ACTIONS 4

Screen screen = {menu, false};
Screen_name last_screen = -1;

Asset screen_assets[MAX_SCREEN_ASSETS];
int screen_assets_size = 0;

Action screen_actions[MAX_SCREEN_ACTIONS];
int screen_actions_size = 0, action_active_index = -1;

int handle_screens() {
    bool found_screen = 1;

    // Screens
    //----------------------------------------------------------------------------------
    if (screen.name == menu)
        handle_menu_screen();
    else if (screen.name == help)
        handle_help_screen();
    else if (screen.name == paused)
        handle_paused_screen();
    else if (screen.name == inventory)
        handle_inventory_screen();
    else if (screen.name == win)
        handle_win_screen();
    else if (screen.name == game_over)
        handle_game_over_screen();
    else if (screen.name == village)
        handle_village_screen();
    else if (screen.name == shop)
        handle_shop_screen();
    else
        found_screen = 0;
    //----------------------------------------------------------------------------------

    // Actions active state
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_UP))
        action_active_index = action_active_index <= 0 ? screen_actions_size - 1 : action_active_index - 1;
    else if (IsKeyPressed(KEY_DOWN))
        action_active_index = action_active_index >= screen_actions_size - 1 ? 0 : action_active_index + 1;
    //----------------------------------------------------------------------------------

    return found_screen;
}

void handle_menu_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        unload_map();

        add_asset(
            "../assets/menu_background.png",
            (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
        );
        add_asset(
            "../assets/menu_logo.png",
            (Rectangle){center_on_screen(639), 25, 639, 241}
        );

        add_action("Jogar", (Rectangle){center_on_screen(600), 250, 600, 60});
        add_action("Carregar Jogo", (Rectangle){center_on_screen(600), 320, 600, 60});
        add_action("Ajuda", (Rectangle){center_on_screen(600), 390, 600, 60});
        add_action("Sair", (Rectangle){center_on_screen(600), 460, 600, 60});


        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    // Init
    //----------------------------------------------------------------------------------
    Asset background = screen_assets[0],
            logo = screen_assets[1];

    Action play_action = screen_actions[0],
            load_action = screen_actions[1],
            help_action = screen_actions[2],
            exit_action = screen_actions[3];
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    draw_asset(&background);
    draw_asset(&logo);

    draw_action(&play_action);
    draw_action(&load_action);
    draw_action(&help_action);
    draw_action(&exit_action);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(&play_action)) {
        reset_game_state();
        save_game_state();
        set_screen(village);
    } else if (is_action_pressed(&load_action)) {
        load_game_state();
        set_screen(village);
    } else if (is_action_pressed(&help_action)) {
        set_screen(help);
    } else if (is_action_pressed(&exit_action)) {
        CloseWindow();
    }
    //----------------------------------------------------------------------------------
}

void handle_paused_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        add_action("Continuar Jogo", (Rectangle){center_on_screen(600), 150, 600, 60});
        add_action("Inventario", (Rectangle){center_on_screen(600), 220, 600, 60});
        add_action("Salvar Jogo", (Rectangle){center_on_screen(600), 290, 600, 60});
        add_action("Voltar ao Menu", (Rectangle){center_on_screen(600), 360, 600, 60});

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    // Init
    //----------------------------------------------------------------------------------
    Action continue_action = screen_actions[0],
            inventory_action = screen_actions[1],
            save_action = screen_actions[2],
            back_menu_action = screen_actions[3];
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(GREEN);

    DrawText("PAUSED", 16, 16, 32, WHITE);

    draw_action(&continue_action);
    draw_action(&inventory_action);
    draw_action(&save_action);
    draw_action(&back_menu_action);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(&continue_action) || IsKeyPressed(KEY_ESCAPE)) {
        set_screen(last_screen);
    } else if (is_action_pressed(&inventory_action)) {
        set_screen(inventory);
    } else if (is_action_pressed(&save_action)) {
        save_game_state();
    } else if (is_action_pressed(&back_menu_action)) {
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
        set_screen(last_screen);
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
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        add_action("Recomecar Jogo", (Rectangle){center_on_screen(600), 150, 600, 60});
        add_action("Carregar Jogo", (Rectangle){center_on_screen(600), 220, 600, 60});
        add_action("Voltar ao Menu", (Rectangle){center_on_screen(600), 290, 600, 60});

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    // Init
    //----------------------------------------------------------------------------------
    Action restart_action = screen_actions[0],
            load_action = screen_actions[1],
            back_menu_action = screen_actions[2];
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(YELLOW);

    DrawText("WIN", 16, 16, 32, WHITE);

    draw_action(&restart_action);
    draw_action(&load_action);
    draw_action(&back_menu_action);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(&restart_action) || IsKeyPressed(KEY_ESCAPE)) {
        reset_game_state();
        save_game_state();
        set_screen(village);
    } else if (is_action_pressed(&load_action)) {
        load_game_state();
        set_screen(village);
    } else if (is_action_pressed(&back_menu_action)) {
        set_screen(menu);
    }
    //----------------------------------------------------------------------------------
}

void handle_game_over_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        add_action("Recomecar Jogo", (Rectangle){center_on_screen(600), 150, 600, 60});
        add_action("Voltar ao Menu", (Rectangle){center_on_screen(600), 220, 600, 60});

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    // Init
    //----------------------------------------------------------------------------------
    Action restart_action = screen_actions[0],
            back_menu_action = screen_actions[1];
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("GAME OVER", 16, 16, 32, WHITE);

    draw_action(&restart_action);
    draw_action(&back_menu_action);
    EndDrawing();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(&restart_action) || IsKeyPressed(KEY_ESCAPE)) {
        reset_game_state();
        save_game_state();
        set_screen(village);
    } else if (is_action_pressed(&back_menu_action)) {
        reset_game_state();
        save_game_state();
        set_screen(menu);
    }
    //----------------------------------------------------------------------------------
}

void handle_village_screen() {
    Player *player = &game_state.player;

    // Load
    //--------------------------------------------------------------------------------------
    if (!load_map(VILLAGE_FILE_PATH)) {
        CloseWindow();
    }
    //--------------------------------------------------------------------------------------

    // Update
    //----------------------------------------------------------------------------------
    float delta_time = GetFrameTime();

    update_player(delta_time);
    update_camera_center((Vector2){player->hitbox.x, player->hitbox.y});
    update_monsters(delta_time);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    draw_map();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    handle_benches_interaction();

    if (handle_shop_interaction())
        set_screen(shop);

    if (IsKeyPressed(KEY_ESCAPE))
        set_screen(paused);
    else if (IsKeyPressed(KEY_TAB))
        set_screen(inventory);
    else if (handle_teleports_interaction())
        set_screen(start);
    //----------------------------------------------------------------------------------
}

void handle_shop_screen() {
    Player *player = &game_state.player;

    // Load
    //--------------------------------------------------------------------------------------
    if (!load_map(SHOP_FILE_PATH)) {
        CloseWindow();
    }
    //--------------------------------------------------------------------------------------

    // Update
    //----------------------------------------------------------------------------------
    float delta_time = GetFrameTime();

    update_player(delta_time);
    update_camera_center((Vector2){player->hitbox.x, player->hitbox.y});
    update_monsters(delta_time);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    draw_map();
    //----------------------------------------------------------------------------------

    // Actions
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE))
        set_screen(paused);
    else if (IsKeyPressed(KEY_TAB))
        set_screen(inventory);
    else if (handle_teleports_interaction()) {
        player->hitbox = (Rectangle){
            SHOP.hitbox.x + SHOP.hitbox.width / 2 - player->hitbox.width / 2, SHOP.hitbox.y + SHOP.hitbox.height,
            player->hitbox.width,
            player->hitbox.height
        };
        player->should_keep_pos = true;
        set_screen(last_screen);
    }
    //----------------------------------------------------------------------------------
}

void set_screen(Screen_name name) {
    for (int i = 0; i < screen_assets_size; i++)
        UnloadTexture(screen_assets[i].texture);

    if (screen.name == village || screen.name == start || screen.name == shop)
        last_screen = screen.name;

    screen_assets_size = 0;
    screen_actions_size = 0;
    action_active_index = -1;
    screen.is_loaded = false;
    screen.name = name;
}

void draw_asset(Asset *asset) {
    DrawTexture(asset->texture, asset->hitbox.x, asset->hitbox.y, WHITE);
}

void add_asset(char texture_path[], Rectangle hitbox) {
    screen_assets[screen_assets_size++] = (Asset){LoadTexture(texture_path), hitbox};
}

void draw_action(Action *action) {
    bool is_hovered = CheckCollisionPointRec(GetMousePosition(), action->hitbox),
            is_active = action_active_index == action->index;

    // Rectangle
    //--------------------------------------------------------------------------------------
    DrawRectangleRec(action->hitbox, is_hovered ? RED : BLUE);
    DrawRectangleLines(action->hitbox.x, action->hitbox.y, action->hitbox.width, action->hitbox.height, BLACK);
    //--------------------------------------------------------------------------------------

    // Text
    //--------------------------------------------------------------------------------------
    int font_size = 32;
    int text_x = action->hitbox.x + action->hitbox.width * .1;
    int text_y = action->hitbox.y + (action->hitbox.height - font_size) / 2;

    DrawText(action->label, text_x, text_y, font_size, WHITE);

    if (is_active) {
        DrawText(">", action->hitbox.x - font_size, text_y, font_size, WHITE);
        DrawText("<", action->hitbox.x + action->hitbox.width + font_size / 2, text_y, font_size, WHITE);
    }
    //--------------------------------------------------------------------------------------
}

void add_action(char label[], Rectangle hitbox) {
    screen_actions[screen_actions_size] = (Action){label, hitbox, screen_actions_size++};
}

bool is_action_pressed(Action *action) {
    bool is_pressed = (IsKeyPressed(KEY_SPACE) && action_active_index == action->index) || (
                          IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(
                              GetMousePosition(), action->hitbox));
    return is_pressed;
}

float center_on_screen(float width) {
    return SCREEN_WIDTH / 2.0 - width / 2;
}


