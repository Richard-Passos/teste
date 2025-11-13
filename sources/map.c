#include <stdio.h>
#include "map.h"
#include "config.h"
#include "enemies.h"

Rectangle player_start = {0};
Rectangle boss_start = {0};

Wall walls[MAX_WALLS];
int walls_count = 0;

Item items[MAX_ITEMS];
int items_count = 0;

Ability abilities[MAX_ITEMS];
int abilities_count = 0;


void add_wall(int x, int y, Texture2D texture) {
    if (walls_count < MAX_WALLS) {
        walls[walls_count++] = (Wall){
            texture, {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
        };
    }
}

void add_monster(int x, int y, float z, float w) {
    if (monsters_count < MAX_MONSTERS) {
        monsters[monsters_count++] = (Monster){
            .speed = {z, w},
            .hitbox = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE},
            .direction = 1,
            .is_flying = true
        };
    }
}


void add_item(int x, int y) {
    if (items_count < MAX_ITEMS) {
        items[items_count++] = (Item){{x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}};
    }
}

void add_ability(int x, int y) {
    if (abilities_count < MAX_ITEMS) {
        abilities[abilities_count++] = (Ability){{x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}};
    }
}

int load_map(char path[], Textures textures) {
    walls_count = 0;
    monsters_count = 0;
    items_count = 0;
    abilities_count = 0;

    FILE *file = fopen(path, "r");
    if (!file) return 0;

    char line[MAX_MAP_COLS];
    int y = 0;

    while (fgets(line, sizeof(line), file)) {
        for (int x = 0; line[x] != '\0' && line[x] != '\n'; x++) {
            switch (line[x]) {
                case 'J':
                case 'j':
                    player_start = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
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
                    add_wall(x, y, textures.Wall);
                    break;
                default:
                    break;
            }
        }

        y++;
    }

    fclose(file);
    return 1;
}

void draw_map() {
    for (int i = 0; i < walls_count; i++)
        DrawTexture(walls[i].texture, walls[i].hitbox.x, walls[i].hitbox.y, WHITE);

    draw_monsters();

    for (int i = 0; i < items_count; i++)
        DrawRectangleRec(items[i].hitbox, GOLD);

    DrawRectangleRec(player_start, BLUE);
    DrawRectangleRec(boss_start, PURPLE);
}


