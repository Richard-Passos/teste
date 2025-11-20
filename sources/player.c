
#include <string.h>
#include "raylib.h"
#include "player.h"
#include "config.h"
#include "enemies.h"
#include "abilities-attacks.h"
#include "game_state.h"
#include "boss.h"
#include "shop.h"

void add_player(int x, int y) {
    Player *player = &game_state.player;

    // Reset player pos when necessary
    //----------------------------------------------------------------------------------
    if (player->should_keep_pos)
        player->should_keep_pos = false;
    else
        player->hitbox = (Rectangle){
            x * TILE_SIZE, y * TILE_SIZE, player->hitbox.width,
            player->hitbox.height
        };
    //----------------------------------------------------------------------------------
}

void draw_player() {
    Player *player = &game_state.player;

    // Escolhe cor do player com base no estado de dano
    Color color_player = GREEN;

    if (player->combat.hurt_timer > 0) {
        // Piscar branco semi-transparente
        if (player->combat.hurt_timer > 0)
            color_player = WHITE;
        else
            color_player = (Color){255, 255, 255, 90};
    }

    // Desenha o player com a cor de piscada
    DrawRectangleRec(player->hitbox, color_player);

    if (player->is_attacking) {
        DrawRectangleRec(player->attack_box, (Color){0, 0, 0, 120});
    }

    if (player->abilitySoulProjectile.active) {
        DrawRectangleRec(player->abilitySoulProjectile.hitbox, WHITE);
    }
}

bool has_collided(Player *player, Wall *wall) {
    return (
        player->hitbox.x < wall->hitbox.x + wall->hitbox.width &&
        player->hitbox.x + player->hitbox.width > wall->hitbox.x &&
        player->hitbox.y < wall->hitbox.y + wall->hitbox.height &&
        player->hitbox.y + player->hitbox.height > wall->hitbox.y
    );
}

void PlayerMonsterCollision(Player *player, float delta) {

    if (player->combat.invuln_timer > 0.0f) {
        player->combat.invuln_timer -= delta;
        if (player->combat.invuln_timer <= 0.0f) {
            player->combat.invulnerable = false;
            player->combat.invuln_timer = 0.0f;
        }
    }

    if (player->combat.hurt_timer > 0.0f) {
        player->combat.hurt_timer -= delta;
        if (player->combat.hurt_timer < 0.0f) player->combat.hurt_timer = 0.0f;
    }

    // Só checamos colisões para aplicar dano caso NÃO esteja invulnerável
    if (player->combat.invulnerable) return;

    for (int i = 0; i < monsters_count; i++) {
        // pula monstros "mortos"
        if (monsters[i].hitbox.width == 0 || monsters[i].hitbox.height == 0) continue;

        if (CheckCollisionRecs(player->hitbox, monsters[i].hitbox)) {
            // Aplica dano
            player->combat.life -= 1;
            if (player->combat.life < 0) player->combat.life = 0;

            // Invulnerabilidade + efeito visual
            player->combat.invulnerable = true;
            player->combat.invuln_timer = 1.0f;
            player->combat.hurt_timer = 0.1f;

            // Knockback: define direção/opções
            // Se o jogador está à esquerda do monstro, empurra para a esquerda (-1)
            float dir = (player->hitbox.x < monsters[i].hitbox.x) ? -1.0f : 1.0f;
            player->combat.push_dir = dir;
            player->combat.push_speed = 420.0f; // velocidade horizontal do knockback (ajustar)
            player->combat.push_timer = 0.18f; // duração do empurrão em segundos (ajustar)

            // Aplica pequena elevação imediata se estiver no chão para dar "pop"
            if (player->on_ground) {
                player->speed.y = -220.0f;
                player->on_ground = false;
            } else {
                // Se já no ar, pode aplicar um impacto menor
                player->speed.y = -120.0f;
            }

            break; // apenas um hit por frame
        }
    }

    // ---------------------------------------------
    // Colisão do BOSS com o jogador (dano ao player)
    // ---------------------------------------------
    if (boss.active && boss.hitbox.width > 0 && boss.hitbox.height > 0) {

        if (!player->combat.invulnerable) {

            if (CheckCollisionRecs(player->hitbox, boss.hitbox)) {

                // Dano
                player->combat.life -= 1;
                if (player->combat.life < 0) player->combat.life = 0;

                // Invulnerabilidade + efeito visual
                player->combat.invulnerable = true;
                player->combat.invuln_timer = 1.0f;
                player->combat.hurt_timer = 0.1f;

                // Knockback igual ao dos monstros
                float dir = (player->hitbox.x < boss.hitbox.x) ? -1.0f : 1.0f;
                player->combat.push_dir = dir;
                player->combat.push_speed = 420.0f;
                player->combat.push_timer = 0.18f;

                // Pop no jogador
                if (player->on_ground) {
                    player->speed.y = -220.0f;
                    player->on_ground = false;
                } else {
                    player->speed.y = -120.0f;
                }
            }
        }
    }
}

void update_player(float delta) {
    Player *player = &game_state.player;

    // ---------------------------------------------------------
    // 1) Atualizar timers de combate
    // ---------------------------------------------------------
    if (player->combat.push_timer > 0.0f) {
        player->combat.push_timer -= delta;
        if (player->combat.push_timer < 0.0f)
            player->combat.push_timer = 0.0f;
    }


    // ---------------------------------------------------------
    // 2) Movimentação horizontal e Dash
    // ---------------------------------------------------------
    if (player->combat.push_timer > 0.0f) {
        player->speed.x = player->combat.push_dir * player->combat.push_speed;
    } else {
        player->speed.x = 0.0f;

        bool dash_hit = DashAbility(player);

        if (dash_hit) {
            player->speed.x = 0.0f;
            return;
        }


        if (player->combat.is_healing) {
            player->speed.x = 0.0f; // Travar movimentação
            goto apply_horizontal; // Pular para aplicar movimento
        }

        if (player->dash.recovery > 0.0f) {
            return;
        } else {
            if (!player->dash.active) {
                if (IsKeyDown(KEY_LEFT)) {
                    player->speed.x = -PLAYER_HOR_SPEED;
                    player->facing_right = false;
                } else if (IsKeyDown(KEY_RIGHT)) {
                    player->speed.x = PLAYER_HOR_SPEED;
                    player->facing_right = true;
                }
            }
        }
    }

apply_horizontal:
    player->hitbox.x += player->speed.x * delta;

    // Colisão horizontal com paredes
    for (int i = 0; i < walls_count; i++) {
        Wall *wall = &walls[i];
        if (has_collided(player, wall)) {
            if (player->speed.x > 0)
                player->hitbox.x = wall->hitbox.x - player->hitbox.width;
            else if (player->speed.x < 0)
                player->hitbox.x = wall->hitbox.x + wall->hitbox.width;
            player->speed.x = 0;
            player->combat.push_timer = 0.0f;
        }
    }

    // ---------------------------------------------------------
    // 3) Movimentação vertical (gravidade e pulo)
    // ---------------------------------------------------------
    player->speed.y += player->is_sitting ? 0 : GRAVITY * delta;

    bool falling = (player->speed.y > 0 && !player->on_ground);

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) && player->combat.push_timer <= 0.0f) {
        if (player->on_ground && player->can_jump) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 1;
            player->can_jump = true;
        } else if (!player->on_ground && player->jump_count == 1) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 2;
        } else if (!player->on_ground && player->jump_count == 0 && falling) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 2;
        }
    }

    player->hitbox.y += player->speed.y * delta;

    // Colisão vertical
    bool grounded_this_frame = false;
    for (int i = 0; i < walls_count; i++) {
        Wall *wall = &walls[i];
        if (has_collided(player, wall)) {
            if (player->speed.y > 0) {
                player->hitbox.y = wall->hitbox.y - player->hitbox.height;
                grounded_this_frame = true;
                player->speed.y = 0;
                player->jump_count = 0;
                player->can_jump = true;
            } else if (player->speed.y < 0) {
                player->hitbox.y = wall->hitbox.y + wall->hitbox.height;
                player->speed.y = 0;
            }
        }
    }
    player->on_ground = grounded_this_frame;

    // ---------------------------------------------------------
    // 4) Colisão com monstros
    // ---------------------------------------------------------
    PlayerMonsterCollision(player, delta);


    // ---------------------------------------------------------
    // 5) Ataque
    // ---------------------------------------------------------
    if (player->attack_cooldown > 0.0f)
        player->attack_cooldown -= delta;

    if (IsKeyPressed(KEY_X) && player->attack_cooldown <= 0.0f && player->combat.push_timer <= 0.0f) {
        int dir = 0;
        if (IsKeyDown(KEY_DOWN)) dir = -1;
        else if (IsKeyDown(KEY_UP)) dir = 1;

        if (!(dir == -1 && player->on_ground)) {
            player->is_attacking = true;
            player->attack_timer = 0.12f;
            player->attack_dir = dir;
            memset(player->monsters_hit, 0, sizeof(player->monsters_hit));
            player->boss_hit = false;
            player->attack_cooldown = 0.4f;
        }
    }

    if (player->is_attacking) {
        player->attack_timer -= delta;

        if (player->attack_dir == 1) {
            player->attack_box = (Rectangle){
                player->hitbox.x + player->hitbox.width / 4.0f,
                player->hitbox.y - TILE_SIZE * 1.5f,
                player->hitbox.width / 2.0f,
                TILE_SIZE * 1.5f
            };
        } else if (player->attack_dir == -1) {
            // baixo
            player->attack_box = (Rectangle){
                player->hitbox.x + player->hitbox.width / 4.0f,
                player->hitbox.y + player->hitbox.height,
                player->hitbox.width / 2.0f,
                TILE_SIZE * 1.5f
            };
        } else {
            // frente
            float offsetX = (player->facing_right) ? player->hitbox.width : -TILE_SIZE * 2;
            player->attack_box = (Rectangle){
                player->hitbox.x + offsetX,
                player->hitbox.y + player->hitbox.height / 4.0f,
                TILE_SIZE * 2,
                player->hitbox.height / 2.0f
            };
        }

        for (int j = 0; j < monsters_count; j++) {
            if (!player->monsters_hit[j]) {
                if (CheckCollisionRecs(player->attack_box, monsters[j].hitbox)) {
                    monsters[j].life -= 1;
                    monsters[j].hurt_timer = 0.1f;
                    monsters[j].invulnerable = true;
                    monsters[j].invuln_time = 0.3f;

                    if (player->souls <= player->max_souls - 10) player->souls += 10;

                    player->monsters_hit[j] = true;

                    if (monsters[j].life <= 0) {
                        monsters[j].hitbox.height = 0;
                        monsters[j].hitbox.width = 0;
                        monsters[j].hitbox.x = -1;
                        monsters[j].hitbox.y = -1;

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

        if (player->attack_timer <= 0.0f)
            player->is_attacking = false;
    }


    if (boss.active && player->is_attacking) {

        // Se ainda não atingiu o boss neste ataque
        if (!player->boss_hit) {

            if (CheckCollisionRecs(player->attack_box, boss.hitbox)) {

                boss.life -= 1;
                boss.hurt_timer = 0.1f;
                boss.invulnerable = true;
                boss.invuln_time = 0.3f;

                // Ganho de almas como no monstro
                if (player->souls <= player->max_souls - 10)
                    player->souls += 10;

                player->boss_hit = true;

                // Morte do boss
                if (boss.life <= 0) {
                    boss.active = false;

                    int gained = GetRandomValue(150, 200);
                    player->money += gained;
                    player->last_money_gain = gained;
                    player->money_gain_timer = 2.0f;
                }

                // Pogo Jump — igual o monstro
                if (player->attack_dir == -1 && player->speed.y > 0) {
                    player->speed.y = -PLAYER_JUMP_SPEED * 0.7f;
                    player->jump_count = 1;
                    player->on_ground = false;
                }
            }
        }
    }

    // Timer do dinheiro
    if (player->money_gain_timer > 0.0f) {
        player->money_gain_timer -= delta;
        if (player->money_gain_timer < 0.0f) player->money_gain_timer = 0.0f;
    }

    // Projetil de almas + aquisição de habilidades
    AbilitiesProjectile(player, delta);
    update_ability_acquisition();

    // Cura
    HealAbility(player, delta);
}
