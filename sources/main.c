#include <stdio.h>
#include "config.h"
#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "map.h"
#include "screen.h"
#include "enemies.h"
#include "HUD.h"
#include "abilities-attacks.h"
#include "game_state.h"
#include "boss.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, NAME);

  // Disable the Escape key from closing the window
  SetExitKey(KEY_NULL);

  SetWindowIcon(LoadImage("../assets/apenas por agora.png"));
  //--------------------------------------------------------------------------------------

  // Init player and camera
  //--------------------------------------------------------------------------------------
  Player *player = &game_state.player;
  init_camera((Vector2){player->hitbox.x, player->hitbox.y});
  //--------------------------------------------------------------------------------------

  SetTargetFPS(60);

  // Main game loop
  while (!WindowShouldClose()) {
    if (handle_screens() == 0) {
      // Load
      //--------------------------------------------------------------------------------------
      if (!load_map(MAP_FILE_PATH)) {
        CloseWindow();
        return 1;
      }
      //--------------------------------------------------------------------------------------

      // Update
      //----------------------------------------------------------------------------------
      float delta_time = GetFrameTime();

      update_player(delta_time);
      update_camera_center((Vector2){player->hitbox.x, player->hitbox.y});
      update_monsters(delta_time);
      update_boss(&game_state.player, delta_time, walls, walls_count);
      //----------------------------------------------------------------------------------

      // Update
      //----------------------------------------------------------------------------------
      draw_map();
      //----------------------------------------------------------------------------------

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

  // De-Initialization
  //--------------------------------------------------------------------------------------
  unload_map();
  CloseWindow();
  //--------------------------------------------------------------------------------------

  return 0;
}
