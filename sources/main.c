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

  flying(walls);

  //--------------------------------------------------------------------------------------
  // Determinar se cada monstro é voador ou terrestre

  //--------------------------------------------------------------------------------------

  // Init player and camera
  //--------------------------------------------------------------------------------------
  Player player = {0};
  player.hitbox = player_start;
  player.speed = (Speed){0, 0};
  player.can_jump = false;
  player.is_attacking = false;
  player.attack_timer = 0.0f;
  player.on_ground = false;
  player.money = 0;
  player.souls = 0;
  player.abilitySoulProjectile.active = false;
  player.abilitySoulProjectile.acquired = false;
  player.combat.life = 5;
  player.combat.max_life = 5;
  player.combat.invulnerable = false;
  player.combat.invuln_timer = 0;
  player.combat.hurt_timer = 0;
  player.combat.heal_hold_needed = 1.0f;

  if (abilities_count > 0) {
    player.abilitySoulProjectile.hitbox = abilities[0].hitbox;
  }


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
      update_monsters(delta_time, walls, walls_count);
      //----------------------------------------------------------------------------------

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();
      ClearBackground(LIGHTGRAY);

      // Tudo dentro do modo 2D se move com a câmera
      BeginMode2D(camera);

      draw_map();
      draw_monsters();
      DrawAbilities();

      // Escolhe cor do player com base no estado de dano
      Color color_player = GREEN;

      if (player.combat.hurt_timer > 0) {
        // Piscar branco semi-transparente
        if (player.combat.hurt_timer>0)
          color_player = WHITE;
        else
          color_player = (Color){255, 255, 255, 90};
      }

      // Desenha o player com a cor de piscada
      DrawRectangleRec(player.hitbox, color_player);
      DrawHealingEffect(&player);

      if (player.is_attacking) {
        DrawRectangleRec(player.attack_box, (Color){0, 0, 0, 120});
      }

      if (player.abilitySoulProjectile.active) {
        DrawRectangleRec(player.abilitySoulProjectile.hitbox, WHITE);
      }

      UpdateAbilityAcquisition(&player);
      AbilitiesProjectile(&player, delta_time);

      EndMode2D();

      DrawHUD(player);

      EndDrawing();
      //----------------------------------------------------------------------------------

      // Actions
      //----------------------------------------------------------------------------------
      if (IsKeyPressed(KEY_ESCAPE))
        set_screen(paused);
      else if (IsKeyPressed(KEY_TAB))
        set_screen(inventory);
      //----------------------------------------------------------------------------------
    }
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadTexture(textures.Wall);
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
