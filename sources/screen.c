//
// Created by richa on 05/11/2025.
//
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "screen.h"
#include "game_state.h"
#include "item.h"
#include "map.h"
#include "shop.h"
#include "teleport.h"

#define MAX_SCREEN_ASSETS 99
#define MAX_SCREEN_ACTIONS 99

Screen screen = {SCREEN_MENU, false};
Screen_name last_screen = -1;

Asset screen_assets[MAX_SCREEN_ASSETS];
int screen_assets_count = 0;

Action screen_actions[MAX_SCREEN_ACTIONS];
int screen_actions_count = 0, action_active_index = -1;

int handle_screens() {
    bool found_screen = 1;

    // Screens
    //----------------------------------------------------------------------------------
    if (screen.name == SCREEN_MENU)
        handle_menu_screen();
    else if (screen.name == SCREEN_HELP)
        handle_help_screen();
    else if (screen.name == SCREEN_PAUSED)
        handle_paused_screen();
    else if (screen.name == SCREEN_CONFIRM_MENU)
        handle_confirm_menu_screen();
    else if (screen.name == SCREEN_INVENTORY)
        handle_inventory_screen();
    else if (screen.name == SCREEN_WIN)
        handle_win_screen();
    else if (screen.name == SCREEN_GAME_OVER)
        handle_game_over_screen();
    else if (screen.name == SCREEN_VILLAGE)
        handle_village_screen();
    else if (screen.name == SCREEN_SHOP)
        handle_shop_screen();
    else if (screen.name == SCREEN_SHOP_NPC)
        handle_shop_npc_screen();
    else
        found_screen = 0;
    //----------------------------------------------------------------------------------

    // Actions active state
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_UP))
        action_active_index = action_active_index <= 0 ? screen_actions_count - 1 : action_active_index - 1;
    else if (IsKeyPressed(KEY_DOWN))
        action_active_index = action_active_index >= screen_actions_count - 1 ? 0 : action_active_index + 1;
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
            (Rectangle){center_on_screen(639, AXIS_X), 25, 639, 241}
        );

        float center_x = center_on_screen(600, AXIS_X);

        add_action("Jogar", (Rectangle){center_x, 250, 600, 60}, "");
        add_action("Carregar Jogo", (Rectangle){center_x, 320, 600, 60}, "");
        add_action("Ajuda", (Rectangle){center_x, 390, 600, 60}, "");
        add_action("Sair", (Rectangle){center_x, 460, 600, 60}, "");


        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action play_action = screen_actions[0],
            load_action = screen_actions[1],
            help_action = screen_actions[2],
            exit_action = screen_actions[3];

    BeginDrawing();
    draw_assets();
    draw_actions();
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(play_action)) {
        reset_game_state();
        save_game_state();
        set_screen(SCREEN_VILLAGE);
    } else if (is_action_pressed(load_action)) {
        load_game_state();
        set_screen(SCREEN_VILLAGE);
    } else if (is_action_pressed(help_action)) {
        set_screen(SCREEN_HELP);
    } else if (is_action_pressed(exit_action)) {
        CloseWindow();
    }
    //----------------------------------------------------------------------------------
}

void handle_paused_screen() {
    float center_x = center_on_screen(600, AXIS_X);

    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        add_action("Continuar Jogo", (Rectangle){center_x, 150, 600, 60}, "");
        add_action("Inventario", (Rectangle){center_x, 220, 600, 60}, "");
        add_action("Salvar Jogo", (Rectangle){center_x, 290, 600, 60}, "");
        add_action("Voltar ao Menu", (Rectangle){center_x, 360, 600, 60}, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action continue_action = screen_actions[0],
            inventory_action = screen_actions[1],
            save_action = screen_actions[2],
            back_menu_action = screen_actions[3];

    BeginDrawing();
    ClearBackground(GREEN);
    draw_actions();

    DrawText("PAUSED", 16, 16, 32, WHITE);
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(continue_action) || IsKeyPressed(KEY_ESCAPE)) {
        set_screen(last_screen);
    } else if (is_action_pressed(inventory_action)) {
        set_screen(SCREEN_INVENTORY);
    } else if (is_action_pressed(save_action)) {
        save_game_state();
    } else if (is_action_pressed(back_menu_action)) {
        set_screen(SCREEN_CONFIRM_MENU);
    }
    //----------------------------------------------------------------------------------
}

void handle_confirm_menu_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        float center_x = center_on_screen(600, AXIS_X);

        add_action("Sim, Voltar ao Menu", (Rectangle){center_x, 150, 600, 60}, "");
        add_action("Cancelar", (Rectangle){center_x, 220, 600, 60}, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action confirm_action = screen_actions[0],
            cancel_action = screen_actions[1];

    BeginDrawing();
    ClearBackground(GREEN);
    draw_actions();

    DrawText("TEM CERTEZA QUE DESEJA VOLTAR AO MENU?", 16, 16, 32, WHITE);
    DrawText("O jogo será salvo automaticamente, mas você retornará no último banco descansado.", 16, 56, 24, WHITE);
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(confirm_action)) {
        save_game_state();
        set_screen(SCREEN_MENU);
    } else if (is_action_pressed(cancel_action) || IsKeyPressed(KEY_ESCAPE)) {
        set_screen(SCREEN_PAUSED);
    }
    //----------------------------------------------------------------------------------
}


void handle_inventory_screen() {
    int active_items_count = 0;

    Item *items = game_state.items;
    int *items_count = &game_state.items_count;

    for (int i = 0; i < screen_actions_count; i++)
        if (is_item_active(&items[i])) active_items_count++;

    Vector2 mouse = GetMousePosition();

    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        int acquired_items_count = 0;
        for (int i = 0; i < *items_count; i++)
            if (items[i].is_acquired)
                add_action(
                    items[i].label,
                    (Rectangle){32 + acquired_items_count++ * 74, 350, 64, 64},
                    items[i].texture_path
                );

        add_action("<- Voltar", (Rectangle){SCREEN_WIDTH - 316, 16, 300, 60}, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action go_back = screen_actions[screen_actions_count - 1];

    BeginDrawing();
    ClearBackground(RED);
    DrawText("Amuletos ativos:", 32, 110, 24, WHITE);
    for (int i = 0; i < MAX_ITEMS_ACTIVATED; i++)
        DrawRectangle(32 + i * 74, 150, 64, 64, (Color){0, 0, 0, 100});

    DrawText("Amuletos adiquiridos:", 32, 310, 24, WHITE);
    for (int i = 0; i < *items_count; i++)
        DrawRectangle(32 + i * 74, 350, 64, 64, (Color){0, 0, 0, 100});

    // Charms
    int active_items_iterator = 0;
    for (int i = *items_count - 1; i >= 0; i--) {
        Item *item = &items[i];

        if (!item->is_acquired) continue;

        for (int j = 0; j < screen_actions_count; j++) {
            Action action = screen_actions[j];

            if (strcmp(item->texture_path, action.texture_path) != 0) continue;

            if (item->is_active) {
                // Active charms
                Action active_action = action;
                active_action.hitbox.x = 32 + (MAX_ITEMS_ACTIVATED - 1 - active_items_iterator++) * 74;
                active_action.hitbox.y = 150;

                draw_action(active_action, true);
                if (CheckCollisionPointRec(mouse, active_action.hitbox))
                    draw_popover(item->label, item->description, mouse);
                if (game_state.player.is_sitting && is_action_pressed(active_action)) {
                    // Toggle item
                    item->is_active = false;
                    set_screen(SCREEN_INVENTORY); // Reset screen to reload actions
                }
            }

            // Acquired charms
            draw_action(action, true);
            if (CheckCollisionPointRec(mouse, action.hitbox))
                draw_popover(item->label, item->description, mouse);
            if (game_state.player.is_sitting && is_action_pressed(action)) {
                // Toggle item
                item->is_active = active_items_count >= MAX_ITEMS_ACTIVATED ? false : !item->is_active;
                set_screen(SCREEN_INVENTORY); // Reset screen to reload actions
            }
        }
    }

    // Go back action
    draw_action(screen_actions[screen_actions_count - 1], false);

    DrawText("INVENTORY", 16, 16, 32, WHITE);
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(go_back) || IsKeyPressed(KEY_ESCAPE))
        set_screen(last_screen);
    //----------------------------------------------------------------------------------
}

void handle_help_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        add_asset(
            "../assets/help.png",
            (Rectangle){center_on_screen(960, AXIS_X), center_on_screen(540, AXIS_Y), 960, 540}
        );

        add_action("<- Voltar", (Rectangle){SCREEN_WIDTH - 316, 16, 300, 60}, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action go_back = screen_actions[0];

    BeginDrawing();
    ClearBackground(BLACK);
    draw_assets();
    draw_actions();

    DrawText("HELP", 16, 16, 32, WHITE);
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(go_back) || IsKeyPressed(KEY_ESCAPE)) {
        set_screen(SCREEN_MENU);
    }
    //----------------------------------------------------------------------------------
}

void handle_win_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        float center_x = center_on_screen(600, AXIS_X);

        add_action("Recomecar Jogo", (Rectangle){center_x, 150, 600, 60}, "");
        add_action("Carregar Jogo", (Rectangle){center_x, 220, 600, 60}, "");
        add_action("Voltar ao Menu", (Rectangle){center_x, 290, 600, 60}, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action restart_action = screen_actions[0],
            load_action = screen_actions[1],
            back_menu_action = screen_actions[2];

    BeginDrawing();
    ClearBackground(YELLOW);
    draw_actions();

    DrawText("WIN", 16, 16, 32, WHITE);
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(restart_action) || IsKeyPressed(KEY_ESCAPE)) {
        reset_game_state();
        save_game_state();
        set_screen(SCREEN_VILLAGE);
    } else if (is_action_pressed(load_action)) {
        load_game_state();
        set_screen(SCREEN_VILLAGE);
    } else if (is_action_pressed(back_menu_action)) {
        set_screen(SCREEN_MENU);
    }
    //----------------------------------------------------------------------------------
}

void handle_game_over_screen() {
    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        float center_x = center_on_screen(600, AXIS_X);

        add_action("Tentar de Novo", (Rectangle){center_x, 150, 600, 60}, "");
        add_action("Voltar ao Menu", (Rectangle){center_x, 220, 600, 60}, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action restart_action = screen_actions[0],
            back_menu_action = screen_actions[1];

    BeginDrawing();
    ClearBackground(BLACK);
    draw_actions();

    DrawText("GAME OVER", 16, 16, 32, WHITE);
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(restart_action) || IsKeyPressed(KEY_ESCAPE)) {
        game_state.player.combat.life = game_state.player.combat.max_life;
        game_state.player.combat.souls = game_state.player.combat.max_souls;
        game_state.player.speed.x = 0;
        game_state.player.speed.y = 0;
        game_state.player.combat.hurt_timer = 0;
        game_state.player.combat.invuln_timer = 0;
        spawn_player_on_bench();

        set_screen(SCREEN_VILLAGE);
    } else if (is_action_pressed(back_menu_action)) {
        reset_game_state();
        save_game_state();
        set_screen(SCREEN_MENU);
    }
    //----------------------------------------------------------------------------------
}

void handle_village_screen() {
    if (!load_map(VILLAGE_FILE_PATH)) CloseWindow();

    handle_map();

    // Actions
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ESCAPE))
        set_screen(SCREEN_PAUSED);
    else if (IsKeyPressed(KEY_TAB))
        set_screen(SCREEN_INVENTORY);
    else if (handle_teleports_interaction()) {
        set_map_path(game_state.level);
        set_screen(SCREEN_START);
    }
    //----------------------------------------------------------------------------------
}

void handle_shop_screen() {
    if (!load_map(SHOP_FILE_PATH)) CloseWindow();

    handle_map();

    // Actions
    //----------------------------------------------------------------------------------
    Player *player = &game_state.player;

    if (IsKeyPressed(KEY_ESCAPE))
        set_screen(SCREEN_PAUSED);
    else if (IsKeyPressed(KEY_TAB))
        set_screen(SCREEN_INVENTORY);
    else if (handle_teleports_interaction()) {
        player->hitbox = (Rectangle){
            shop.hitbox.x + shop.hitbox.width / 2 - player->hitbox.width / 2,
            shop.hitbox.y + shop.hitbox.height - player->hitbox.height - 2,
            player->hitbox.width,
            player->hitbox.height
        };
        player->should_keep_pos = true;

        set_screen(SCREEN_VILLAGE);
    } else if (handle_shop_npc_interaction())
        set_screen(SCREEN_SHOP_NPC);
    //----------------------------------------------------------------------------------
}

void handle_shop_npc_screen() {
    Player *player = &game_state.player;
    Item *items = game_state.items;
    int *items_count = &game_state.items_count;
    Vector2 mouse = GetMousePosition();

    // Load
    //----------------------------------------------------------------------------------
    if (!screen.is_loaded) {
        add_asset(
            "../assets/menu_background.png",
            (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}
        );

        float center_x = center_on_screen(600, AXIS_X);

        int buyable_items_count = 0;
        for (int i = 0; i < *items_count; i++)
            if (items[i].is_buyable && !items[i].is_acquired) {
                char label[150];
                sprintf(label, "Valor: %-3d | %s", items[i].cost, items[i].label);

                add_action(
                    label,
                    (Rectangle){
                        center_x, 150 + (buyable_items_count * 70), 600, 60
                    },
                    items[i].texture_path
                );

                buyable_items_count++;
            }

        add_action(
            "<- Voltar",
            (Rectangle){
                center_x, 150 + (buyable_items_count * 80), 600, 60
            }, "");

        screen.is_loaded = true;
    }
    //----------------------------------------------------------------------------------

    Action go_back = screen_actions[screen_actions_count - 1];

    BeginDrawing();
    draw_assets();
    for (int i = screen_actions_count - 1; i >= 0; i--) {
        Action action = screen_actions[i];
        Item *item = NULL;

        draw_action(action, false);

        // Get item
        for (int j = 0; j < *items_count; j++)
            if (items[j].is_buyable && !items[j].is_acquired) {
                if (strcmp(items[j].texture_path, action.texture_path) == 0) {
                    item = &items[j];
                    break;
                }
            }

        if (item == NULL) continue;

        // Buy item
        if (is_action_pressed(action) && player->money >= item->cost) {
            add_player_money(-item->cost);
            item->is_acquired = true;
            set_screen(SCREEN_SHOP_NPC); // Reset screen to reload actions
        }
        if (CheckCollisionPointRec(mouse, action.hitbox))
            draw_popover(item->label, item->description, mouse);
    }

    DrawText("SHOP OWNER", 16, 16, 32, WHITE);

    // Money
    //----------------------------------------------------------------------------------
    char money_text[64];
    sprintf(money_text, "Moedas: %d", player->money);
    DrawText(money_text, 10, 750, 30, WHITE);

    if (player->money_gain_timer > 0.0f) {
        player->money_gain_timer -= GetFrameTime();

        // Fade effect
        //----------------------------------------------------------------------------------
        float alpha = player->money_gain_timer / 1.5f;
        int transparency = (int) (255 * alpha);

        Color cost_color = (Color){255, 255, 255, transparency};

        char cost_text[32];
        sprintf(cost_text, "-%d", player->last_money_gain);
        DrawText(cost_text, 140, 720, 20, cost_color);
        //----------------------------------------------------------------------------------
    }
    //----------------------------------------------------------------------------------
    EndDrawing();

    // Actions
    //----------------------------------------------------------------------------------
    if (is_action_pressed(go_back) || IsKeyPressed(KEY_ESCAPE))
        set_screen(last_screen);
    //----------------------------------------------------------------------------------
}

void set_screen(Screen_name name) {
    for (int i = 0; i < screen_assets_count; i++)
        UnloadTexture(screen_assets[i].texture);

    for (int i = 0; i < screen_actions_count; i++)
        if (strcmp(screen_actions[i].texture_path, "") != 0)
            UnloadTexture(screen_actions[i].texture);
        else
            strcpy(screen_actions[i].texture_path, "");

    if (screen.name == SCREEN_VILLAGE || screen.name == SCREEN_START || screen.name == SCREEN_SHOP)
        last_screen = screen.name;

    screen_assets_count = 0;
    screen_actions_count = 0;
    action_active_index = -1;
    screen.is_loaded = false;
    screen.name = name;
}

void add_asset(char texture_path[], Rectangle hitbox) {
    screen_assets[screen_assets_count++] = (Asset){LoadTexture(texture_path), hitbox};
}

void draw_assets() {
    for (int i = 0; i < screen_assets_count; i++) {
        Asset *asset = &screen_assets[i];

        DrawTexturePro(
            asset->texture,
            (Rectangle){0, 0, asset->texture.width, asset->texture.height},
            asset->hitbox,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    }
}

void add_action(char label[], Rectangle hitbox, char texture_path[]) {
    Action *action = &screen_actions[screen_actions_count];

    strcpy(action->label, label);
    strcpy(action->texture_path, texture_path);
    action->hitbox = hitbox;
    action->index = screen_actions_count++;

    if (strcmp(texture_path, "") != 0)
        action->texture = LoadTexture(texture_path);
}

void draw_actions() {
    for (int i = 0; i < screen_actions_count; i++)
        draw_action(screen_actions[i], strcmp(screen_actions[i].texture_path, "") != 0);
}

void draw_action(Action action, bool draw_as_texture) {
    bool is_hovered = CheckCollisionPointRec(GetMousePosition(), action.hitbox),
            is_active = action_active_index == action.index;

    DrawRectangleRec(action.hitbox, is_hovered ? RED : BLUE);
    DrawRectangleLines(action.hitbox.x, action.hitbox.y, action.hitbox.width, action.hitbox.height, BLACK);

    int font_count = 32;
    int text_x = action.hitbox.x + action.hitbox.width * .1;
    int text_y = action.hitbox.y + (action.hitbox.height - font_count) / 2;

    if (!draw_as_texture) {
        DrawText(action.label, text_x, text_y, font_count, WHITE);
    } else {
        DrawTexturePro(
            action.texture,
            (Rectangle){0, 0, action.texture.width, action.texture.height},
            action.hitbox,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    }

    if (is_active) {
        DrawText(">", action.hitbox.x - font_count, text_y, font_count, WHITE);
        DrawText("<", action.hitbox.x + action.hitbox.width + font_count / 2, text_y, font_count, WHITE);
    }
}

bool is_action_pressed(Action action) {
    return (IsKeyPressed(KEY_X) && action_active_index == action.index)
           || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), action.hitbox));
}

float center_on_screen(float size, Axis axis) {
    return (axis == AXIS_X ? SCREEN_WIDTH : SCREEN_HEIGHT) / 2.0f - size / 2.0f;
}

void draw_popover(char label[], char description[], Vector2 mouse) {
    int width = 400, height = 60;

    DrawRectangle(mouse.x + 16, mouse.y + 16, width, height, (Color){0, 0, 0, 200});
    DrawRectangleLines(mouse.x + 16, mouse.y + 16, width, height, WHITE);

    DrawText(label, mouse.x + 26, mouse.y + 21, 20, YELLOW);
    DrawText(description, mouse.x + 26, mouse.y + 46, 18, WHITE);
}
