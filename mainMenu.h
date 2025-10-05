#ifndef MAINMENU_H
#define MAINMENU_H

#include <SDL2/SDL.h>

typedef enum {
  MENU_START_GAME,
  MENU_UPGRADES,
  MENU_SOUND,
  MENU_QUIT,
  MENU_OPTION_COUNT
} MenuOption;

typedef struct {
  int is_active;
  MenuOption selected_option;
  int option_count;
} MainMenu;

// Function declarations
void initialize_main_menu(MainMenu *menu);
void update_main_menu(MainMenu *menu, SDL_Event *event, int *game_running, int *start_game, int *show_upgrades, int *show_sound);
void draw_main_menu(MainMenu *menu, SDL_Renderer *renderer, int window_w, int window_h, int is_paused);

#endif