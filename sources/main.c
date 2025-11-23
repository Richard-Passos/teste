#include <stdio.h>
#include "config.h"
#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "map.h"
#include "screen.h"
#include "abilities.h"
#include "game_state.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, NAME);
  SetExitKey(KEY_NULL);
  SetWindowIcon(LoadImage("../assets/apenas por agora.png"));

  Player *player = &game_state.player;
  init_camera((Vector2){player->hitbox.x, player->hitbox.y});

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (handle_screens() == 0) {
      if (!load_map(map_path)) {
        CloseWindow();
        return 1;
      }

      handle_map();

      // Actions
      //----------------------------------------------------------------------------------
      if (IsKeyPressed(KEY_ESCAPE))
        set_screen(SCREEN_PAUSED);
      else if (IsKeyPressed(KEY_TAB))
        set_screen(SCREEN_INVENTORY);
      else if (game_state.level > MAX_LEVEL)
        set_screen(SCREEN_WIN);
      else if (player->combat.life <= 0)
        set_screen(SCREEN_GAME_OVER);
      //----------------------------------------------------------------------------------
    }
  }

  unload_map();
  CloseWindow();

  return 0;
}
