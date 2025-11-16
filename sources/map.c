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

char map_path[100];

Texture2D map_textures[99];
int map_textures_count = 0;
bool should_load_textues = true;

Rectangle boss_start = {0};

void add_texture(char texture_path[]) {
    map_textures[map_textures_count++] = LoadTexture(texture_path);
}

int load_map(char path[]) {
    // Load just once
    //----------------------------------------------------------------------------------
    if (strcmp(path, map_path) == 0) return 1;

    strcpy(map_path, path);

    if (should_load_textues) {
        add_texture("../assets/wall.png");
        add_texture("../assets/wall.png");

        should_load_textues = false;
    }
    //----------------------------------------------------------------------------------

    Texture wall_texture = map_textures[0],
            bench_texture = map_textures[1];

    walls_count = 0;
    items_count = 0;
    benchs_count = 0;
    abilities_count = 0;
    monsters_count = 0;
    teleports_count = 0;
    shop_hitbox = (Rectangle){0};
    boss_start = (Rectangle){0};

    FILE *file = fopen(path, "r");
    if (!file) return 0;

    char line[MAX_MAP_COLS];
    int y = 0;

    while (fgets(line, sizeof(line), file)) {
        for (int x = 0; line[x] != '\0' && line[x] != '\n'; x++) {
            switch (line[x]) {
                case 'J':
                case 'j':
                    game_state.player.hitbox = (Rectangle){
                        x * TILE_SIZE, y * TILE_SIZE, game_state.player.hitbox.width, game_state.player.hitbox.height
                    };
                    break;
                case 'C':
                case 'c':
                    boss_start = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
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
                    shop_hitbox = (Rectangle){
                        x * TILE_SIZE, y * TILE_SIZE - TILE_SIZE * 2, TILE_SIZE * 3, TILE_SIZE * 3
                    };
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

    flying(); // Set each enemy as flying or grounded

    return 1;
}

void unload_map() {
    for (int i = 0; i < map_textures_count; i++) {
        UnloadTexture(map_textures[i]);
    }

    strcpy(map_path, "");
    should_load_textues = true;
}

void draw_map() {
    Player *player = &game_state.player;
    float delta_time = GetFrameTime();

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    // Tudo dentro do modo 2D se move com a câmera
    BeginMode2D(camera);

    draw_walls();
    draw_items();
    draw_benchs();
    draw_teleports();
    draw_shop();
    draw_monsters();
    draw_abilities();

    // ---- DESENHAR "INSPECIONAR" ----
    if (!player->abilitySoulProjectile.acquired &&
        CheckCollisionRecs(player->hitbox, player->abilitySoulProjectile.hitbox)) {

        DrawText(
            "Inspecionar",
            player->abilitySoulProjectile.hitbox.x - 50,
            player->abilitySoulProjectile.hitbox.y - 50,
            30,
            WHITE
        );
    }

    if (!player->is_sitting) {
        for (int i = 0; i < benchs_count; i++) {
            if (CheckCollisionRecs(player->hitbox, benchs[i].hitbox)) {
                DrawText("Descansar",
                         benchs[i].hitbox.x - 50,
                         benchs[i].hitbox.y - 50,
                         32, WHITE);
                break; // só desenhar para o banco que está colidindo
            }
        }
    }

    if (CheckCollisionRecs(player->hitbox, shop_hitbox)) {
        DrawText("Entrar",
                 shop_hitbox.x + 40,
                 shop_hitbox.y - 50,
                 32,
                 WHITE);
    }

    draw_player();
    draw_healing_effect();

    if (player->is_attacking) {
        DrawRectangleRec(player->attack_box, GRAY);
    }
    if (player->abilitySoulProjectile.active) {
        DrawRectangleRec(player->abilitySoulProjectile.hitbox, WHITE);
    }
    EndMode2D();
    draw_hud();

    EndDrawing();
    //----------------------------------------------------------------------------------
}


