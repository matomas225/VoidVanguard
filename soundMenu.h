#ifndef SOUNDMENU_H
#define SOUNDMENU_H

#include <SDL2/SDL.h>

typedef enum {
  SOUND_VOLUME_UP,
  SOUND_VOLUME_DOWN,
  SOUND_BACK,
  SOUND_OPTION_COUNT
} SoundOption;

typedef struct {
  int is_active;
  SoundOption selected_option;
  int option_count;
  int master_volume; // 0-128
} SoundMenu;

// Function declarations
void initialize_sound_menu(SoundMenu *menu);
void update_sound_menu(SoundMenu *menu, SDL_Event *event, int *show_sound);
void draw_sound_menu(SoundMenu *menu, SDL_Renderer *renderer, int window_w, int window_h);

#endif