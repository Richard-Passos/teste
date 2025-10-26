//
// Created by Harry on 24/10/2025.
//

#ifndef MY_RAYLIB_GAME_WALL
#define MY_RAYLIB_GAME_WALL

#include "raylib.h"

typedef struct {
    Texture2D texture;  // imagem da parede
    Rectangle hitbox;   // área sólida (para colisão)
    int blocking;         // se for sólida, o player não atravessa
} wall;

#endif //MY_RAYLIB_GAME_WALL