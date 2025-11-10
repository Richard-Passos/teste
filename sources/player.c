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

    // Verifica início de queda: se não estiver no chão e velocidade > 0 (caindo)
    bool falling = (player->speed.y > 0 && !player->on_ground);

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) {
        // Pulo normal do chão
        if (player->on_ground && player->can_jump) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 1; // Conta como 1 pulo
            player->can_jump = true;
        }
        // Pulo duplo (no ar)
        else if (!player->on_ground && player->jump_count == 1) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 2; // Agora usou o segundo pulo
        }
        // Caso tenha caído sem pular (sem estar no chão e sem pulo anterior)
        else if (!player->on_ground && player->jump_count == 0 && falling) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 2; // Marca como se fosse o segundo pulo
        }
    }

    player->hitbox.y += player->speed.y * delta;

    // --- Colisão vertical ---
    bool grounded_this_frame = false;

    for (int i = 0; i < walls_count; i++) {
        Wall *wall = &walls[i];

        if (has_collided(player, wall)) {
            if (player->speed.y > 0) {
                player->hitbox.y = wall->hitbox.y - player->hitbox.height;
                grounded_this_frame = true;
                player->speed.y = 0;
                player->jump_count = 0;  // Reset total de pulos ao tocar o chão
                player->can_jump = true;
            } else if (player->speed.y < 0) {
                player->hitbox.y = wall->hitbox.y + wall->hitbox.height;
                player->speed.y = 0;
            }
        }
    }

    player->on_ground = grounded_this_frame;


    //--------------------------------------------------------------------------------------

    // Attack in any direction //


    // Atualiza cooldown do ataque
    if (player->attack_cooldown > 0.0f)
        player->attack_cooldown -= delta;

    // Inicia ataque se a tecla for pressionada e o cooldown acabou
    if (IsKeyPressed(KEY_X) && player->attack_cooldown <= 0.0f) {
        // Prioridade de direção: baixo > cima > horizontal
        int dir = 0; // 0 = horizontal, 1 = cima, -1 = baixo
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
            dir = -1;
        else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
            dir = 1;

        // Somente bloqueia ataque para baixo no chão
        if (dir == -1 && player->on_ground) {
            // Ignora ataque para baixo no chão
        } else {
            player->is_attacking = true;
            player->attack_timer = 0.12f;
            player->attack_dir = dir;
            memset(player->monsters_hit, 0, sizeof(player->monsters_hit));

            // Reseta cooldown (impede spam)
            player->attack_cooldown = 0.4f; // tempo mínimo entre ataques
        }
    }

    // Atualiza ataque enquanto ativo
    if (player->is_attacking) {
        player->attack_timer -= delta;

        // Atualiza hitbox conforme direção
        if (player->attack_dir == 1) {
            // Cima
            player->attack_box = (Rectangle){
                player->hitbox.x + player->hitbox.width / 4.0f,
                player->hitbox.y - WALL_SIZE * 1.5f,
                player->hitbox.width / 2.0f,
                WALL_SIZE * 1.5f
            };
        }
        else if (player->attack_dir == -1) {
            // Baixo
            player->attack_box = (Rectangle){
                player->hitbox.x + player->hitbox.width / 4.0f,
                player->hitbox.y + player->hitbox.height,
                player->hitbox.width / 2.0f,
                WALL_SIZE * 1.5f
            };
        }
        else {
            // Frente
            float offsetX = (player->facing_right) ? player->hitbox.width : -WALL_SIZE * 2;
            player->attack_box = (Rectangle){
                player->hitbox.x + offsetX,
                player->hitbox.y + player->hitbox.height / 4.0f,
                WALL_SIZE * 2,
                player->hitbox.height / 2.0f
            };
        }


        // Checagem de colisão com monstros
        for (int j = 0; j < monsters_count; j++) {
            if (!player->monsters_hit[j]) {
                if (CheckCollisionRecs(player->attack_box, monsters[j].hitbox)) {
                    monsters[j].life -= 1; // 1 de dano
                    monsters[j].hurt_timer = 0.1f;
                    monsters[j].invulnerable = true;
                    monsters[j].invuln_time = 0.3f;

                    // Marca que já foi atingido neste ataque
                    player->monsters_hit[j] = true;

                    // Se morrer, remove e adiciona dinheiro
                    if (monsters[j].life <= 0) {
                        monsters[j].hitbox.height = 0;
                        monsters[j].hitbox.width = 0;

                        int gained = GetRandomValue(35, 45);
                        player->money += gained;

                        player->last_money_gain = gained;
                        player->money_gain_timer = 1.5f;
                    }

                    // Pogo Jump
                    if (player->attack_dir == -1 && player->speed.y > 0) {
                        player->speed.y = -PLAYER_JUMP_SPEED * 0.7f;
                        player->jump_count = 1;
                        player->on_ground = false;
                    }
                }
            }
        }
        // Termina ataque quando o timer zera
        if (player->attack_timer <= 0.0f)
            player->is_attacking = false;
    }

    // diminuir timer do dinheiro
    if (player->money_gain_timer > 0.0f) {
        player->money_gain_timer -= delta;
        if (player->money_gain_timer < 0.0f)
            player->money_gain_timer = 0.0f;
    }
}