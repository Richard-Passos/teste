#include "raylib.h"
#include "enemies.h"
#include "wall.h"
#include "config.h"

Monster monsters[MAX_MONSTERS];
int monsters_count = 0;

void add_monster(int x, int y, float z, float w) {
    if (monsters_count < MAX_MONSTERS) {
        monsters[monsters_count++] = (Monster){
            .speed = {z, w},
            .hitbox = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE},
            .direction = 1,
            .is_flying = true
        };
    }
}

bool check_collision_with_wall(Monster *m, Wall *wall) {
    return (
        m->hitbox.x < wall->hitbox.x + wall->hitbox.width &&
        m->hitbox.x + m->hitbox.width > wall->hitbox.x &&
        m->hitbox.y < wall->hitbox.y + wall->hitbox.height &&
        m->hitbox.y + m->hitbox.height > wall->hitbox.y
    );
}

void flying() {
    // Determinar se cada monstro é voador ou terrestre
    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];
        bool has_ground = false;
        m->life = 4;

        // Cria um retângulo logo abaixo do monstro
        Rectangle check = {
            m->hitbox.x,
            m->hitbox.y + m->hitbox.height + 1,
            m->hitbox.width,
            2
        };

        for (int j = 0; j < walls_count; j++) {
            if (CheckCollisionRecs(check, walls[j].hitbox)) {
                has_ground = true;
                m->life = 5;
                break;
            }
        }

        m->is_flying = !has_ground;
    }
}

void update_monsters(float delta) {
    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];

        // Atualiza timers de dano
        if (m->invulnerable) {
            m->invuln_time -= delta;
            if (m->invuln_time <= 0) {
                m->invulnerable = false;
            }
        }
    }

    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];

        // Reset do estado de chão; será setado true se colidir vindo de cima

        m->on_ground = false;

        // --- Movimento horizontal ---
        m->hitbox.x += m->direction * m->speed.x * delta;

        // --- Colisão horizontal (laterais) ---
        for (int j = 0; j < walls_count; j++) {
            Wall *wall = &walls[j];
            if (CheckCollisionRecs(m->hitbox, wall->hitbox)) {
                if (m->direction > 0) {
                    m->hitbox.x = wall->hitbox.x - m->hitbox.width;
                } else {
                    m->hitbox.x = wall->hitbox.x + m->hitbox.width;
                }

                m->direction *= -1;
                break;
            }
        }

        // --- Gravidade e movimento vertical ---
        if (!m->is_flying) {
            m->speed.y += GRAVITY * delta;
            m->hitbox.y += m->speed.y * delta;
        }


        // --- Colisão vertical (chão / teto) ---
        for (int j = 0; j < walls_count; j++) {
            Wall *wall = &walls[j];
            if (CheckCollisionRecs(m->hitbox, wall->hitbox)) {
                if (m->speed.y > 0) {
                    // caindo — encostou no chão
                    m->hitbox.y = wall->hitbox.y - m->hitbox.height;
                    m->on_ground = true; // agora está no chão
                } else if (m->speed.y < 0) {
                    // batendo por baixo (teto)
                    m->hitbox.y = wall->hitbox.y + wall->hitbox.height;
                }
                m->speed.y = 0;
                break;
            }
        }

        // Atualiza floor_checker (depois de mover e colidir)
        // posiciona na diagonal direita (ajuste offsets conforme sprite/tile)

        m->floor_checker.width = WALL_SIZE;
        m->floor_checker.height = WALL_SIZE;
        m->floor_checker.x = m->hitbox.x + m->direction * WALL_SIZE; // ligeiramente à direita
        m->floor_checker.y = m->hitbox.y + WALL_SIZE; // ligeiramente abaixo dos pés

        // Verifica chão à frente MAS só se estiver no chão
        if (m->on_ground) {
            bool tem_chao = false;
            for (int j = 0; j < walls_count; j++) {
                Wall *wall = &walls[j];
                if (CheckCollisionRecs(m->floor_checker, wall->hitbox)) {
                    tem_chao = true;
                    break;
                }
            }

            if (!tem_chao) {
                // vira antes de cair
                m->direction *= -1;
            }
        }

        // Colisão entre monstros
        for (int k = 0; k < monsters_count; k++) {
            Monster *a = &monsters[k];

            for (int j = k + 1; j < monsters_count; j++) {
                Monster *b = &monsters[j];

                if (CheckCollisionRecs(a->hitbox, b->hitbox)) {
                    // Ambos invertem direção
                    a->direction *= -1;
                    b->direction *= -1;

                    // Reposiciona levemente para evitar "grudar"
                    if (a->direction > 0)
                        a->hitbox.x += 2;
                    else
                        a->hitbox.x -= 2;

                    if (b->direction > 0)
                        b->hitbox.x += 2;
                    else
                        b->hitbox.x -= 2;
                }
            }
        }
    }
}


void draw_monsters() {
    for (int i = 0; i < monsters_count; i++) {
        Monster *m = &monsters[i];

        if (m->hurt_timer > 0) {
            m->hurt_timer -= GetFrameTime();
            DrawRectangleRec(m->hitbox, WHITE);
        } else if (m->is_flying) {
            DrawRectangleRec(m->hitbox, ORANGE);
        } else {
            DrawRectangleRec(m->hitbox, RED);
        }
    }
}
