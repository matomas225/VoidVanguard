#include "mainMenu.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

// Forward declarations
void draw_text(SDL_Renderer *renderer, const char *text, float x, float y,
               SDL_Color color, float scale);
float get_text_width(const char *text, float scale);
float get_text_width(const char *text, float scale);

void initialize_main_menu(MainMenu *menu) {
  menu->is_active = 1;
  menu->selected_option = MENU_START_GAME;
  menu->option_count = MENU_OPTION_COUNT;
}

void update_main_menu(MainMenu *menu, SDL_Event *event, int *game_running, int *start_game, int *show_upgrades, int *show_sound) {
  if (event->type == SDL_KEYDOWN) {
    switch (event->key.keysym.sym) {
    case SDLK_UP:
      if (menu->selected_option > 0) {
        menu->selected_option--;
      }
      break;
    case SDLK_DOWN:
      if (menu->selected_option < menu->option_count - 1) {
        menu->selected_option++;
      }
      break;
    case SDLK_RETURN:
    case SDLK_SPACE:
      switch (menu->selected_option) {
      case MENU_START_GAME:
        *start_game = 1;
        menu->is_active = 0;
        break;
       case MENU_UPGRADES:
         *show_upgrades = 1;
         menu->is_active = 0;
         break;
       case MENU_SOUND:
         *show_sound = 1;
         menu->is_active = 0;
         break;
       case MENU_QUIT:
         *game_running = 0;
         break;
      default:
        break;
      }
      break;
    case SDLK_ESCAPE:
      *game_running = 0;
      break;
    }
  }
}

void draw_main_menu(MainMenu *menu, SDL_Renderer *renderer, int window_w, int window_h, int is_paused) {
   // Clear screen
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);

    // Draw title
    SDL_Color title_color = {255, 255, 255, 255};
    const char *title = "VOID VANGUARD";
    float title_width = get_text_width(title, 3.0f);
    draw_text(renderer, title, window_w / 2 - title_width / 2, 100, title_color, 3.0f);

     // Draw menu options
      const char *options[] = {is_paused ? "RESUME" : "START GAME", "UPGRADES", "SOUND", "QUIT"};
    for (int i = 0; i < menu->option_count; i++) {
      SDL_Color color = (i == menu->selected_option) ? (SDL_Color){255, 255, 0, 255} : (SDL_Color){255, 255, 255, 255};
      float text_width = get_text_width(options[i], 2.0f);
      float x = window_w / 2 - text_width / 2;
      draw_text(renderer, options[i], x, 250 + i * 60, color, 2.0f);
    }

    // Draw instructions
    SDL_Color instr_color = {200, 200, 200, 255};
    const char *instr = "USE UP/DOWN TO SELECT, ENTER TO CONFIRM";
    float instr_width = get_text_width(instr, 1.0f);
    draw_text(renderer, instr, window_w / 2 - instr_width / 2, window_h - 50, instr_color, 1.0f);
}