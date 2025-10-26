#include "raylib.h"
#include "player.h"
#include "config.h"
#include "update_player.h"
#include "update_camera_center.h"
#include "parede.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, NAME);
  SetWindowIcon(LoadImage("../assets/apenas por agora.png"));


  Player player = {0};
  player.position = (Vector2){400, 280};
  player.hitbox = (Rectangle){ player.position.x, player.position.y, 32, 32};
  player.speed = 0;
  player.canJump = false;

  wall env_wall[] = {
    {
      LoadTexture("../assets/wall.png"), // textura
      {400, 400, 32, 32},              // retângulo (x, y, width, height)
      1                               // bloqueia passagem
    },
    {
      LoadTexture("../assets/wall.png"), // textura
      {368, 400, 32, 32},
      1
    },
  {
  LoadTexture("../assets/wall.png"),
  {336, 400, 32, 32},
  1
   },
  {
  LoadTexture("../assets/wall.png"),
  {336, 368, 32, 32},
   1
}
  };

  int env_wall_length = sizeof(env_wall) / sizeof(env_wall[0]);



  Camera2D camera = {0};
  camera.target = player.position;
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())
  {
    float delta_time = GetFrameTime();


    // Update
    //----------------------------------------------------------------------------------

    update_player(&player, env_wall, env_wall_length, delta_time);
    update_camera_center(&camera, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    BeginMode2D(camera);

    for (int i = 0; i < env_wall_length; i++)
    {
      DrawTexture(env_wall[i].texture, env_wall[i].hitbox.x, env_wall[i].hitbox.y, WHITE);
    }

    DrawRectangleRec(player.hitbox, RED);



    EndMode2D();
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}