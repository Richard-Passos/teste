#include "config.h"
#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "map.h"
#include "enemies.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, NAME);

  SetWindowIcon(LoadImage("../assets/apenas por agora.png"));
  Textures textures = {
    LoadTexture("../assets/wall.png"),
  };

  //--------------------------------------------------------------------------------------

  // Load map
  //--------------------------------------------------------------------------------------
  if (!load_map("../map.txt", textures)) {
    CloseWindow();

    return 1;
  }

  //--------------------------------------------------------------------------------------
  // Determinar se cada monstro é voador ou terrestre
  for (int i = 0; i < monsters_count; i++) {
    Monster *m = &monsters[i];
    bool has_ground = false;

    // Cria um retângulo logo abaixo do monstro
    Rectangle check = {
      m->hitbox.x,
      m->hitbox.y + m->hitbox.height + 1,
      m->hitbox.width,
      2
  };

    for (int j = 0; j < walls_count; j++) {
      if (CheckCollisionRecs(check, walls[j].hitbox)) {
        has_ground = true;
        break;
      }
    }

    m->is_flying = !has_ground;
  }

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
    float delta_time = GetFrameTime();

    // Update
    //----------------------------------------------------------------------------------
    update_player(&player, delta_time);
    update_camera_center(&camera, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
    update_monsters(GetFrameTime(), walls, walls_count);

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
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadTexture(textures.Wall);
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
