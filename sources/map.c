#include <stdio.h>
#include <string.h>
#include "map.h"
#include "config.h"
#include "wall.h"
#include "item.h"
#include "abilities.h"
#include "bench.h"
#include "enemies.h"
#include "game_state.h"
#include "HUD.h"
#include "camera.h"
#include "teleport.h"
#include "shop.h"
#include "boss.h"

char map_path[100];
char last_map_path[100];

Texture2D map_textures[99];
int map_textures_count = 0;
bool should_load_textures = true;

void add_texture(char texture_path[]) {
    map_textures[map_textures_count++] = LoadTexture(texture_path);
}

bool load_map(char path[]) {
    // Load just once
    //----------------------------------------------------------------------------------
    if (strcmp(path, last_map_path) == 0) return 1;

    strcpy(last_map_path, path);

    if (should_load_textures) {
        add_texture("../assets/wall.png");
        add_texture("../assets/bench.png");
        add_texture("../assets/shop.png");
        add_texture("../assets/npc.png");

        should_load_textures = false;
    }
    //----------------------------------------------------------------------------------

    // Reset
    //----------------------------------------------------------------------------------
    walls_count = 0;
    benches_count = 0;
    monsters_count = 0;
    teleports_count = 0;
    shop.is_active = false;
    shop_npc.is_active = false;
    unload_items();
    unload_abilities();

    boss.hitbox = (Rectangle){-1, -1, 0, 0};
    boss.active = false;
    boss.life = boss.max_life;
    //----------------------------------------------------------------------------------

    Texture wall_texture = map_textures[0],
            bench_texture = map_textures[1],
            shop_texture = map_textures[2],
            npc_texture = map_textures[3];

    FILE *file = fopen(path, "r");
    if (!file) return false;

    char line[MAX_MAP_COLS];
    int row = 0;

    while (fgets(line, sizeof(line), file)) {
        for (int col = 0; col < strlen(line); col++) {
            switch (line[col]) {
                case 'J':
                case 'j':
                    add_player(col, row);
                    break;
                case 'C':
                case 'c':
                    add_boss(col, row);
                    break;
                case 'M':
                case 'm':
                    add_monster(col, row, LAND_MONSTER_HOR_SPEED, 0.0f);
                    break;
                case 'A':
                case 'a':
                    add_item(col, row);
                    break;
                case 'H':
                case 'h':
                    add_ability(col, row);
                    break;
                case 'P':
                case 'p':
                    add_wall(col, row, wall_texture);
                    break;
                case 'B':
                case 'b':
                    add_bench(col, row, bench_texture);
                    break;
                case 'T':
                case 't':
                    add_teleport(col, row);
                    break;
                case 'S':
                case 's':
                    add_shop(col, row, shop_texture);
                    break;
                case 'N':
                case 'n':
                    add_shop_npc(col, row, npc_texture);
                    break;
                default:
                    break;
            }
        }

        row++;
    }

    fclose(file);

    // ==========================================================
    //     SOMENTE SPAWNAR SE REALMENTE HOUVER UM CHEFE NO MAPA
    // ==========================================================
    if (boss.hitbox.x != -1 && boss.hitbox.y != -1) {
        spawn_boss();
    } else {
        boss.active = false;
    }

    flying(); // monstros voadores ou terrestres

    return true;
}

void unload_map() {
    for (int i = 0; i < map_textures_count; i++) {
        UnloadTexture(map_textures[i]);
    }

    strcpy(last_map_path, "");
    should_load_textures = true;
}

void handle_map() {
    update_map();
    draw_map();
}

void draw_map() {
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    BeginMode2D(camera); // Tudo dentro do modo 2D se move com a câmera
    draw_walls();
    draw_items();
    draw_benches();
    draw_teleports();
    draw_shop();
    draw_shop_npc();
    draw_monsters();
    draw_boss();
    draw_abilities();
    draw_player();
    draw_healing_effect();
    EndMode2D();

    draw_hud();
    EndDrawing();
}

void update_map() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    update_player();
    update_camera_center((Vector2){player->hitbox.x, player->hitbox.y});
    update_monsters();
    update_boss(&game_state.player, delta_time, walls, walls_count);
}

void set_map_path(int level) {
    sprintf(map_path, "%s%d.txt", MAP_DEFAULT_PATH, level);
}
