#include <stdio.h>
#include "config.h"
#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "map.h"
#include "screen.h"
#include "enemies.h"

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
  Textures textures = {
    LoadTexture("../assets/wall.png"),
  };

  //--------------------------------------------------------------------------------------

  // Load map
  //--------------------------------------------------------------------------------------
  if (!load_map(MAP_FILE_PATH, textures)) {
    CloseWindow();

    return 1;
  }

  //--------------------------------------------------------------------------------------
  // Determinar se cada monstro é voador ou terrestre

  //--------------------------------------------------------------------------------------

  // Init player and camera
  //--------------------------------------------------------------------------------------
  Player player = {0};
  player.hitbox = player_start;
  player.speed = (Speed){0, 0};
  player.can_jump = false;

  Camera2D camera = {0};
  camera.target = (Vector2){player.hitbox.x, player.hitbox.y};
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  //--------------------------------------------------------------------------------------

  SetTargetFPS(60);

  // Main game loop
  while (!WindowShouldClose()) {
    if (handle_screens() == 0) {
      float delta_time = GetFrameTime();

      // Update
      //----------------------------------------------------------------------------------
      update_player(&player, delta_time);
      update_camera_center(&camera, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
      flying(walls);
      update_monsters(delta_time, walls, walls_count);
      //----------------------------------------------------------------------------------

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();
      ClearBackground(LIGHTGRAY);
      BeginMode2D(camera);

      draw_map();
      DrawRectangleRec(player.hitbox, GREEN);

      EndMode2D();
      EndDrawing();
      //----------------------------------------------------------------------------------

      if (IsKeyPressed(KEY_ESCAPE))
        set_screen('p');
      else if (IsKeyPressed(KEY_TAB))
        set_screen('i');
    }
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadTexture(textures.Wall);
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
