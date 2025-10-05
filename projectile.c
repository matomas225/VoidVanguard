#include "projectile.h"
#include "enemy.h"
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include <stdio.h>

// Update all player projectiles: move them, check for enemy hits, spawn death
// projectiles for purple enemies
void update_player_projectiles(Projectile *projectiles, int *count, int max,
                               EnemyManager *enemies, int *score, int window_w,
                               int window_h, EnemyProjectile *enemy_projectiles,
                               int *enemy_proj_count, int enemy_max,
                               float frame_time, PlayerUpgrades *upgrades,
                               Mix_Chunk *explode_sound) {
  for (int i = 0; i < *count; i++) {
    if (projectiles[i].alive) {
      projectiles[i].x += projectiles[i].vx * frame_time;
      projectiles[i].y += projectiles[i].vy * frame_time;

      // Check collision with enemies
      for (int j = 0; j < enemies->current_enemy_count; j++) {
        if (enemies->enemies_array[j].is_alive &&
            !enemies->enemies_array[j].is_exploding) {
          Enemy *e = &enemies->enemies_array[j];
          if (check_collision(projectiles[i].x, projectiles[i].y, 5, 5,
                              e->position_x, e->position_y, e->width,
                              e->height)) {
            e->health_points -= 10 + 5 * upgrades->damage_level;
             if (e->health_points <= 0) {
               e->is_exploding = 1;
               e->explosion_timer = 0.3f;
               if (explode_sound) Mix_PlayChannel(-1, explode_sound, 0);
               *score += 5;
              if (e->enemy_type == 2 && !e->has_spawned_death_projectiles) {
                spawn_purple_enemy_death_projectiles(
                    e, enemy_projectiles, enemy_proj_count, enemy_max);
              }
              printf("Enemy shot down! +5 points.\n");
            }
            projectiles[i].alive = 0;
            break;
          }
        }
      }

      // Remove if out of bounds
      if (projectiles[i].x < 0 || projectiles[i].x > window_w ||
          projectiles[i].y < 0 || projectiles[i].y > window_h) {
        projectiles[i].alive = 0;
      }
    }
  }

  // Compact array
  int new_count = 0;
  for (int i = 0; i < *count; i++) {
    if (projectiles[i].alive) {
      projectiles[new_count++] = projectiles[i];
    }
  }
  *count = new_count;
}

// Update all enemy projectiles: move them, check for player hits, remove if out
// of bounds
void update_enemy_projectiles(EnemyProjectile *projectiles, int *count, int max,
                              float player_x, float player_y, float player_w,
                              float player_h, float *player_health,
                              int window_w, int window_h, float frame_time) {
  for (int i = 0; i < *count; i++) {
    if (projectiles[i].alive) {
      projectiles[i].x += projectiles[i].vx * frame_time;
      projectiles[i].y += projectiles[i].vy * frame_time;

      // Check collision with player
      if (check_collision(projectiles[i].x, projectiles[i].y, 5, 5, player_x,
                          player_y, player_w, player_h)) {
        *player_health -= 15;
        projectiles[i].alive = 0;
        printf("Hit by enemy projectile! Health: %d\n", (int)*player_health);
      }

      // Remove if out of bounds
      if (projectiles[i].x < 0 || projectiles[i].x > window_w ||
          projectiles[i].y < 0 || projectiles[i].y > window_h) {
        projectiles[i].alive = 0;
      }
    }
  }

  // Compact array
  int new_count = 0;
  for (int i = 0; i < *count; i++) {
    if (projectiles[i].alive) {
      projectiles[new_count++] = projectiles[i];
    }
  }
  *count = new_count;
}

// Draw all active player projectiles
void draw_player_projectiles(Projectile *projectiles, int count,
                             SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
  for (int i = 0; i < count; i++) {
    if (projectiles[i].alive) {
      SDL_FRect proj_rect = {projectiles[i].x - 2.5f, projectiles[i].y - 2.5f,
                             5, 5};
      SDL_RenderFillRectF(renderer, &proj_rect);
    }
  }
}

// Draw all active enemy projectiles
void draw_enemy_projectiles(EnemyProjectile *projectiles, int count,
                            SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
  for (int i = 0; i < count; i++) {
    if (projectiles[i].alive) {
      SDL_FRect ep_rect = {projectiles[i].x - 2.5f, projectiles[i].y - 2.5f, 5,
                           5};
      SDL_RenderFillRectF(renderer, &ep_rect);
    }
  }
}

// Spawn 8 red projectiles in a circle around the dead purple enemy
void spawn_purple_enemy_death_projectiles(Enemy *e,
                                          EnemyProjectile *enemy_projectiles,
                                          int *enemy_proj_count,
                                          int enemy_max) {
  printf("Purple enemy killed! Spawning projectiles.\n");
  for (int k = 0; k < 8; k++) {
    if (*enemy_proj_count < enemy_max) {
      float angle = k * 3.14159f / 4.0f;
      EnemyProjectile *ep = &enemy_projectiles[(*enemy_proj_count)++];
      ep->x = e->position_x + e->width / 2;
      ep->y = e->position_y + e->height / 2;
      ep->vx = cosf(angle) * 300.0f;
      ep->vy = sinf(angle) * 300.0f;
      ep->alive = 1;
    }
  }
  e->has_spawned_death_projectiles = 1;
}
