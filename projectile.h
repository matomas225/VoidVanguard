#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "enemy.h"
#include "upgrades.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// Player's projectile struct
typedef struct {
  float x, y, vx, vy; // Position and velocity
  int alive;          // Whether the projectile is active
} Projectile;

// Enemy's projectile struct (e.g., from purple enemies)
typedef struct {
  float x, y, vx, vy; // Position and velocity
  int alive;          // Whether the projectile is active
} EnemyProjectile;

// Function declarations

// Update player projectiles: move, check collisions with enemies, handle enemy
// deaths
void update_player_projectiles(Projectile *projectiles, int *count, int max,
                               EnemyManager *enemies, int *score, int window_w,
                               int window_h, EnemyProjectile *enemy_projectiles,
                               int *enemy_proj_count, int enemy_max,
                               float frame_time, PlayerUpgrades *upgrades,
                               Mix_Chunk *explode_sound);

// Update enemy projectiles: move, check collisions with player, remove out of
// bounds
void update_enemy_projectiles(EnemyProjectile *projectiles, int *count, int max,
                              float player_x, float player_y, float player_w,
                              float player_h, float *player_health,
                              int window_w, int window_h, float frame_time);

// Draw player projectiles as yellow squares
void draw_player_projectiles(Projectile *projectiles, int count,
                             SDL_Renderer *renderer);

// Draw enemy projectiles as red squares
void draw_enemy_projectiles(EnemyProjectile *projectiles, int count,
                            SDL_Renderer *renderer);

// Spawn 8 projectiles in a circle when a purple enemy dies
void spawn_purple_enemy_death_projectiles(Enemy *e,
                                          EnemyProjectile *enemy_projectiles,
                                          int *enemy_proj_count, int enemy_max);

#endif
