
#include <string.h>
#include "raylib.h"
#include "player.h"
#include <stdio.h>
#include "enemies.h"
#include "abilities.h"
#include "game_state.h"
#include "boss.h"

void add_player(int x, int y) {
    Player *player = &game_state.player;

    if (player->should_keep_pos)
        player->should_keep_pos = false;
    else
        player->hitbox = (Rectangle){
            x * TILE_SIZE, y * TILE_SIZE, player->hitbox.width, player->hitbox.height
        };
}

void draw_player() {
    Player *player = &game_state.player;

    Color player_color = player->combat.hurt_timer > 0 ? WHITE : GREEN;
    DrawRectangleRec(player->hitbox, player_color);

    if (player->combat.is_attacking)
        DrawRectangleRec(player->combat.attack_box, (Color){0, 0, 0, 120});
    if (is_ability_active(player->abilities.soul_projectile))
        DrawRectangleRec(player->abilities.soul_projectile->hitbox, WHITE);
}

void update_player_items() {
    Player *player = &game_state.player;

    player->combat.max_life = is_item_active(player->items.add_life) ? 7 : 5;
    player->combat.damage = is_item_active(player->items.add_damage) ? 2 : 1;
    player->multipliers.speed = is_item_active(player->items.add_speed) ? 1.5f : 1;
    player->multipliers.money = is_item_active(player->items.add_money) ? 1.5f : 1;
    if (is_item_active(player->items.add_invuln_dash) && is_ability_active(player->abilities.dash)) {
        player->combat.invulnerable = true;
        player->combat.invuln_timer = .3f;
    }
}

void update_player() {
    update_ability_acquisition();
    soul_projectile_ability();
    dash_ability();
    double_jump_ability();
    heal_ability();

    update_item_acquisition();
    update_player_items();

    handle_player_horizontal_movement();
    handle_player_vertical_movement();
    handle_player_monsters_collision();
    handle_player_attack();
}

void handle_player_horizontal_movement() {
    Player *player = &game_state.player;

    // Smooth movement to knockback
    if (player->speed.x < -PLAYER_SPEED || player->speed.x > PLAYER_SPEED)
        player->speed.x += PLAYER_SPEED * (player->speed.x > 0 ? -DELTA_TIME : DELTA_TIME);
    else
        player->speed.x = 0;

    if (!is_ability_active(player->abilities.heal) && player->combat.hurt_timer <= 0) {
        if (IsKeyDown(KEY_LEFT)) {
            player->speed.x = -PLAYER_SPEED;
            player->direction = DIR_LEFT;
        } else if (IsKeyDown(KEY_RIGHT)) {
            player->speed.x = PLAYER_SPEED;
            player->direction = DIR_RIGHT;
        }
    }

    player->hitbox.x += player->speed.x * player->multipliers.speed * DELTA_TIME;

    // Collision
    Wall *hit_wall = handle_collision_with_walls(player->hitbox);

    if (hit_wall != NULL) {
        player->hitbox.x = hit_wall->hitbox.x;
        if (player->speed.x > 0)
            player->hitbox.x -= player->hitbox.width;
        else
            player->hitbox.x += hit_wall->hitbox.width;

        player->speed.x = 0;
    }
}

void handle_player_vertical_movement() {
    Player *player = &game_state.player;

    // Movement
    player->speed.y += player->is_sitting ? 0 : GRAVITY * DELTA_TIME;

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) && player->jump_count == 0 && player->on_ground) {
        player->speed.y = -PLAYER_JUMP_SPEED;
        player->jump_count = 1;
        player->on_ground = false;
    }

    player->hitbox.y += player->speed.y * DELTA_TIME;

    // Collision
    Wall *hit_wall = handle_collision_with_walls(player->hitbox);

    if (hit_wall == NULL)
        player->on_ground = false;
    else {
        player->hitbox.y = hit_wall->hitbox.y;
        if (player->speed.y > 0) {
            player->hitbox.y -= player->hitbox.height;
            player->jump_count = 0;
            player->on_ground = true;
        } else
            player->hitbox.y += hit_wall->hitbox.height;

        player->speed.y = 0;
    }
}

void handle_player_attack() {
    Player *player = &game_state.player;

    // Init attack
    if (player->combat.attack_cooldown > 0)
        player->combat.attack_cooldown -= DELTA_TIME;
    else if (IsKeyPressed(KEY_X) && !(player->direction == DIR_DOWN && player->on_ground)) {
        player->combat.is_attacking = true;
        player->combat.attack_timer = PLAYER_ATTACK_TIMER;
        player->combat.attack_cooldown = PLAYER_ATTACK_COOLDOWN;
    }

    if (!player->combat.is_attacking) return;

    if (IsKeyDown(KEY_UP)) player->direction = DIR_UP;
    else if (IsKeyDown(KEY_DOWN)) player->direction = DIR_DOWN;
    // else keep current direction

    // Sword hitbox
    Rectangle *attack_box = &player->combat.attack_box;
    attack_box->x = player->hitbox.x;
    attack_box->y = player->hitbox.y;

    float sword_width = TILE_SIZE * 1.5f,
            sword_height = TILE_SIZE / 3.5f;

    if (player->direction == DIR_LEFT || player->direction == DIR_RIGHT) {
        attack_box->width = sword_width;
        attack_box->height = sword_height;
        attack_box->x += player->direction == DIR_RIGHT ? player->hitbox.width : -attack_box->width;
        attack_box->y += player->hitbox.height / 2.0f - attack_box->height / 2.0f;
    } else {
        attack_box->width = sword_height;
        attack_box->height = sword_width;
        attack_box->x += player->hitbox.width / 2.0f - attack_box->width / 2.0f;
        attack_box->y += player->direction == DIR_DOWN ? player->hitbox.height : -attack_box->height;
    }

    // Colission
    if (handle_collision_with_monster(player->combat.attack_box, player->combat.damage)
        || handle_collision_with_boss(player->combat.attack_box, player->combat.damage)) {
        add_player_souls(10);

        // Pogo
        if (player->direction == DIR_DOWN) {
            player->speed.y = -PLAYER_JUMP_SPEED * 0.7f;
            player->jump_count = 1;
        }
    }

    if (player->combat.attack_timer > 0)
        player->combat.attack_timer -= DELTA_TIME;
    else
        player->combat.is_attacking = false;
}

void handle_player_monsters_collision() {
    Player *player = &game_state.player;

    if (player->combat.invuln_timer > 0.0f)
        player->combat.invuln_timer -= DELTA_TIME;
    else
        player->combat.invulnerable = false;

    if (player->combat.hurt_timer > 0.0f)
        player->combat.hurt_timer -= DELTA_TIME;

    if (player->combat.invulnerable) return;

    Boss *hit_boss = handle_collision_with_boss(player->hitbox, 0);
    Monster *hit_monster = handle_collision_with_monster(player->hitbox, 0);

    if (hit_boss != NULL || hit_monster != NULL) {
        player->combat.life -= hit_boss != NULL ? hit_boss->damage : hit_monster->damage;

        player->combat.invulnerable = true;
        player->combat.invuln_timer = PLAYER_INVULN_TIMER;
        player->combat.hurt_timer = PLAYER_HURT_TIMER;

        // Knockback
        player->speed.x = 1.15f * (player->direction == DIR_LEFT ? PLAYER_SPEED : -PLAYER_SPEED);
        player->speed.y = -PLAYER_JUMP_SPEED;
        if (player->on_ground) {
            player->speed.y /= 2;
            player->on_ground = false;
        } else
            player->speed.y /= 4;
    }
}

void spawn_player_on_bench() {
    Player *player = &game_state.player;

    if (!player->has_spawn) return;

    player->hitbox = (Rectangle){
        player->spawn_pos.x, player->spawn_pos.y, player->hitbox.width, player->hitbox.height
    };
    player->speed.x = 0;
    player->speed.y = 0;
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

    player->combat.souls += +souls;
    if (player->combat.souls > player->combat.max_souls)
        player->combat.souls = player->combat.max_souls;
    else if (player->combat.souls < 0)
        player->combat.souls = 0;
}
