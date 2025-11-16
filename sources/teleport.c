//
// Created by richa on 16/11/2025.
//

#include "config.h"
#include "teleport.h"
#include "game_state.h"

Teleport teleports[MAX_TELEPORTS];
int teleports_count = 0;

void add_teleport(int x, int y) {
    if (teleports_count < MAX_TELEPORTS) {
        teleports[teleports_count++] = (Teleport){
            x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE
        };
    }
}

void draw_teleports() {
    for (int i = 0; i < teleports_count; i++)
        DrawRectangle(teleports[i].x, teleports[i].y, teleports[i].width, teleports[i].height, YELLOW);
}

bool handle_teleports_interaction() {
    Player *player = &game_state.player;

    for (int i = 0; i < teleports_count; i++)
        if (CheckCollisionRecs(player->hitbox, teleports[i])) {
            return true;
        }

    return false;
}
