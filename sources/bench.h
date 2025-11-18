//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_BENCH_H
#define HOLLOW_BENCH_H
#include "raylib.h"

#define MAX_BENCHES 99

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
} Bench;

void add_bench(int, int, Texture2D);

void draw_benches();

void handle_benches_interaction();

extern Bench benches[MAX_BENCHES];
extern int benches_count;

#endif //HOLLOW_BENCH_H
