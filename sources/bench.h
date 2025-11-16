//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_BENCH_H
#define HOLLOW_BENCH_H
#include "raylib.h"

#define MAX_BENCHS 128

typedef struct {
    Texture2D texture;
    Rectangle hitbox;
} Bench;

void add_bench(int, int, Texture2D);

void draw_benchs();

extern Bench benchs[MAX_BENCHS];
extern int benchs_count;

#endif //HOLLOW_BENCH_H
