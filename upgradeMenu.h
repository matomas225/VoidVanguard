#ifndef UPGRADEMENU_H
#define UPGRADEMENU_H

#include "mainMenu.h"
#include "upgrades.h"
#include <SDL2/SDL.h>

typedef enum {
  UPGRADE_DAMAGE,
  UPGRADE_DOUBLE_SHOTS,
  UPGRADE_TRIPLE_SHOTS,
  UPGRADE_BACK,
  UPGRADE_OPTION_COUNT
} UpgradeOption;

typedef struct {
  int is_active;
  UpgradeOption selected_option;
  int option_count;
} UpgradeMenu;

// Function declarations
void initialize_upgrade_menu(UpgradeMenu *menu);
void update_upgrade_menu(UpgradeMenu *menu, SDL_Event *event, int *show_upgrades, int *coins, PlayerUpgrades *upgrades, MainMenu *main_menu);
void draw_upgrade_menu(UpgradeMenu *menu, SDL_Renderer *renderer, int window_w, int window_h, int coins, PlayerUpgrades *upgrades);
void load_upgrades(PlayerUpgrades *upgrades);

#endif