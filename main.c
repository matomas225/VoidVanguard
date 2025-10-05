#include "dieMenu.h"
#include "enemy.h"
#include "mainMenu.h"
#include "projectile.h"
#include "soundMenu.h"
#include "upgradeMenu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Save/load functions
void save_coins(int coins) {
  FILE *file = fopen("coins.txt", "w");
  if (file) {
    fprintf(file, "%d\n", coins);
    fclose(file);
  }
}

int load_coins() {
  FILE *file = fopen("coins.txt", "r");
  int coins = 0;
  if (file) {
    fscanf(file, "%d", &coins);
    fclose(file);
  }
  return coins;
}

// Forward declaration for draw_text function
void draw_text(SDL_Renderer *renderer, const char *text, float x, float y,
               SDL_Color color, float scale);

int main(int argc, char *argv[]) {
   // Set render scale quality
   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

   // Initialize game window and graphics
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
     printf("Error: Could not start SDL: %s\n", SDL_GetError());
     return -1;
   }

   // Initialize SDL_mixer
   if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
     printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
     return -1;
   }
   Mix_AllocateChannels(16); // Allow more simultaneous sound channels

#ifdef _WIN32
    // Get screen size for initial window size on Windows
    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);
    int win_w = dm.w;
    int win_h = dm.h;
    Uint32 win_flags = 0;
#else
    int win_w = 800;
    int win_h = 600;
    Uint32 win_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
#endif

    // Create game window
     SDL_Window *game_window = SDL_CreateWindow("Void Vanguard", SDL_WINDOWPOS_UNDEFINED,
                                               SDL_WINDOWPOS_UNDEFINED, win_w, win_h, win_flags);
   if (!game_window) {
     printf("Error: Could not create window\n");
     SDL_Quit();
     return -1;
   }

   // Create graphics renderer
   SDL_Renderer *graphics_renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED);
   if (!graphics_renderer) {
     printf("Error: Could not create renderer\n");
     SDL_DestroyWindow(game_window);
     SDL_Quit();
     return -1;
   }

  // Hide the mouse cursor
  SDL_ShowCursor(0);

  // Load sound effects with SDL_mixer
   Mix_Chunk *shoot_sound = Mix_LoadWAV("shoot.wav");
   if (!shoot_sound) {
     printf("Warning: Could not load shoot.wav: %s\n", Mix_GetError());
   } else {
      Mix_VolumeChunk(shoot_sound, 16); // Quieter shooting sound
   }

   Mix_Chunk *explode_sound = Mix_LoadWAV("enemy_explode.wav");
   if (!explode_sound) {
     printf("Warning: Could not load enemy_explode.wav: %s\n", Mix_GetError());
   } else {
      Mix_VolumeChunk(explode_sound, 128); // Louder explosion sound
   }

  Mix_Music *bg_music = Mix_LoadMUS("bg_music.wav");
  if (!bg_music) {
    printf("Warning: Could not load bg_music.wav: %s\n", Mix_GetError());
  }

   // Play background music (loop)
   if (bg_music) {
     Mix_PlayMusic(bg_music, -1);
     Mix_VolumeMusic(128); // Max volume for background music
   }

  // Game state variables
  int game_running = 1;
  SDL_Event current_event;
  int player_score = 0;
  int player_coins = load_coins();

  // Player information
  float player_x = 200.0f;
  float player_y = 100.0f;
  float player_width = 50.0f;
  float player_height = 50.0f;
  float player_speed = 5.0f;
  float player_health = 200.0f;
  int player_is_alive = 1;
  int key_up = 0, key_down = 0, key_left = 0, key_right = 0;

  // Enemy system setup
  EnemyManager enemies;
  initialize_enemy_manager(&enemies, 1000); // Room for 10 enemies

  // Create starting enemies
  add_enemy_to_manager(&enemies, 400.0f, 300.0f, 1, 0); // Middle
  add_enemy_to_manager(&enemies, 100.0f, 100.0f, 1, 0); // Top-left
  add_enemy_to_manager(&enemies, 600.0f, 400.0f, 1, 0); // Bottom-right
  add_enemy_to_manager(&enemies, 200.0f, 500.0f, 1, 0); // Bottom-left

  // Timing for smooth movement
  Uint64 last_frame_time = SDL_GetTicks();

  float enemy_spawn_timer = 0.0f;
  int enemies_spawned_count = 0;
  float total_play_time = 0.0f;

  // Add DieMenu after your existing variables
  DieMenu game_over_menu;
  initialize_die_menu(&game_over_menu);
  int restart_game = 0;
  int go_to_main_menu = 0;

    // Main menu
    MainMenu main_menu;
    initialize_main_menu(&main_menu);
    int start_game = 0;
    int show_upgrades = 0;
    int show_sound = 0;
   int is_paused = 0; // 1 if paused from game, 0 if from main menu
   int key_esc = 0;
   int key_esc_prev = 0;

   // Upgrades
   PlayerUpgrades player_upgrades;
   load_upgrades(&player_upgrades);
   UpgradeMenu upgrade_menu;
   initialize_upgrade_menu(&upgrade_menu);

   // Sound menu
   SoundMenu sound_menu;
   initialize_sound_menu(&sound_menu);

  // Projectile system
  Projectile projectiles[30];
  int projectile_count = 0;
  for (int i = 0; i < 30; i++)
    projectiles[i].alive = 0;

  // Enemy projectile system
  EnemyProjectile enemy_projectiles[50];
  int enemy_proj_count = 0;
  for (int i = 0; i < 50; i++)
    enemy_projectiles[i].alive = 0;

  // Mouse position
  float mouse_x = 400.0f;
  float mouse_y = 300.0f;

  // Window size for bounds
  int window_w = 800;
  int window_h = 600;

  // Star positions for background
  float star_x[100];
  float star_y[100];
  int prev_window_w = 800;
  int prev_window_h = 600;

  // Main game loop
  while (game_running) {
    // Calculate time since last frame
    Uint64 current_time = SDL_GetTicks();
    float frame_time = (current_time - last_frame_time) / 1000.0f;
    last_frame_time = current_time;

    // Update total play time
    total_play_time += frame_time;

    // Calculate difficulty level
    int difficulty_level = (int)(total_play_time / 30.0f);

      // Update window size
      SDL_GetWindowSize(game_window, &window_w, &window_h);
      // Ensure viewport covers the entire renderer
      SDL_RenderSetViewport(graphics_renderer, NULL);

    // Regenerate stars if window size changed
    if (window_w != prev_window_w || window_h != prev_window_h) {
      for (int i = 0; i < 100; i++) {
        star_x[i] = rand() % window_w;
        star_y[i] = rand() % window_h;
      }
      prev_window_w = window_w;
      prev_window_h = window_h;
    }

    // Handle input events
    while (SDL_PollEvent(&current_event)) {
      if (current_event.type == SDL_QUIT) {
        game_running = 0;
      }
        if (main_menu.is_active) {
          update_main_menu(&main_menu, &current_event, &game_running, &start_game,
                           &show_upgrades, &show_sound);
        } else if (upgrade_menu.is_active) {
          update_upgrade_menu(&upgrade_menu, &current_event, &show_upgrades,
                              &player_coins, &player_upgrades, &main_menu);
        } else if (sound_menu.is_active) {
          update_sound_menu(&sound_menu, &current_event, &show_sound);
        } else if (game_over_menu.is_active) {
          // Send events to die menu instead of game
          update_die_menu(&game_over_menu, &current_event, &game_running,
                          &restart_game, &go_to_main_menu);
        } else {
          if (current_event.type == SDL_KEYDOWN ||
              current_event.type == SDL_KEYUP) {
            int key_pressed = (current_event.type == SDL_KEYDOWN);

            switch (current_event.key.keysym.sym) {
            case SDLK_ESCAPE:
              key_esc = key_pressed;
              break;
            case SDLK_w:
             key_up = key_pressed;
             break;
           case SDLK_a:
             key_left = key_pressed;
             break;
           case SDLK_s:
             key_down = key_pressed;
             break;
           case SDLK_d:
             key_right = key_pressed;
             break;
           case SDLK_SPACE:
             if (key_pressed) {
               // Add new enemy at random position
               float random_x = (rand() % 700) + 50.0f;
               float random_y = (rand() % 500) + 50.0f;
               add_enemy_to_manager(&enemies, random_x, random_y, 1,
                                    difficulty_level);
               printf("New enemy added! Total enemies: %d\n",
                      enemies.current_enemy_count);
             }
             break;
           }
         }
         if (current_event.type == SDL_MOUSEMOTION) {
          mouse_x = current_event.motion.x;
          mouse_y = current_event.motion.y;
        }
        if (current_event.type == SDL_MOUSEBUTTONDOWN) {
          if (current_event.button.button == SDL_BUTTON_LEFT) {
            // Shoot projectile
            if (projectile_count < 30) {
              Projectile *p = &projectiles[projectile_count++];
              p->x = player_x + player_width / 2;
              p->y = player_y + player_height / 2;
              float dx = mouse_x - p->x;
              float dy = mouse_y - p->y;
              float dist = sqrtf(dx * dx + dy * dy);
              if (dist > 0) {
                p->vx = (dx / dist) * 500.0f;
                p->vy = (dy / dist) * 500.0f;
              } else {
                p->vx = 500.0f;
                p->vy = 0.0f;
              }
              p->alive = 1;
              // Double shots upgrade
              if (player_upgrades.double_shots && projectile_count < 30) {
                Projectile *p2 = &projectiles[projectile_count++];
                p2->x = p->x;
                p2->y = p->y;
                float angle = atan2f(dy, dx);
                float offset_angle = 0.1f; // Tighter spread ~6 degrees
                float new_angle = angle + offset_angle;
                p2->vx = cosf(new_angle) * 500.0f;
                p2->vy = sinf(new_angle) * 500.0f;
                p2->alive = 1;
              }
              // Triple shots upgrade
              if (player_upgrades.triple_shots && projectile_count < 30) {
                // Shoot two additional projectiles with wider spread
                for (int i = 0; i < 2 && projectile_count < 30; i++) {
                  Projectile *p_extra = &projectiles[projectile_count++];
                  p_extra->x = p->x;
                  p_extra->y = p->y;
                  float angle = atan2f(dy, dx);
                  float offset_angle =
                      (i == 0) ? -0.3f : 0.3f; // Left and right
                  float new_angle = angle + offset_angle;
                  p_extra->vx = cosf(new_angle) * 500.0f;
                  p_extra->vy = sinf(new_angle) * 500.0f;
                  p_extra->alive = 1;
                }
              }
            }
            // Play shoot sound
            if (shoot_sound) {
              Mix_PlayChannel(-1, shoot_sound, 0);
            }
          }
        }
      }
     }

   // Handle escape key press
   if (key_esc && !key_esc_prev) {
     if (game_over_menu.is_active) {
       game_running = 0;
     } else if (main_menu.is_active) {
       game_running = 0;
      } else if (upgrade_menu.is_active) {
        main_menu.is_active = 1;
        upgrade_menu.is_active = 0;
      } else if (sound_menu.is_active) {
        main_menu.is_active = 1;
        sound_menu.is_active = 0;
      } else {
       // Playing
       main_menu.is_active = 1;
       is_paused = 1;
     }
   }
   key_esc_prev = key_esc;

   // Check if we need to restart the game
    if (restart_game) {
      reset_game_state(&player_x, &player_y, &player_health, &player_is_alive,
                       &enemies, &player_score, 0);
      // Reset key states to prevent momentum carryover
      key_up = 0;
      key_down = 0;
      key_left = 0;
      key_right = 0;
      restart_game = 0;
      game_over_menu.is_active = 0; // Reset menu state
      // Reset projectiles and difficulty
      for (int i = 0; i < 30; i++)
        projectiles[i].alive = 0;
      projectile_count = 0;
      for (int i = 0; i < 50; i++)
        enemy_projectiles[i].alive = 0;
      enemy_proj_count = 0;
      total_play_time = 0.0f; // Reset difficulty on restart
      continue;               // Skip the rest of this frame
    }

    // Handle menu actions
    if (start_game) {
      start_game = 0;
      if (is_paused) {
        // Resume game
        main_menu.is_active = 0;
        upgrade_menu.is_active = 0;
        game_over_menu.is_active = 0;
        is_paused = 0;
      } else {
        // Start new game
        player_x = 200.0f;
        player_y = 100.0f;
        player_health = 200.0f;
        player_is_alive = 1;
        player_score = 0;
        player_coins = load_coins();
        cleanup_enemy_manager(&enemies);
        initialize_enemy_manager(&enemies, 1000);
        add_enemy_to_manager(&enemies, 400.0f, 300.0f, 1, 0);
        add_enemy_to_manager(&enemies, 100.0f, 100.0f, 1, 0);
        add_enemy_to_manager(&enemies, 600.0f, 400.0f, 1, 0);
        add_enemy_to_manager(&enemies, 200.0f, 500.0f, 1, 0);
        for (int i = 0; i < 30; i++) projectiles[i].alive = 0;
        projectile_count = 0;
        for (int i = 0; i < 50; i++) enemy_projectiles[i].alive = 0;
        enemy_proj_count = 0;
        total_play_time = 0.0f;
        enemy_spawn_timer = 0.0f;
        enemies_spawned_count = 0;
        main_menu.is_active = 0;
        upgrade_menu.is_active = 0;
        game_over_menu.is_active = 0;
        // Reset key states
        key_up = 0;
        key_down = 0;
        key_left = 0;
        key_right = 0;
        key_esc = 0;
        key_esc_prev = 0;
      }
    }
     if (show_upgrades) {
       upgrade_menu.is_active = 1;
       show_upgrades = 0;
     }
     if (show_sound) {
       sound_menu.is_active = 1;
       show_sound = 0;
     }
    if (go_to_main_menu) {
      go_to_main_menu = 0;
      main_menu.is_active = 1;
      game_over_menu.is_active = 0;
    }

    // Clear the screen with dynamic background color based on score (blue space
    // theme)
    int bg_r = 0, bg_g = 0, bg_b = 0;
    if (player_score >= 10000) {
      bg_b = 200; // Light blue
    } else if (player_score >= 8000) {
      bg_b = 150; // Medium blue
    } else if (player_score >= 6000) {
      bg_b = 100; // Dark blue
    } else if (player_score >= 4000) {
      bg_b = 50; // Very dark blue
    } else if (player_score >= 2000) {
      bg_b = 25; // Deep blue
    } // else black
    SDL_SetRenderDrawColor(graphics_renderer, bg_r, bg_g, bg_b, 255);
    SDL_RenderClear(graphics_renderer);

    // Draw stars
    SDL_SetRenderDrawColor(graphics_renderer, 255, 255, 255, 255);
    for (int i = 0; i < 100; i++) {
      SDL_Rect star_rect = {star_x[i], star_y[i], 2, 2};
      SDL_RenderFillRect(graphics_renderer, &star_rect);
    }

    if (main_menu.is_active) {
      draw_main_menu(&main_menu, graphics_renderer, window_w, window_h, is_paused);
     } else if (upgrade_menu.is_active) {
       draw_upgrade_menu(&upgrade_menu, graphics_renderer, window_w, window_h,
                         player_coins, &player_upgrades);
     } else if (sound_menu.is_active) {
       draw_sound_menu(&sound_menu, graphics_renderer, window_w, window_h);
     } else if (game_over_menu.is_active) {
      // Draw the game over menu
      draw_die_menu(&game_over_menu, graphics_renderer);
    } else {

      if (player_is_alive) {
        // NEW: Automatic enemy spawning over time (increasing difficulty)
        enemy_spawn_timer += frame_time;

        // Update difficulty based on play time (already calculated above)
        int current_max_enemies = 15 + difficulty_level * 5;
        float current_spawn_time = 3.0f - difficulty_level * 0.2f;
        if (current_spawn_time < 0.3f)
          current_spawn_time = 0.3f;

        // Count how many enemies are currently alive (not dead or exploding)
        int alive_enemies_count = 0;
        for (int i = 0; i < enemies.current_enemy_count; i++) {
          if (enemies.enemies_array[i].is_alive &&
              !enemies.enemies_array[i].is_exploding) {
            alive_enemies_count++;
          }
        }

        // Spawn new enemy if timer reached and we're under the limit
        if (enemy_spawn_timer >= current_spawn_time &&
            alive_enemies_count < current_max_enemies &&
            enemies.current_enemy_count < enemies.max_enemy_capacity) {

          // Find a spawn position away from player
          float spawn_x, spawn_y;
          int attempts = 0;
          int found_good_position = 0;

          while (attempts < 10 && !found_good_position) {
            // Try to spawn at edge of screen
            int side = rand() % 4; // 0=top, 1=right, 2=bottom, 3=left
            switch (side) {
            case 0: // Top
              spawn_x = (rand() % 700) + 50.0f;
              spawn_y = 20.0f;
              break;
            case 1: // Right
              spawn_x = 750.0f;
              spawn_y = (rand() % 500) + 50.0f;
              break;
            case 2: // Bottom
              spawn_x = (rand() % 700) + 50.0f;
              spawn_y = 550.0f;
              break;
            case 3: // Left
              spawn_x = 20.0f;
              spawn_y = (rand() % 500) + 50.0f;
              break;
            }

            // Check if spawn position is not too close to player
            float dx = spawn_x - player_x;
            float dy = spawn_y - player_y;
            float distance_to_player = sqrtf(dx * dx + dy * dy);

            if (distance_to_player >
                150.0f) { // At least 150 pixels from player
              found_good_position = 1;
            }

            attempts++;
          }

          // If no good position found, use random position
          if (!found_good_position) {
            spawn_x = (rand() % 700) + 50.0f;
            spawn_y = (rand() % 500) + 50.0f;
          }

          int enemy_type =
              (rand() % 3 == 0) ? 2 : 1; // 33% chance for purple enemy
          // Boss spawn chance after 5 minutes
          if (total_play_time > 300.0f && rand() % 20 == 0) {
            enemy_type = 3; // Boss
          }
          // Adjust spawn position based on window size
          if (spawn_x > window_w - 50)
            spawn_x = window_w - 50;
          if (spawn_y > window_h - 50)
            spawn_y = window_h - 50;
          add_enemy_to_manager(&enemies, spawn_x, spawn_y, enemy_type,
                               difficulty_level);
          enemies_spawned_count++;
          enemy_spawn_timer = 0.0f; // Reset timer

          printf(
              "Auto-spawn: Enemy #%d spawned at (%.0f, %.0f). Alive enemies: "
              "%d/%d (Difficulty: %d)\n",
              enemies_spawned_count, spawn_x, spawn_y, alive_enemies_count + 1,
              current_max_enemies, difficulty_level);
        }

        // Process player movement
        float move_x = 0.0f, move_y = 0.0f;

        if (key_up)
          move_y -= 1.0f;
        if (key_down)
          move_y += 1.0f;
        if (key_left)
          move_x -= 1.0f;
        if (key_right)
          move_x += 1.0f;

        // Fix diagonal movement speed
        if (move_x != 0.0f && move_y != 0.0f) {
          move_x *= 0.7071f;
          move_y *= 0.7071f;
        }

        // Update player position
        player_x += move_x * player_speed;
        player_y += move_y * player_speed;

        // Keep player within window bounds
        if (player_x < 0)
          player_x = 0;
        if (player_x + player_width > window_w)
          player_x = window_w - player_width;
        if (player_y < 0)
          player_y = 0;
        if (player_y + player_height > window_h)
          player_y = window_h - player_height;

        // Update enemy positions (they chase player)
        update_all_enemies(&enemies, player_x, player_y, frame_time, player_x,
                           player_y, player_width, player_height);

        // Check for boss death and spawn minions
        for (int i = 0; i < enemies.current_enemy_count; i++) {
          Enemy *e = &enemies.enemies_array[i];
          if (e->enemy_type == 3 && !e->is_alive && !e->has_spawned_minions) {
            // Spawn 5 fast small cube enemies
            for (int j = 0; j < 5; j++) {
              float minion_x = e->position_x + (rand() % 100) - 50;
              float minion_y = e->position_y + (rand() % 100) - 50;
              add_enemy_to_manager(&enemies, minion_x, minion_y, 4,
                                   difficulty_level);
            }
            e->has_spawned_minions = 1;
          }
        }

        // Remove dead enemies to free up array space
        cleanup_dead_enemies(&enemies);

        // Update projectiles
        update_player_projectiles(projectiles, &projectile_count, 30, &enemies,
                                  &player_score, window_w, window_h,
                                  enemy_projectiles, &enemy_proj_count, 50,
                                  frame_time, &player_upgrades, explode_sound);
        update_enemy_projectiles(enemy_projectiles, &enemy_proj_count, 50,
                                 player_x, player_y, player_width,
                                 player_height, &player_health, window_w,
                                 window_h, frame_time);

        // NEW: Handle collision damage between player and enemies
        float damage_taken = handle_player_enemy_collision_damage(
            &enemies, player_x, player_y, player_width, player_height,
            &player_score, explode_sound);
        player_health -= damage_taken;

        // Check for purple enemies that just died and spawn projectiles
        for (int j = 0; j < enemies.current_enemy_count; j++) {
          Enemy *e = &enemies.enemies_array[j];
          if (e->is_exploding && e->enemy_type == 2 &&
              !e->has_spawned_death_projectiles) {
            spawn_purple_enemy_death_projectiles(e, enemy_projectiles,
                                                 &enemy_proj_count, 50);
          }

        }
      }

      // Check if player died
      if (player_health <= 0) {
        player_health = 0;
        player_is_alive = 0;
        // Earn coins based on score
        int coins_earned = player_score / 10;
        player_coins += coins_earned;
        save_coins(player_coins);
        printf("Game Over! Earned %d coins. Total coins: %d\n", coins_earned,
               player_coins);
        game_over_menu.is_active = 1;
      }

      if (player_is_alive) {
        // Draw player as red square
        SDL_SetRenderDrawColor(graphics_renderer, 255, 0, 0, 255);
        SDL_Rect player_rect = {player_x, player_y, player_width,
                                 player_height};
        SDL_RenderFillRect(graphics_renderer, &player_rect);

         SDL_SetRenderDrawColor(graphics_renderer, 255, 0, 0, 255);
         SDL_Rect player_health_bar = {10, 10, player_health, 20};
         SDL_RenderFillRect(graphics_renderer, &player_health_bar);
      }

      // Draw score on the right side
      int window_width, window_height;
      SDL_GetRendererOutputSize(graphics_renderer, &window_width, &window_height);
      SDL_Color score_color = {255, 255, 255, 255}; // White
      char score_text[20];
      sprintf(score_text, "SCORE: %d", player_score);
       draw_text(graphics_renderer, score_text, window_width - 280, 10,
                 score_color, 2.0f);

      // Draw all enemies
      draw_all_enemies(&enemies, graphics_renderer);

      // Draw projectiles
      draw_player_projectiles(projectiles, projectile_count, graphics_renderer);
      draw_enemy_projectiles(enemy_projectiles, enemy_proj_count,
                             graphics_renderer);

      // Draw crosshair as smaller thicker circle
      SDL_SetRenderDrawColor(graphics_renderer, 255, 255, 255, 255); // White
      int radius = 8;
      for (int i = 0; i < 360; i += 1) {
        float angle1 = i * 3.14159f / 180.0f;
        float angle2 = (i + 1) * 3.14159f / 180.0f;
        int x1 = mouse_x + cosf(angle1) * radius;
        int y1 = mouse_y + sinf(angle1) * radius;
        int x2 = mouse_x + cosf(angle2) * radius;
        int y2 = mouse_y + sinf(angle2) * radius;
        // Draw thicker
        SDL_RenderDrawLine(graphics_renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(graphics_renderer, x1 + 1, y1, x2 + 1, y2);
        SDL_RenderDrawLine(graphics_renderer, x1 - 1, y1, x2 - 1, y2);
        SDL_RenderDrawLine(graphics_renderer, x1, y1 + 1, x2, y2 + 1);
        SDL_RenderDrawLine(graphics_renderer, x1, y1 - 1, x2, y2 - 1);
      }
    }

    // Show everything on screen
    SDL_RenderPresent(graphics_renderer);

    // Wait a bit (about 60 frames per second)
    SDL_Delay(16);
  }

  // Clean up memory
  cleanup_enemy_manager(&enemies);
  if (shoot_sound) Mix_FreeChunk(shoot_sound);
  if (explode_sound) Mix_FreeChunk(explode_sound);
  if (bg_music) Mix_FreeMusic(bg_music);
  Mix_CloseAudio();
  SDL_DestroyRenderer(graphics_renderer);
  SDL_DestroyWindow(game_window);
  SDL_Quit();

  return 0;
}
