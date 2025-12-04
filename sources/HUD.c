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

    char moneyText[64];
    sprintf(moneyText, "Moedas: %d", player->money);
    DrawText(moneyText, 16, SCREEN_HEIGHT - 40, 30, BLACK);

    draw_ability_popup();

    // Desenha o ganho
    if (player->money_gain_timer > 0.0f) {
        player->money_gain_timer -= DELTA_TIME;

        // Fazer sumir depois de um tempo
        float alpha = player->money_gain_timer / 1.5f;
        int transparency = (int) (255 * alpha);

        Color gainColor = (Color){0, 0, 0, transparency};

        char gainText[32];
        sprintf(gainText, "%c%d", player->last_money_gain >= 0 ? '+' : '-', abs(player->last_money_gain));
        DrawText(gainText, 146, SCREEN_HEIGHT - 66, 20, gainColor);
    }


    // ==============================
    // Barra de Almas
    // ==============================
    float maxSouls = player->combat.max_souls;
    float souls = player->combat.souls;

    float barWidth = 200;

    // Calcula a proporção preenchida
    float fillWidth = (souls / maxSouls) * barWidth;

    // Cor de fundo da barra
    DrawRectangle(16, 50, 200, 20, (Color){80, 80, 80, 255});

    // Cor da barra conforme a quantidade
    Color fillColor;
    if (souls < 33) {
        fillColor = GRAY;
    } else {
        fillColor = WHITE;
    }

    // Desenha o preenchimento
    DrawRectangle(16, 50, fillWidth, 20, fillColor);

    // Moldura
    DrawRectangleLines(16, 50, barWidth, 20, BLACK);

    // ==============================
    // Barra de Vida
    // ==============================
    int squareSize = 30;
    int spacing = 5;
    int startX = 16;
    int startY = 16;

    for (int i = 0; i < player->combat.max_life; i++) {
        int x = startX + i * (squareSize + spacing);
        Color color = (i < player->combat.life) ? WHITE : (Color){100, 100, 100, 255};
        DrawRectangle(x, startY, squareSize, squareSize, color);
        DrawRectangleLines(x, startY, squareSize, squareSize, BLACK);
    }

    // ==============================
    // Level
    // ==============================
    sprintf(level_text, "%02d/%02d", game_state.level, MAX_LEVEL);
    DrawText(level_text, SCREEN_WIDTH - 106, 16, 32, BLACK);
}
