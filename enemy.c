#include "enemy.h"
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_enemy_manager(EnemyManager *manager, int max_capacity) {
  manager->enemies_array = malloc(sizeof(Enemy) * max_capacity);
  manager->current_enemy_count = 0;
  manager->max_enemy_capacity = max_capacity;
}

void add_enemy_to_manager(EnemyManager *manager, float start_x, float start_y,
                          int enemy_type, int difficulty_level) {
  if (manager->current_enemy_count >= manager->max_enemy_capacity)
    return;

  Enemy *new_enemy = &manager->enemies_array[manager->current_enemy_count];
  new_enemy->position_x = start_x;
  new_enemy->position_y = start_y;
  new_enemy->is_alive = 1;
  new_enemy->enemy_type = enemy_type;
  new_enemy->is_exploding = 0;
  new_enemy->explosion_timer = 0.0f;
  new_enemy->damage_to_player = 10.0f;
  new_enemy->has_spawned_death_projectiles = 0;
  new_enemy->has_spawned_minions = 0;

  // Scale based on difficulty
  if (enemy_type == 3) { // Boss
    new_enemy->width = 100.0f;
    new_enemy->height = 100.0f;
    new_enemy->movement_speed = 50.0f;
    new_enemy->max_health = 200 + 50 * difficulty_level;
  } else if (enemy_type == 4) { // Minions
    new_enemy->width = 25.0f;
    new_enemy->height = 25.0f;
    new_enemy->movement_speed = 350.0f;
    new_enemy->max_health = 10;
  } else { // Normal and purple
    new_enemy->width = 40.0f;
    new_enemy->height = 40.0f;
    new_enemy->movement_speed = 150.0f + difficulty_level * 10.0f;
    if (new_enemy->movement_speed > 300.0f) new_enemy->movement_speed = 300.0f;
    new_enemy->max_health = 30 + difficulty_level * 10;
  }

  new_enemy->health_points = new_enemy->max_health;

  manager->current_enemy_count++;
}

// Check if two rectangles are overlapping
int check_collision(float x1, float y1, float w1, float h1, float x2, float y2,
                    float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// Handle damage when player collides with enemies
float handle_player_enemy_collision_damage(EnemyManager *manager,
                                           float player_x, float player_y,
                                           float player_w, float player_h,
                                           int *score, Mix_Chunk *explode_sound) {
  float total_damage_taken = 0.0f;
  for (int i = 0; i < manager->current_enemy_count; i++) {
    if (!manager->enemies_array[i].is_alive ||
        manager->enemies_array[i].is_exploding)
      continue;

    Enemy *enemy = &manager->enemies_array[i];

    if (check_collision(player_x, player_y, player_w, player_h,
                        enemy->position_x, enemy->position_y, enemy->width,
                        enemy->height)) {
      // Player takes damage
      total_damage_taken += 15.0f;
       // Enemy dies
       enemy->is_exploding = 1;
       enemy->explosion_timer = 0.3f;
       if (explode_sound) Mix_PlayChannel(-1, explode_sound, 0);
       *score += 5;
      printf("Player collided with enemy! Took 15 damage.\n");
    }
  }
  return total_damage_taken;
}

// Update explosion animations
void update_explosions(EnemyManager *manager, float time_since_last_frame) {
  for (int i = 0; i < manager->current_enemy_count; i++) {
    if (manager->enemies_array[i].is_exploding) {
      manager->enemies_array[i].explosion_timer -= time_since_last_frame;

      // When explosion finishes, mark enemy as dead
      if (manager->enemies_array[i].explosion_timer <= 0) {
        manager->enemies_array[i].is_alive = 0;
        manager->enemies_array[i].is_exploding = 0;
        printf("Enemy removed from game.\n");
      }
    }
  }
}

// Remove dead enemies from the array to free up space
void cleanup_dead_enemies(EnemyManager *manager) {
  int write_index = 0;
  for (int i = 0; i < manager->current_enemy_count; i++) {
    if (manager->enemies_array[i].is_alive) {
      // Keep alive enemies
      if (write_index != i) {
        manager->enemies_array[write_index] = manager->enemies_array[i];
      }
      write_index++;
    }
  }
  manager->current_enemy_count = write_index;
}

// Check if moving to a new position would cause collision
int would_collide(Enemy *enemy, float new_x, float new_y, EnemyManager *manager,
                  int ignore_index) {
  for (int i = 0; i < manager->current_enemy_count; i++) {
    if (i == ignore_index)
      continue;
    if (!manager->enemies_array[i].is_alive)
      continue;
    if (manager->enemies_array[i].is_exploding)
      continue; // Don't collide with exploding enemies

    Enemy *other = &manager->enemies_array[i];
    if (check_collision(new_x, new_y, enemy->width, enemy->height,
                        other->position_x, other->position_y, other->width,
                        other->height)) {
      return 1;
    }
  }
  return 0;
}

// Check if enemy would collide with player
int would_collide_with_player(Enemy *enemy, float new_x, float new_y,
                              float player_x, float player_y, float player_w,
                              float player_h) {
  return check_collision(new_x, new_y, enemy->width, enemy->height, player_x,
                         player_y, player_w, player_h);
}

// Improved enemy update with collision prevention
void update_single_enemy(Enemy *enemy, float target_x, float target_y,
                         float time_since_last_frame, EnemyManager *manager,
                         int enemy_index, float player_x, float player_y,
                         float player_w, float player_h) {
  // Don't update if dead or exploding
  if (!enemy->is_alive || enemy->is_exploding)
    return;

  // Calculate direction to target
  float direction_x = target_x - enemy->position_x;
  float direction_y = target_y - enemy->position_y;

  // Calculate distance to target
  float distance_to_target =
      sqrtf(direction_x * direction_x + direction_y * direction_y);

  // Normalize direction if we're not at the target
  if (distance_to_target > 0) {
    direction_x /= distance_to_target;
    direction_y /= distance_to_target;
  }

  // Calculate desired movement
  float desired_move_x =
      direction_x * enemy->movement_speed * time_since_last_frame;
  float desired_move_y =
      direction_y * enemy->movement_speed * time_since_last_frame;

  // Try moving in X direction first (if no collision)
  float new_x = enemy->position_x + desired_move_x;
  float new_y = enemy->position_y;

  if (!would_collide(enemy, new_x, new_y, manager, enemy_index)) {
    enemy->position_x = new_x;
  }

  // Try moving in Y direction (if no collision)
  new_x = enemy->position_x;
  new_y = enemy->position_y + desired_move_y;

  if (!would_collide(enemy, new_x, new_y, manager, enemy_index)) {
    enemy->position_y = new_y;
  }
}

// Fallback: if enemies do overlap, push them apart
void resolve_any_collisions(EnemyManager *manager) {
  for (int i = 0; i < manager->current_enemy_count; i++) {
    if (!manager->enemies_array[i].is_alive ||
        manager->enemies_array[i].is_exploding)
      continue;

    Enemy *enemy1 = &manager->enemies_array[i];

    // Check collision with other enemies
    for (int j = i + 1; j < manager->current_enemy_count; j++) {
      if (!manager->enemies_array[j].is_alive ||
          manager->enemies_array[j].is_exploding)
        continue;

      Enemy *enemy2 = &manager->enemies_array[j];

      if (check_collision(enemy1->position_x, enemy1->position_y, enemy1->width,
                          enemy1->height, enemy2->position_x,
                          enemy2->position_y, enemy2->width, enemy2->height)) {
        // Push enemies apart from each other
        float push_strength = 3.0f;
        float dx = enemy2->position_x - enemy1->position_x;
        float dy = enemy2->position_y - enemy1->position_y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance > 0) {
          dx /= distance;
          dy /= distance;
        }

        enemy1->position_x -= dx * push_strength;
        enemy1->position_y -= dy * push_strength;
        enemy2->position_x += dx * push_strength;
        enemy2->position_y += dy * push_strength;
      }
    }
  }
}

void draw_single_enemy(Enemy *enemy, SDL_Renderer *renderer) {
  if (!enemy->is_alive)
    return;

  if (enemy->is_exploding) {
    // Explosion effect - changing colors and growing size
    float explosion_progress = 1.0f - (enemy->explosion_timer / 0.3f);
    int red = 255;
    int green = (int)(255 * explosion_progress);
    int blue = 0;

    // For purple enemies, make explosion purple
    if (enemy->enemy_type == 2) {
      red = 128;
      blue = 128;
    } else if (enemy->enemy_type == 3) { // Boss explosion
      red = 255;
      green = (int)(128 * explosion_progress);
      blue = 0;
    }

    float base_size = (enemy->enemy_type == 3) ? 100.0f : 40.0f;
    float explosion_size = base_size + (20.0f * explosion_progress);
    float offset = (explosion_size - base_size) / 2.0f;

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_Rect explosion_rect = {enemy->position_x - offset,
                                enemy->position_y - offset, explosion_size,
                                explosion_size};
    SDL_RenderFillRect(renderer, &explosion_rect);

    // Draw some explosion particles (simple circles)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (int i = 0; i < 4; i++) {
      float angle = (float)i * 3.14159f / 2.0f;
      float particle_x =
          enemy->position_x + cosf(angle) * explosion_size * 0.6f;
      float particle_y =
          enemy->position_y + sinf(angle) * explosion_size * 0.6f;

      SDL_Rect particle = {particle_x - 2, particle_y - 2, 4, 4};
      SDL_RenderFillRect(renderer, &particle);
    }
  } else {
    // Normal enemy - color based on type and health
    int red = 0, green = 255, blue = 255;
    if (enemy->enemy_type == 2) {
      red = 128;
      green = 0;
      blue = 128; // Purple
    } else if (enemy->enemy_type == 3) {
      red = 255;
      green = 128;
      blue = 0; // Yellowish-red for boss
    } else if (enemy->enemy_type == 4) {
      red = 255;
      green = 165;
      blue = 0; // Bright orange for minions
    } else {
      int damage_percent = (int)((enemy->max_health - enemy->health_points) / (float)enemy->max_health * 255);
      green = 255 - damage_percent;
    }

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_Rect enemy_rectangle = {enemy->position_x, enemy->position_y,
                                 enemy->width, enemy->height};
    SDL_RenderFillRect(renderer, &enemy_rectangle);

    // Draw health bar for non-minions
    if (enemy->enemy_type != 4) {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect health_bg = {enemy->position_x, enemy->position_y - 5,
                           enemy->width, 3};
      SDL_RenderFillRect(renderer, &health_bg);

      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      float health_ratio = enemy->health_points / (float)enemy->max_health;
    SDL_Rect health_bar = {enemy->position_x, enemy->position_y - 5,
                            enemy->width * health_ratio, 3};
      SDL_RenderFillRect(renderer, &health_bar);
    }
  }
}

void update_all_enemies(EnemyManager *manager, float target_x, float target_y,
                        float time_since_last_frame, float player_x,
                        float player_y, float player_w, float player_h) {
  // First update each enemy with collision prevention
  for (int i = 0; i < manager->current_enemy_count; i++) {
    update_single_enemy(&manager->enemies_array[i], target_x, target_y,
                        time_since_last_frame, manager, i, player_x, player_y,
                        player_w, player_h);
  }

  // Then resolve any remaining collisions (as backup)
  resolve_any_collisions(manager);

  // Update explosion animations
  update_explosions(manager, time_since_last_frame);
}

void draw_all_enemies(EnemyManager *manager, SDL_Renderer *renderer) {
  for (int i = 0; i < manager->current_enemy_count; i++) {
    draw_single_enemy(&manager->enemies_array[i], renderer);
  }
}



void cleanup_enemy_manager(EnemyManager *manager) {
  free(manager->enemies_array);
  manager->enemies_array = NULL;
  manager->current_enemy_count = 0;
  manager->max_enemy_capacity = 0;
}
