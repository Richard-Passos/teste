//
// Created by richa on 26/10/2025.
//

#include "raylib.h"
#include "player.h"
#include "config.h"
#include "map.h"

bool has_collided(Player *player, Wall *wall) {
    return (
        player->hitbox.x < wall->hitbox.x + wall->hitbox.width &&
        player->hitbox.x + player->hitbox.width > wall->hitbox.x &&
        player->hitbox.y < wall->hitbox.y + wall->hitbox.height &&
        player->hitbox.y + player->hitbox.height > wall->hitbox.y
    );
}

void update_player(Player *player, float delta) {
    // Movement horizontal
    //--------------------------------------------------------------------------------------
    player->speed.x = 0.0f;

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        player->speed.x = -PLAYER_HOR_SPEED;
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        player->speed.x = PLAYER_HOR_SPEED;

    player->hitbox.x += player->speed.x * delta;
    //--------------------------------------------------------------------------------------

    // Collision horizontal
    //--------------------------------------------------------------------------------------
    for (int i = 0; i < walls_count; i++) {
        Wall *wall = &walls[i];
        if (has_collided(player, wall)) {
            if (player->speed.x > 0)
                player->hitbox.x = wall->hitbox.x - player->hitbox.width;
            else if (player->speed.x < 0)
                player->hitbox.x = wall->hitbox.x + wall->hitbox.width;
            player->speed.x = 0;
        }
    }
    //--------------------------------------------------------------------------------------

    // Movement vertical
    //--------------------------------------------------------------------------------------
    player->speed.y += GRAVITY * delta;

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && player->can_jump) {
        player->speed.y = -PLAYER_JUMP_SPEED;
        player->can_jump = false;
    }

    player->hitbox.y += player->speed.y * delta;
    //--------------------------------------------------------------------------------------

    // Collision vertical
    //--------------------------------------------------------------------------------------
    player->can_jump = false;

    for (int i = 0; i < walls_count; i++) {
        Wall *wall = &walls[i];

        if (has_collided(player, wall)) {
            if (player->speed.y > 0) {
                /* Falling / from top */
                player->hitbox.y = wall->hitbox.y - player->hitbox.height;
                player->can_jump = true;
            } else if (player->speed.y < 0) {
                /* Jumping / from bottom */
                player->hitbox.y = wall->hitbox.y + wall->hitbox.height;
            }
            player->speed.y = 0;
        }
    }
    //--------------------------------------------------------------------------------------
}
