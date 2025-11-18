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
        set_screen(paused);
      else if (IsKeyPressed(KEY_TAB))
        set_screen(inventory);
      else if (game_state.level > MAX_LEVEL)
        set_screen(win);
      else if (player->combat.life <= 0)
        set_screen(game_over);
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
