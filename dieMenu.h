#ifndef DIE_MENU_H
#define DIE_MENU_H

#include "enemy.h"
#include <SDL2/SDL.h>

typedef enum {
  DIE_RESTART,
  DIE_MAIN_MENU,
  DIE_QUIT,
  DIE_OPTION_COUNT
} DieOption;

typedef struct {
  int is_active;
  int selected_option; // 0 = Restart, 1 = Main Menu, 2 = Quit
  SDL_Color background_color;
  SDL_Color text_color;
  SDL_Color selected_color;
  SDL_Rect restart_button;
  SDL_Rect main_menu_button;
  SDL_Rect quit_button;
} DieMenu;

// Function declarations
void initialize_die_menu(DieMenu *menu);
void update_die_menu(DieMenu *menu, SDL_Event *event, int *game_running,
                     int *restart_game, int *go_to_main_menu);
void draw_die_menu(DieMenu *menu, SDL_Renderer *renderer);
void reset_game_state(float *player_x, float *player_y, float *player_health,
                      int *player_is_alive, EnemyManager *enemies, int *score, int difficulty_level);

#endif
