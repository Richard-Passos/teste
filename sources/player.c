//
// Created by richa on 26/10/2025.
//

#include "raylib.h"
#include "player.h"
#include "config.h"
#include "map.h"
#include "stdio.h"
#include <string.h>

bool has_collided(Player *player, Wall *wall) {
    return (
        player->hitbox.x < wall->hitbox.x + wall->hitbox.width &&
        player->hitbox.x + player->hitbox.width > wall->hitbox.x &&
        player->hitbox.y < wall->hitbox.y + wall->hitbox.height &&
        player->hitbox.y + player->hitbox.height > wall->hitbox.y
    );
}

int jumps = 0;

void update_player(Player *player, float delta) {
    // Movement horizontal
    //--------------------------------------------------------------------------------------
    player->speed.x = 0.0f;

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->speed.x = -PLAYER_HOR_SPEED;
        player->facing_right = false;
    }
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->speed.x = PLAYER_HOR_SPEED;
        player->facing_right = true;
    }


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


    if (player->on_ground == true) {
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && player->can_jump) {
        player->speed.y = -PLAYER_JUMP_SPEED;
        jumps += 1;
        }


            if (jumps == 2) {
                player->can_jump = false;
                jumps = 0;
            }
        }
    else{
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && player->can_jump) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->can_jump = false;
            jumps = 0;
        }
    }


    player->hitbox.y += player->speed.y * delta;
    //--------------------------------------------------------------------------------------

    // Collision vertical
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < walls_count; i++) {
        Wall *wall = &walls[i];

        if (has_collided(player, wall)) {
            if (player->speed.y > 0) {
                /* Falling / from top */
                player->hitbox.y = wall->hitbox.y - player->hitbox.height;
                player->on_ground = true;
                player->can_jump = true;
            } else if (player->speed.y < 0) {
                /* Jumping / from bottom */
                player->hitbox.y = wall->hitbox.y + wall->hitbox.height;
            }
            player->speed.y = 0;
        }
        else {
            if (player->on_ground != true)
            player->on_ground = false;
        }
    }
    //--------------------------------------------------------------------------------------

    // Attack in any direction


        // Inicia ataque
        if (IsKeyPressed(KEY_C)) {
            player->is_attacking = true;
            player->attack_timer = 0.12f; // duração do ataque em segundos (ajuste)
            // reseta flags de quem já foi atingido nesse ataque
            memset(player->monsters_hit, 0, sizeof(player->monsters_hit));
        }

        // Atualiza ataque enquanto ativo
        if (player->is_attacking) {
            player->attack_timer -= delta;

            // recalcula offset baseado na direção
            float offsetX = (player->facing_right) ? player->hitbox.width : -WALL_SIZE * 2;

            player->attack_box = (Rectangle){
                player->hitbox.x + offsetX,
                player->hitbox.y + player->hitbox.height / 4.0f,
                WALL_SIZE * 2,
                player->hitbox.height / 2.0f
            };

            // checagem de colisão com monstros
            for (int j = 0; j < monsters_count; j++) {
                if (!player->monsters_hit[j]) {
                    if (CheckCollisionRecs(player->attack_box, monsters[j].hitbox)) {
                        player->monsters_hit[j] = true; // marca como atingido
                        printf("acertou hit no monstro %d\n", j);
                    }
                }
            }

            // se terminou o tempo do ataque, desativa
            if (player->attack_timer <= 0.0f) {
                player->is_attacking = false;
            }
        }
    }
