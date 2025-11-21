//
// Created by Harry on 10/11/2025.
//

#include <math.h>

#include "game_state.h"
#include "raylib.h"
#include "stdio.h"
#include "player.h"

void draw_hud() {
    Player *player = &game_state.player;

    char moneyText[64];
    sprintf(moneyText, "Moedas: %d", player->money);
    DrawText(moneyText, 10, 750, 30, BLACK);

    // Desenha o ganho
    if (player->money_gain_timer > 0.0f) {
        // Fazer sumir depois de um tempo
        float alpha = player->money_gain_timer / 1.5f;
        int transparency = (int) (255 * alpha);

        Color gainColor = (Color){0, 0, 0, transparency};

        char gainText[32];
        sprintf(gainText, "%c%d", player->last_money_gain >= 0 ? '+' : '-', abs(player->last_money_gain));
        DrawText(gainText, 140, 720, 20, gainColor);
    }


    // ==============================
    // Barra de Almas
    // ==============================
    float maxSouls = player->max_souls;
    float souls = player->souls;

    float barWidth = 200;

    // Calcula a proporção preenchida
    float fillWidth = (souls / maxSouls) * barWidth;

    // Cor de fundo da barra
    DrawRectangle(10, 50, 200, 20, (Color){80, 80, 80, 255});

    // Cor da barra conforme a quantidade
    Color fillColor;
    if (souls < 33) {
        fillColor = GRAY;
    } else {
        fillColor = WHITE;
    }

    // Desenha o preenchimento
    DrawRectangle(10, 50, fillWidth, 20, fillColor);

    // Moldura
    DrawRectangleLines(10, 50, barWidth, 20, BLACK);

    // ==============================
    // Barra de Vida
    // ==============================
    int squareSize = 30;
    int spacing = 5;
    int startX = 10;
    int startY = 10;

    for (int i = 0; i < player->combat.max_life; i++) {
        int x = startX + i * (squareSize + spacing);
        Color color = (i < player->combat.life) ? WHITE : (Color){100, 100, 100, 255};
        DrawRectangle(x, startY, squareSize, squareSize, color);
        DrawRectangleLines(x, startY, squareSize, squareSize, BLACK);
    }
}
