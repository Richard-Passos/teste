#include "enemies.h"
#include "raylib.h"


Monster monsters[MAX_MONSTERS];
int monsters_count = 0;



bool check_collision_with_wall(Monster *m, Wall *wall)
{
    return (
    m->hitbox.x < wall->hitbox.x + wall->hitbox.width &&
    m->hitbox.x + m->hitbox.width > wall->hitbox.x &&
    m->hitbox.y < wall->hitbox.y + wall->hitbox.height &&
    m->hitbox.y + m->hitbox.height > wall->hitbox.y
);
}

void update_monsters(float delta, Wall *walls, int walls_count)
{

    for (int i = 0; i < monsters_count; i++)
    {
        Monster *m = &monsters[i];

        // Reset do estado de chão; será setado true se colidir vindo de cima

            m->on_ground = false;

        // --- Movimento horizontal ---
        m->hitbox.x += m->direction * m->speed.x * delta;

        // --- Colisão horizontal (laterais) ---
        for (int j = 0; j < walls_count; j++)
        {
            Wall *wall = &walls[j];
            if (CheckCollisionRecs(m->hitbox, wall->hitbox))
            {
                if (m->direction > 0) {
                    m->hitbox.x = wall->hitbox.x - m->hitbox.width;
                }
                else {
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
        for (int j = 0; j < walls_count; j++)
        {
            Wall *wall = &walls[j];
            if (CheckCollisionRecs(m->hitbox, wall->hitbox))
            {
                if (m->speed.y > 0)
                {
                    // caindo — encostou no chão
                    m->hitbox.y = wall->hitbox.y - m->hitbox.height;
                    m->on_ground = true; // agora está no chão
                }
                else if (m->speed.y < 0)
                {
                    // batendo por baixo (teto)
                    m->hitbox.y = wall->hitbox.y + wall->hitbox.height;
                }
                m->speed.y = 0;
                break;
            }
        }

        // Atualiza floor_checker (depois de mover e colidir)
        // posiciona na diagonal direita (ajuste offsets conforme sprite/tile)

        m->floor_checker.width  = WALL_SIZE;
        m->floor_checker.height = WALL_SIZE;
        m->floor_checker.x = m->hitbox.x + m->direction*WALL_SIZE;   // ligeiramente à direita
        m->floor_checker.y = m->hitbox.y + WALL_SIZE;  // ligeiramente abaixo dos pés

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


void draw_monsters(void) {
    for (int i = 0; i < monsters_count; i++) {
        if (monsters[i].is_flying)
            DrawRectangleRec(monsters[i].hitbox, ORANGE);
        else
            DrawRectangleRec(monsters[i].hitbox, RED);

    }
}