#include <minwindef.h>
#include "raylib.h"
#include "enemies.h"
#include "wall.h"
#include "player.h"

Monster monsters[MAX_MONSTERS];
int monsters_count = 0;

void add_monster(int x, int y) {
    if (monsters_count < MAX_MONSTERS) {
        monsters[monsters_count++] = (Monster){
            .speed = {LAND_MONSTER_HOR_SPEED, 0},
            .hitbox = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE},
            .direction = DIR_RIGHT,
            .is_active = true,
            .damage = MONSTER_DAMAGE
        };
    }
}

void draw_monsters() {
    float delta_time = GetFrameTime();

    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];

        if (!m->is_active) continue;

        Color color;

        if (m->hurt_timer > 0) {
            m->hurt_timer -= delta_time;
            color = WHITE;
        } else if (m->is_flying)
            color = ORANGE;
        else
            color = RED;

        DrawRectangleRec(m->hitbox, color);
    }
}


void handle_monster_variants() {
    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];

        if (!m->is_active) continue;

        Rectangle below_wall_check = {
            m->hitbox.x,
            m->hitbox.y + m->hitbox.height + 1,
            m->hitbox.width,
            2
        };

        if (handle_collision_with_walls(below_wall_check)) {
            m->is_flying = false;
            m->life = LAND_MONSTER_LIFE;
        } else {
            m->is_flying = true;
            m->life = FLYING_MONSTER_LIFE;
        }
    }
}

void update_monsters() {
    float delta_time = GetFrameTime();

    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];

        if (!m->is_active) continue;

        if (m->invuln_timer > 0)
            m->invuln_timer -= delta_time;
        else
            m->invulnerable = false;

        handle_monster_horizontal_movement(m);
        handle_monster_monsters_collision(m);
    }
}

void handle_monster_horizontal_movement(Monster *m) {
    float delta_time = GetFrameTime();

    m->hitbox.x += m->speed.x * (m->direction == DIR_RIGHT ? delta_time : -delta_time);

    Wall *hit_wall = handle_collision_with_walls(m->hitbox);

    if (hit_wall != NULL) {
        m->hitbox.x = hit_wall->hitbox.x;
        if (m->direction == DIR_RIGHT)
            m->hitbox.x -= m->hitbox.width;
        else
            m->hitbox.x += hit_wall->hitbox.width;

        toggle_monster_direction(m);
    }

    // Avoid falling
    if (!m->is_flying) {
        Rectangle floor_check = {
            m->hitbox.x + (m->direction == DIR_RIGHT ? TILE_SIZE : -TILE_SIZE),
            m->hitbox.y + TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE,
        };

        hit_wall = handle_collision_with_walls(floor_check);

        if (hit_wall == NULL)
            toggle_monster_direction(m);
    }
}

void handle_monster_monsters_collision(Monster *m) {
    Monster *hit_monster = handle_collision_with_monster(m->hitbox, 0);

    if (hit_monster != NULL) {
        toggle_monster_direction(m);
        toggle_monster_direction(hit_monster);

        // Avoid sticking
        m->hitbox.x += m->direction == DIR_RIGHT ? 2 : -2;
        hit_monster->hitbox.x += hit_monster->direction == DIR_RIGHT ? 2 : -2;
    }
}

Monster *handle_collision_with_monster(Rectangle hitbox, int damage) {
    Monster *hit_monster = NULL;

    for (int i = 0; i < monsters_count && hit_monster == NULL; i++) {
        Monster *m = &monsters[i];

        if (m->is_active && !m->invulnerable && CheckCollisionRecs(hitbox, m->hitbox)) {
            if (damage > 0) {
                m->life -= damage;
                handle_monster_death(&monsters[i]);

                m->invulnerable = true;
                m->invuln_timer = MONSTER_INVULN_TIMER;
                m->hurt_timer = MONSTER_HURT_TIMER;
            }

            hit_monster = m;
        }
    }

    return hit_monster;
}

void handle_monster_death(Monster *m) {
    if (m->life > 0) return;

    m->life = 0;
    m->is_active = false;

    add_player_money(GetRandomValue(35, 45));
}

void toggle_monster_direction(Monster *m) {
    m->direction = m->direction == DIR_RIGHT ? DIR_LEFT : DIR_RIGHT;
};
