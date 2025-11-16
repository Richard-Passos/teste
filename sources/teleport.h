//
// Created by richa on 16/11/2025.
//

#ifndef HOLLOW_TELEPORT_H
#define HOLLOW_TELEPORT_H
#include "raylib.h"

#define MAX_TELEPORTS 99

typedef Rectangle Teleport;

void add_teleport(int, int);

void draw_teleports();

bool handle_teleports_interaction();

extern Teleport teleports[MAX_TELEPORTS];
extern int teleports_count;

#endif //HOLLOW_TELEPORT_H
