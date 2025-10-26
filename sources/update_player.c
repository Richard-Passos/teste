#include "raylib.h"
#include "player.h"
#include "config.h"
#include "parede.h"

bool has_hit_obstacle(wall *w, Vector2 *pos, float delta, float speed)
{
    return (
            w->hitbox.x-32 <= pos->x &&
            w->hitbox.x-32 + w->hitbox.width >= pos->x &&
            w->hitbox.y-32 >= pos->y &&
            w->hitbox.y-32 <= pos->y + speed * delta);
}

/*
bool has_hit_side(wall *w, Player *player)
{
    return (
        player->position.x + player->hitbox.width >= w->hitbox.x &&  // lado direito do player tocando a parede
        player->position.x <= w->hitbox.x + w->hitbox.width && // lado esquerdo tocando
        player->position.y + player->hitbox.height >= w->hitbox.y && // sobreposição vertical
        player->position.y <= w->hitbox.y + w->hitbox.height
    );
}
*/

void update_player(Player *player, wall *env_wall, int env_wall_length, float delta) {
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        player->position.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        player->position.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }


    bool hit_obstacle = false;
    //bool hit_side = true;

    for (int i = 0; i < +env_wall_length; i++)
    {
        wall *w = env_wall + i;
        Vector2 *pos = &(player->position);
        if (has_hit_obstacle(w, pos, delta, player->speed))
        {
            hit_obstacle = true;
            player->speed = 0.0f;
            pos->y = w->hitbox.y-32;
            break;
        }
    }
    /*
        for (int i = 0; i < +env_wall_length; i++)
        {
            wall *w = env_wall + i;
            Vector2 *pos = &(player->position);
            if (has_hit_side(env_wall ,player))
            {
                hit_side = true;
                player->speed = 0.0f;
                pos->x = w->hitbox.x;
                break;
            }
        }*/

    if (!hit_obstacle)
    {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        player->canJump = false;
    }
    else
        player->canJump = true;

    player->hitbox = (Rectangle){.x = player->position.x, .y = player->position.y, 32, 32};
}