#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
  float damage_to_player;
  float position_x, position_y;
  float width, height;
  float movement_speed;
  int health_points;
  int max_health;
  int is_alive;
  int enemy_type;
  int collision_count;               // Track how many times hit by player
  int is_exploding;                  // Is enemy currently exploding?
  float explosion_timer;             // Timer for explosion animation
  int has_spawned_death_projectiles; // For purple enemies
  int has_spawned_minions;           // For boss enemies
} Enemy;

typedef struct {
  Enemy *enemies_array;
  int current_enemy_count;
  int max_enemy_capacity;
} EnemyManager;

// Function declarations
void initialize_enemy_manager(EnemyManager *manager, int max_capacity);
void add_enemy_to_manager(EnemyManager *manager, float start_x, float start_y,
                          int enemy_type, int difficulty_level);
void update_single_enemy(Enemy *enemy, float target_x, float target_y,
                         float time_since_last_frame, EnemyManager *manager,
                         int enemy_index, float player_x, float player_y,
                         float player_w, float player_h);
void draw_single_enemy(Enemy *enemy, SDL_Renderer *renderer);
void update_all_enemies(EnemyManager *manager, float target_x, float target_y,
                        float time_since_last_frame, float player_x,
                        float player_y, float player_w, float player_h);
void draw_all_enemies(EnemyManager *manager, SDL_Renderer *renderer);
void cleanup_enemy_manager(EnemyManager *manager);

// Utility functions
int check_collision(float x1, float y1, float w1, float h1, float x2, float y2,
                    float w2, float h2);

// New functions for collision damage and explosions
float handle_player_enemy_collision_damage(EnemyManager *manager,
                                           float player_x, float player_y,
                                           float player_w, float player_h,
                                           int *score, Mix_Chunk *explode_sound);
void update_explosions(EnemyManager *manager, float time_since_last_frame);
void cleanup_dead_enemies(EnemyManager *manager);

#endif
