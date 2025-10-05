#include "dieMenu.h"
#include "enemy.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

void initialize_die_menu(DieMenu *menu) {
  menu->is_active = 0;
  menu->selected_option = 0;

  // Set colors
  menu->background_color = (SDL_Color){0, 0, 0, 200}; // Semi-transparent black
  menu->text_color = (SDL_Color){255, 255, 255, 255}; // White
  menu->selected_color = (SDL_Color){255, 0, 0, 255}; // Red

  // Initialize button positions (will be set properly when drawn)
  menu->restart_button = (SDL_Rect){0, 0, 200, 50};
  menu->main_menu_button = (SDL_Rect){0, 0, 200, 50};
  menu->quit_button = (SDL_Rect){0, 0, 200, 50};
}

void update_die_menu(DieMenu *menu, SDL_Event *event, int *game_running,
                     int *restart_game, int *go_to_main_menu) {
  if (!menu->is_active)
    return;

  if (event->type == SDL_KEYDOWN) {
    switch (event->key.keysym.sym) {
    case SDLK_UP:
      if (menu->selected_option > 0)
        menu->selected_option--;
      break;
    case SDLK_DOWN:
      if (menu->selected_option < 2)
        menu->selected_option++;
      break;

    case SDLK_RETURN:
    case SDLK_SPACE:
      // Execute selected option
      if (menu->selected_option == 0) {
        // Restart game
        *restart_game = 1;
        menu->is_active = 0;
      } else if (menu->selected_option == 1) {
        // Go to main menu
        *go_to_main_menu = 1;
        menu->is_active = 0;
      } else {
        // Quit game
        *game_running = 0;
      }
      break;

    case SDLK_ESCAPE:
      // Quit on escape
      *game_running = 0;
      break;
    }
  }

  // Handle mouse clicks on buttons
  if (event->type == SDL_MOUSEBUTTONDOWN) {
    if (event->button.button == SDL_BUTTON_LEFT) {
      float mouse_x = event->button.x;
      float mouse_y = event->button.y;

      // Check if mouse clicked on restart button
      if (mouse_x >= menu->restart_button.x &&
          mouse_x <= menu->restart_button.x + menu->restart_button.w &&
          mouse_y >= menu->restart_button.y &&
          mouse_y <= menu->restart_button.y + menu->restart_button.h) {
        *restart_game = 1;
        menu->is_active = 0;
      }

      // Check if mouse clicked on main menu button
      if (mouse_x >= menu->main_menu_button.x &&
          mouse_x <= menu->main_menu_button.x + menu->main_menu_button.w &&
          mouse_y >= menu->main_menu_button.y &&
          mouse_y <= menu->main_menu_button.y + menu->main_menu_button.h) {
        *go_to_main_menu = 1;
        menu->is_active = 0;
      }

      // Check if mouse clicked on quit button
      if (mouse_x >= menu->quit_button.x &&
          mouse_x <= menu->quit_button.x + menu->quit_button.w &&
          mouse_y >= menu->quit_button.y &&
          mouse_y <= menu->quit_button.y + menu->quit_button.h) {
        *game_running = 0;
      }
    }
  }

  // Handle mouse movement for hover effects
  if (event->type == SDL_MOUSEMOTION) {
    float mouse_x = event->motion.x;
    float mouse_y = event->motion.y;

    // Update selected option based on mouse position
    if (mouse_x >= menu->restart_button.x &&
        mouse_x <= menu->restart_button.x + menu->restart_button.w &&
        mouse_y >= menu->restart_button.y &&
        mouse_y <= menu->restart_button.y + menu->restart_button.h) {
      menu->selected_option = 0;
    } else if (mouse_x >= menu->main_menu_button.x &&
               mouse_x <= menu->main_menu_button.x + menu->main_menu_button.w &&
               mouse_y >= menu->main_menu_button.y &&
               mouse_y <= menu->main_menu_button.y + menu->main_menu_button.h) {
      menu->selected_option = 1;
    } else if (mouse_x >= menu->quit_button.x &&
               mouse_x <= menu->quit_button.x + menu->quit_button.w &&
               mouse_y >= menu->quit_button.y &&
               mouse_y <= menu->quit_button.y + menu->quit_button.h) {
      menu->selected_option = 2;
    }
  }
}

// Get text width
float get_text_width(const char *text, float scale) {
  float width = 0.0f;
  for (size_t i = 0; text[i] != '\0'; i++) {
    char c = text[i];
    if (c == ' ') {
      width += 6 * scale;
    } else {
      width += 12 * scale;
    }
  }
  return width;
}

// Clean text rendering for SCORE: and numbers
void draw_text(SDL_Renderer *renderer, const char *text, float x, float y,
                SDL_Color color, float scale) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  float current_x = x;
  for (size_t i = 0; text[i] != '\0'; i++) {
    char c = text[i];
    if (c == ' ') {
      current_x += 6 * scale;
      continue;
    }

    // Only render characters we need: S, C, O, R, E, :, 0-9
    if (c == 'S') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 6 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 4 * scale, 2 * scale,
                      6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == 'C') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
    } else if (c == 'O') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == 'R') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_Rect r6 = {current_x + 4 * scale, y + 6 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
      SDL_RenderFillRect(renderer, &r6);
    } else if (c == 'E') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == ':') {
      SDL_Rect r1 = {current_x + 2 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r2 = {current_x + 2 * scale, y + 6 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
    } else if (c == '0') {
      SDL_Rect r1 = {current_x, y, 6 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r4 = {current_x + 4 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == '1') {
      SDL_Rect r1 = {current_x + 2 * scale, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x, y + 2 * scale, 4 * scale, 2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
    } else if (c == '2') {
      SDL_Rect r1 = {current_x, y, 6 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 6 * scale, 2 * scale};
      SDL_Rect r4 = {current_x + 4 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r5 = {current_x, y + 6 * scale, 2 * scale, 2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == 'G') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == 'A') {
      SDL_Rect r1 = {current_x + 2 * scale, y, 6 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 2 * scale, 2 * scale, 8 * scale};
      SDL_Rect r3 = {current_x + 8 * scale, y + 2 * scale, 2 * scale,
                      8 * scale};
      SDL_Rect r4 = {current_x + 2 * scale, y + 4 * scale, 6 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == 'M') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x + 8 * scale, y, 2 * scale, 10 * scale};
      SDL_Rect r3 = {current_x + 2 * scale, y, 2 * scale, 4 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y, 2 * scale, 4 * scale};
      SDL_Rect r5 = {current_x + 4 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == 'V') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 6 * scale};
      SDL_Rect r2 = {current_x + 2 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_Rect r3 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_Rect r4 = {current_x + 8 * scale, y, 2 * scale, 6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == 'T') {
      SDL_Rect r1 = {current_x, y, 10 * scale, 2 * scale};
      SDL_Rect r2 = {current_x + 4 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
    } else if (c == 'Q') {
      SDL_Rect r1 = {current_x, y, 10 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r4 = {current_x + 8 * scale, y, 2 * scale, 8 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r6 = {current_x + 8 * scale, y + 8 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
      SDL_RenderFillRect(renderer, &r6);
    } else if (c == 'U') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 8 * scale};
      SDL_Rect r2 = {current_x + 8 * scale, y, 2 * scale, 8 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 10 * scale, 2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
    } else if (c == 'I') {
      SDL_Rect r1 = {current_x + 4 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
    } else if (c == '3') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == '4') {
      SDL_Rect r1 = {current_x + 4 * scale, y, 2 * scale,
                      10 * scale}; // right vertical
      SDL_Rect r2 = {current_x, y + 6 * scale, 6 * scale, 2 * scale}; // bottom
      SDL_Rect r3 = {current_x + 2 * scale, y, 2 * scale,
                      6 * scale}; // left top
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
    } else if (c == '5') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 6 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 4 * scale, 2 * scale,
                      6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == '6') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 6 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 4 * scale, 2 * scale,
                      6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == '7') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x + 6 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
    } else if (c == '8') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == '9') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 8 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 6 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == 'N') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x + 8 * scale, y, 2 * scale, 10 * scale};
      SDL_Rect r3 = {current_x + 2 * scale, y, 2 * scale, 2 * scale};
      SDL_Rect r4 = {current_x + 4 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 4 * scale, 2 * scale,
                      6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    } else if (c == 'Y') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 4 * scale};
      SDL_Rect r2 = {current_x + 8 * scale, y, 2 * scale, 4 * scale};
      SDL_Rect r3 = {current_x + 4 * scale, y + 4 * scale, 2 * scale,
                      6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
    } else if (c == 'H') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x + 8 * scale, y, 2 * scale, 10 * scale};
      SDL_Rect r3 = {current_x + 2 * scale, y + 4 * scale, 6 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
    } else if (c == 'P') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == 'D') {
      SDL_Rect r1 = {current_x, y, 6 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r4 = {current_x + 4 * scale, y + 2 * scale, 2 * scale,
                      6 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == 'K') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x + 6 * scale, y, 2 * scale, 4 * scale};
      SDL_Rect r3 = {current_x + 4 * scale, y + 4 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
    } else if (c == 'B') {
      SDL_Rect r1 = {current_x, y, 6 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y + 8 * scale, 6 * scale, 2 * scale};
      SDL_Rect r4 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r5 = {current_x + 6 * scale, y + 2 * scale, 2 * scale,
                      2 * scale};
      SDL_Rect r6 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
      SDL_RenderFillRect(renderer, &r6);
    } else if (c == 'F') {
      SDL_Rect r1 = {current_x, y, 8 * scale, 2 * scale};
      SDL_Rect r2 = {current_x, y + 4 * scale, 6 * scale, 2 * scale};
      SDL_Rect r3 = {current_x, y, 2 * scale, 10 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
    } else if (c == 'L') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x, y + 8 * scale, 8 * scale, 2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
    } else if (c == 'W') {
      SDL_Rect r1 = {current_x, y, 2 * scale, 10 * scale};
      SDL_Rect r2 = {current_x + 8 * scale, y, 2 * scale, 10 * scale};
      SDL_Rect r3 = {current_x + 2 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_Rect r4 = {current_x + 6 * scale, y + 6 * scale, 2 * scale,
                      4 * scale};
      SDL_Rect r5 = {current_x + 4 * scale, y + 8 * scale, 2 * scale,
                      2 * scale};
      SDL_RenderFillRect(renderer, &r1);
      SDL_RenderFillRect(renderer, &r2);
      SDL_RenderFillRect(renderer, &r3);
      SDL_RenderFillRect(renderer, &r4);
      SDL_RenderFillRect(renderer, &r5);
    }

    current_x += 12 * scale;
  }
}

void draw_die_menu(DieMenu *menu, SDL_Renderer *renderer) {
  if (!menu->is_active)
    return;

  // Get window size
  int window_width, window_height;
  SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

  // Draw semi-transparent background
  SDL_SetRenderDrawColor(renderer, menu->background_color.r,
                         menu->background_color.g, menu->background_color.b,
                         menu->background_color.a);
  SDL_RenderClear(renderer);

   // Calculate positions
   float center_x = window_width / 2.0f;
   float center_y = window_height / 2.0f;

    // Draw "GAME OVER" text
    SDL_Color game_over_color = {255, 0, 0, 255}; // Red
    const char *game_over_text = "GAME OVER";
    float game_over_width = get_text_width(game_over_text, 4.0f);
    draw_text(renderer, game_over_text, center_x - game_over_width / 2, center_y - 130,
              game_over_color, 4.0f);

    // Define button areas
    menu->restart_button = (SDL_Rect){center_x - 146, center_y - 15, 292, 60};
    menu->main_menu_button = (SDL_Rect){center_x - 146, center_y + 55, 292, 60};
    menu->quit_button = (SDL_Rect){center_x - 92, center_y + 125, 184, 60};

    // Draw restart button text
    SDL_Color restart_color =
        (menu->selected_option == 0) ? menu->selected_color : menu->text_color;
    const char *restart_text = "RESTART";
    float restart_width = get_text_width(restart_text, 3.0f);
    draw_text(renderer, restart_text, center_x - restart_width / 2, center_y - 5, restart_color,
              3.0f);

    // Draw main menu button text
    SDL_Color main_menu_color =
        (menu->selected_option == 1) ? menu->selected_color : menu->text_color;
    const char *main_menu_text = "MAIN MENU";
    float main_menu_width = get_text_width(main_menu_text, 3.0f);
    draw_text(renderer, main_menu_text, center_x - main_menu_width / 2, center_y + 65,
              main_menu_color, 3.0f);

    // Draw quit button text
    SDL_Color quit_color =
        (menu->selected_option == 2) ? menu->selected_color : menu->text_color;
    const char *quit_text = "QUIT";
    float quit_width = get_text_width(quit_text, 3.0f);
    draw_text(renderer, quit_text, center_x - quit_width / 2, center_y + 135, quit_color, 3.0f);


}

void reset_game_state(float *player_x, float *player_y, float *player_health,
                      int *player_is_alive, EnemyManager *enemies, int *score, int difficulty_level) {
  // Reset player state
  *player_x = 200.0f;
  *player_y = 100.0f;
  *player_health = 200.0f;
  *player_is_alive = 1;
  *score = 0; // Reset score

  // Clear all enemies
  cleanup_enemy_manager(enemies);
  initialize_enemy_manager(enemies, 1000);

  // Add starting enemies again
  add_enemy_to_manager(enemies, 400.0f, 300.0f, 1, difficulty_level);
  add_enemy_to_manager(enemies, 100.0f, 100.0f, 1, difficulty_level);
  add_enemy_to_manager(enemies, 600.0f, 400.0f, 1, difficulty_level);
  add_enemy_to_manager(enemies, 200.0f, 500.0f, 1, difficulty_level);

  printf("Game restarted!\n");
}
