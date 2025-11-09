//
// Created by richa on 09/11/2025.
//

#include "raylib.h"
#include "timer.h"

double timer_start;

void start_timer() {
    timer_start = GetTime();
}

bool check_timer(double ms) {
    bool is_ok = false;

    if (timer_start > 0 && GetTime() - timer_start >= ms) {
        is_ok = true;
        timer_start = 0;
    }

    return is_ok;
}
