
#include <string.h>
#include "raylib.h"
#include "player.h"

#include <math.h>
#include <stdio.h>

#include "config.h"
#include "enemies.h"
#include "abilities.h"
#include "game_state.h"
#include "boss.h"
#include "screen.h"

void add_player(int x, int y) {
    Player *player = &game_state.player;

    // Reset player pos when necessary
    //----------------------------------------------------------------------------------
    if (player->should_keep_pos)
        player->should_keep_pos = false;
    else
        player->hitbox = (Rectangle){
            x * TILE_SIZE, y * TILE_SIZE, player->hitbox.width, player->hitbox.height
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

    if (player->abilities.soul_projectile->is_acquired && player->abilities.soul_projectile->is_active) {
        DrawRectangleRec(player->abilities.soul_projectile->hitbox, WHITE);
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

void update_player_items() {
    Player *player = &game_state.player;

    if (player->items.add_life->is_acquired && player->items.add_life->is_active) {
        player->combat.max_life = 7;
    } else {
        player->combat.max_life = 5;
    }
    if (player->items.add_damage->is_acquired && player->items.add_damage->is_active) {
        player->combat.damage = 2;
    } else {
        player->combat.damage = 1;
    }
    if (player->items.add_speed->is_acquired && player->items.add_speed->is_active) {
        player->multipliers.speed = 1.2f;
    } else {
        player->multipliers.speed = 1;
    }
    if (player->items.add_money->is_acquired && player->items.add_money->is_active) {
        player->multipliers.money = 1.5f;
    } else {
        player->multipliers.money = 1;
    }
    if (player->items.add_invuln_dash->is_acquired && player->items.add_invuln_dash->is_active)
        if (player->abilities.dash->is_active) {
            player->combat.invulnerable = true;
            player->combat.invuln_timer = .3f;
        }
}

void update_player() {
    update_ability_acquisition();
    soul_projectile_ability();
    heal_ability();
    double_jump_ability();

    update_item_acquisition();
    update_player_items();

    handle_timers();
    handle_horizontal_movement();
    handle_vertical_movement();
    handle_monsters_collision();
    handle_attack();
}

void handle_timers() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    if (player->combat.push_timer > 0.0f) {
        player->combat.push_timer -= delta_time;

        if (player->combat.push_timer < 0.0f)
            player->combat.push_timer = 0.0f;
    }

    if (player->money_gain_timer > 0.0f) {
        player->money_gain_timer -= delta_time;

        if (player->money_gain_timer < 0.0f)
            player->money_gain_timer = 0.0f;
    }
}

void handle_horizontal_movement() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    dash_ability();

    if (player->combat.push_timer > 0.0f) {
        player->speed.x = player->combat.push_dir * player->combat.push_speed;
    } else {
        player->speed.x = 0;

        if (player->abilities.heal == NULL ||
            (player->abilities.heal->is_acquired && !player->abilities.heal->is_active)) {
            // Movimento normal (SEM DASH)
            if (IsKeyDown(KEY_LEFT)) {
                player->speed.x = -PLAYER_HOR_SPEED * player->multipliers.speed;
                player->facing_right = false;
            } else if (IsKeyDown(KEY_RIGHT)) {
                player->speed.x = PLAYER_HOR_SPEED * player->multipliers.speed;
                player->facing_right = true;
            }
        }
    }

    player->hitbox.x += player->speed.x * player->multipliers.speed * delta_time;

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
}

void handle_vertical_movement() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    player->speed.y += player->is_sitting ? 0 : GRAVITY * delta_time;

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) && player->combat.push_timer <= 0.0f) {
        if (player->on_ground && player->can_jump) {
            player->speed.y = -PLAYER_JUMP_SPEED;
            player->jump_count = 1;
            player->can_jump = true;
        }
    }

    player->hitbox.y += player->speed.y * delta_time;

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
}

void handle_attack() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    if (player->attack_cooldown > 0.0f)
        player->attack_cooldown -= delta_time;

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
        player->attack_timer -= delta_time;

        if (player->attack_dir == 1) {
            player->attack_box = (Rectangle){
                player->hitbox.x + player->hitbox.width / 4.0f,
                player->hitbox.y - TILE_SIZE * 1.5f,
                player->hitbox.width / 2.0f,
                TILE_SIZE * 1.5f
            };
        } else if (player->attack_dir == -1) {
            player->attack_box = (Rectangle){
                player->hitbox.x + player->hitbox.width / 4.0f,
                player->hitbox.y + player->hitbox.height,
                player->hitbox.width / 2.0f,
                TILE_SIZE * 1.5f
            };
        } else {
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
                    monsters[j].life -= player->combat.damage;
                    monsters[j].hurt_timer = 0.1f;
                    monsters[j].invulnerable = true;
                    monsters[j].invuln_time = 0.3f;

                    add_player_souls(10);

                    player->monsters_hit[j] = true;

                    if (monsters[j].life <= 0) {
                        monsters[j].hitbox.height = 0;
                        monsters[j].hitbox.width = 0;
                        monsters[j].hitbox.x = -1;
                        monsters[j].hitbox.y = -1;

                        add_player_money(GetRandomValue(35, 45));
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

    // Ataque no boss
    if (boss.active && player->is_attacking) {
        if (!player->boss_hit) {
            if (CheckCollisionRecs(player->attack_box, boss.hitbox)) {
                boss.life -= player->combat.damage;
                boss.hurt_timer = 0.1f;
                boss.invulnerable = true;
                boss.invuln_time = 0.3f;

                add_player_souls(10);

                player->boss_hit = true;

                if (boss.life <= 0) {
                    boss.active = false;

                    add_player_money(GetRandomValue(150, 200));

                    game_state.level++;
                    set_screen(SCREEN_VILLAGE);
                }

                if (player->attack_dir == -1 && player->speed.y > 0) {
                    player->speed.y = -PLAYER_JUMP_SPEED * 0.7f;
                    player->jump_count = 1;
                    player->on_ground = false;
                }
            }
        }
    }
}

void handle_monsters_collision() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    if (player->combat.invuln_timer > 0.0f) {
        player->combat.invuln_timer -= delta_time;
        if (player->combat.invuln_timer <= 0.0f) {
            player->combat.invulnerable = false;
            player->combat.invuln_timer = 0.0f;
        }
    }

    if (player->combat.hurt_timer > 0.0f) {
        player->combat.hurt_timer -= delta_time;
        if (player->combat.hurt_timer < 0.0f) player->combat.hurt_timer = 0.0f;
    }

    // Só checamos colisões para aplicar dano caso NÃO esteja invulnerável
    if (player->combat.invulnerable) return;

    for (int i = 0; i < monsters_count; i++) {
        if (player->ignore_next_monster_hit) {
            player->ignore_next_monster_hit = false;
            return; // ignora colisão só neste frame
        }

        // pula monstros "mortos"
        if (monsters[i].hitbox.width == 0 || monsters[i].hitbox.height == 0) continue;

        if (!player->abilities.dash->is_active && CheckCollisionRecs(player->hitbox, monsters[i].hitbox)) {
            // Aplica dano
            player->combat.life -= 1;
            if (player->combat.life < 0) player->combat.life = 0;

            // Invulnerabilidade + efeito visual
            player->combat.invulnerable = true;
            player->combat.invuln_timer = 1.0f;
            player->combat.hurt_timer = 0.1f;

            // Knockback:
            // Se o jogador está à esquerda do monstro, empurra para a esquerda (-1)
            float dir = (player->hitbox.x < monsters[i].hitbox.x) ? -1.0f : 1.0f;
            player->combat.push_dir = dir;
            player->combat.push_speed = 620.0f; // velocidade horizontal do knockback
            player->combat.push_timer = 0.18f; // duração do empurrão em segundos

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
        if (player->abilities.dash->is_active && CheckCollisionRecs(player->hitbox, monsters[i].hitbox)) {
            if (!player->combat.invulnerable) {
                // Knockback
                float dir = (player->hitbox.x < monsters[i].hitbox.x) ? -1.0f : 1.0f;
                player->combat.push_dir = dir;
                player->combat.push_speed = 620.0f;
                player->combat.push_timer = 0.18f;
            }
        }
    }

    // ---------------------------------------------
    // Colisão do BOSS com o jogador (dano ao player)
    // ---------------------------------------------
    if (!player->abilities.dash->is_active && boss.active && boss.hitbox.width > 0 && boss.hitbox.height > 0) {
        if (!player->combat.invulnerable) {
            if (CheckCollisionRecs(player->hitbox, boss.hitbox)) {
                // Dano
                player->combat.life -= 2;
                if (player->combat.life < 0) player->combat.life = 0;

                // Invulnerabilidade + efeito visual
                player->combat.invulnerable = true;
                player->combat.invuln_timer = 1.0f;
                player->combat.hurt_timer = 0.1f;

                // Knockback
                float dir = (player->hitbox.x < boss.hitbox.x) ? -1.0f : 1.0f;
                player->combat.push_dir = dir;
                player->combat.push_speed = 820.0f;
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

    if (player->abilities.dash->is_active && CheckCollisionRecs(player->hitbox, boss.hitbox)) {
        if (!player->combat.invulnerable) {
            // Knockback
            float dir = (player->hitbox.x < boss.hitbox.x) ? -1.0f : 1.0f;
            player->combat.push_dir = dir;
            player->combat.push_speed = 820.0f;
            player->combat.push_timer = 0.18f;
        }
    }
}

void spawn_player_on_bench() {
    Player *player = &game_state.player;

    if (!player->has_spawn) return;

    player->combat.life = player->combat.max_life;
    player->speed.y = 0;
    player->speed.x = 0;
    player->combat.push_speed = 0;

    player->hitbox = (Rectangle){
        player->spawn_pos.x, player->spawn_pos.y, player->hitbox.width, player->hitbox.height
    };
    player->souls = player->max_souls;
    player->is_sitting = true;
    player->should_keep_pos = true;
}

void add_player_money(int money) {
    Player *player = &game_state.player;

    money *= player->multipliers.money;

    player->money += money;
    player->last_money_gain = money;
    player->money_gain_timer = 1.5f;
}

void add_player_souls(int souls) {
    Player *player = &game_state.player;

    int player_souls = player->souls + souls;

    if (player_souls > player->max_souls)
        player->souls = player->max_souls;
    else if (player_souls < 0)
        player->souls = 0;
    else
        player->souls = player_souls;
}
