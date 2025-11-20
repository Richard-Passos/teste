#include <stdio.h>
#include <string.h>
#include "map.h"
#include "config.h"
#include "wall.h"
#include "item.h"
#include "abilities-attacks.h"
#include "bench.h"
#include "enemies.h"
#include "game_state.h"
#include "HUD.h"
#include "camera.h"
#include "teleport.h"
#include "shop.h"
#include "boss.h"

char map_path[100];

Texture2D map_textures[99];
int map_textures_count = 0;
bool should_load_textures = true;

void add_texture(char texture_path[]) {
    map_textures[map_textures_count++] = LoadTexture(texture_path);
}

bool load_map(char path[]) {
    // Load just once
    //----------------------------------------------------------------------------------
    if (strcmp(path, map_path) == 0) return 1;

    strcpy(map_path, path);

    if (should_load_textures) {
        add_texture("../assets/wall.png");
        add_texture("../assets/wall.png");

        should_load_textures = false;
    }
    //----------------------------------------------------------------------------------

    // Textures
    //----------------------------------------------------------------------------------
    walls_count = 0;
    items_count = 0;
    benches_count = 0;
    abilities_count = 0;
    monsters_count = 0;
    teleports_count = 0;
    shop.should_draw = false;
    // <<< ESSENCIAL: marca como *inexistente*
    boss.hitbox = (Rectangle){-1, -1, 0, 0};
    boss.active = false; // <<< Boss começa desligado sempre
    boss.life = boss.max_life;
    //----------------------------------------------------------------------------------

    // Textures
    //----------------------------------------------------------------------------------
    Texture wall_texture = map_textures[0],
            bench_texture = map_textures[1];
    //----------------------------------------------------------------------------------

    FILE *file = fopen(path, "r");
    if (!file) return false;

    char line[MAX_MAP_COLS];
    int y = 0;

    while (fgets(line, sizeof(line), file)) {
        for (int x = 0; line[x] != '\0' && line[x] != '\n'; x++) {
            switch (line[x]) {
                case 'J':
                case 'j':
                    add_player(x, y);
                    break;
                case 'C':
                case 'c':
                    add_boss(x, y);
                    break;
                case 'M':
                case 'm':
                    add_monster(x, y, LAND_MONSTER_HOR_SPEED, 0.0f);
                    break;
                case 'A':
                case 'a':
                    add_item(x, y);
                    break;
                case 'H':
                case 'h':
                    add_ability(x, y);
                    break;
                case 'P':
                case 'p':
                    add_wall(x, y, wall_texture);
                    break;
                case 'B':
                case 'b':
                    add_bench(x, y, bench_texture);
                    break;
                case 'S':
                case 's':
                    add_shop(x, y);
                    break;
                case 'T':
                case 't':
                    add_teleport(x, y);
                    break;
                default:
                    break;
            }
        }

        y++;
    }

    fclose(file);

    // ==========================================================
    //     SOMENTE SPAWNAR SE REALMENTE HOUVER UM CHEFE NO MAPA
    // ==========================================================
    if (boss.hitbox.x != -1 && boss.hitbox.y != -1) {
        spawn_boss();
    } else {
        boss.active = false; // Garantia
    }

    flying(); // monstros voadores ou terrestres

    return true;
}

void unload_map() {
    for (int i = 0; i < map_textures_count; i++) {
        UnloadTexture(map_textures[i]);
    }

    strcpy(map_path, "");
    should_load_textures = true;
}

void draw_map() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    BeginMode2D(camera); // Tudo dentro do modo 2D se move com a câmera

    draw_walls();
    draw_items();
    draw_benches();
    draw_teleports();
    draw_shop();
    draw_monsters();
    draw_boss();
    draw_abilities();
    draw_player();
    draw_healing_effect();

    // Actions
    //----------------------------------------------------------------------------------
    if (player->is_attacking)
        DrawRectangleRec(player->attack_box, GRAY);
    if (player->abilitySoulProjectile.active)
        DrawRectangleRec(player->abilitySoulProjectile.hitbox, WHITE);
    //----------------------------------------------------------------------------------
    EndMode2D();
    draw_hud();
    EndDrawing();
    //----------------------------------------------------------------------------------
}
