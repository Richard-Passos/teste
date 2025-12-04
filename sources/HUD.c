//
// Created by Harry on 10/11/2025.
//

#include <math.h>

#include "game_state.h"
#include "raylib.h"
#include "stdio.h"
#include "player.h"

char level_text[10];

void draw_hud() {
    Player *player = &game_state.player;

    draw_ability_popup();
    draw_item_popup();

    // ==============================
    // Life Bar
    // ==============================
    int life_size = 30, life_spacing = 5, life_x = 16, life_y = 16;

    for (int i = 0; i < player->combat.max_life; i++) {
        int x = life_x + i * (life_size + life_spacing);
        Color color = (i < player->combat.life) ? WHITE : (Color){100, 100, 100, 255};
        DrawRectangle(x, life_y, life_size, life_size, color);
        DrawRectangleLines(x, life_y, life_size, life_size, BLACK);
    }

    // ==============================
    // Souls Bar
    // ==============================
    float max_souls = player->combat.max_souls;
    float souls = player->combat.souls;

    float souls_width = 200, souls_fill_width = (souls / max_souls) * souls_width,
            souls_x = 16, souls_y = life_size + life_y + 10;
    Color souls_fill_color = souls < 33 ? GRAY : WHITE;

    DrawRectangle(souls_x, souls_y, 200, 20, (Color){80, 80, 80, 255});
    DrawRectangle(souls_x, souls_y, souls_fill_width, 20, souls_fill_color);
    DrawRectangleLines(souls_x, souls_y, souls_width, 20, BLACK);

    // ==============================
    // Level
    // ==============================
    sprintf(level_text, "%02d/%02d", game_state.level, MAX_LEVEL);
    DrawText(level_text, SCREEN_WIDTH - 106, 16, 32, BLACK);

    // ==============================
    // Money
    // ==============================
    char money_text[64];
    sprintf(money_text, "Moedas: %d", player->money);
    DrawText(money_text, 16, SCREEN_HEIGHT - 40, 30, BLACK);

    if (player->money_gain_timer > 0.0f) {
        player->money_gain_timer -= DELTA_TIME;

        // Fade effect
        float alpha = player->money_gain_timer / 1.5f;
        int transparency = (int) (255 * alpha);

        char gain_text[32];
        sprintf(gain_text, "%c%d", player->last_money_gain >= 0 ? '+' : '-', abs(player->last_money_gain));
        DrawText(gain_text, 146, SCREEN_HEIGHT - 66, 20, (Color){0, 0, 0, transparency});
    }
}
