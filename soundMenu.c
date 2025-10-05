#include "soundMenu.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Forward declaration
extern void draw_text(SDL_Renderer *renderer, const char *text, float x, float y, SDL_Color color, float scale);

void initialize_sound_menu(SoundMenu *menu) {
  menu->is_active = 0;
  menu->selected_option = SOUND_VOLUME_UP;
  menu->option_count = SOUND_OPTION_COUNT;
  menu->master_volume = 128; // max
  Mix_Volume(-1, menu->master_volume);
  Mix_VolumeMusic(menu->master_volume);
}

void update_sound_menu(SoundMenu *menu, SDL_Event *event, int *show_sound) {
  if (event->type == SDL_KEYDOWN) {
    switch (event->key.keysym.sym) {
      case SDLK_UP:
        if (menu->selected_option > 0) menu->selected_option--;
        break;
      case SDLK_DOWN:
        if (menu->selected_option < menu->option_count - 1) menu->selected_option++;
        break;
      case SDLK_RETURN:
      case SDLK_SPACE:
        switch (menu->selected_option) {
          case SOUND_VOLUME_UP:
            if (menu->master_volume < 128) {
              menu->master_volume += 8;
              Mix_Volume(-1, menu->master_volume);
              Mix_VolumeMusic(menu->master_volume);
            }
            break;
          case SOUND_VOLUME_DOWN:
            if (menu->master_volume > 0) {
              menu->master_volume -= 8;
              Mix_Volume(-1, menu->master_volume);
              Mix_VolumeMusic(menu->master_volume);
            }
            break;
          case SOUND_BACK:
            *show_sound = 0;
            menu->is_active = 0;
            break;
        }
        break;
    }
  }
}

void draw_sound_menu(SoundMenu *menu, SDL_Renderer *renderer, int window_w, int window_h) {
  // Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // Draw title
  SDL_Color title_color = {255, 255, 255, 255};
  const char *title = "SOUND SETTINGS";
  draw_text(renderer, title, window_w / 2 - 150, 100, title_color, 2.5f);

  // Draw current volume
  char vol_text[20];
  sprintf(vol_text, "Volume: %d%%", (menu->master_volume * 100) / 128);
  draw_text(renderer, vol_text, window_w / 2 - 100, 200, title_color, 2.0f);

  // Draw options
  const char *options[] = {"VOLUME UP", "VOLUME DOWN", "BACK"};
  for (int i = 0; i < menu->option_count; i++) {
    SDL_Color color = (i == menu->selected_option) ? (SDL_Color){255, 255, 0, 255} : (SDL_Color){255, 255, 255, 255};
    draw_text(renderer, options[i], window_w / 2 - 100, 300 + i * 60, color, 2.0f);
  }

  // Draw instructions
  SDL_Color instr_color = {200, 200, 200, 255};
  const char *instr = "USE UP/DOWN TO SELECT, ENTER TO CONFIRM";
  draw_text(renderer, instr, window_w / 2 - 200, window_h - 50, instr_color, 1.0f);
}