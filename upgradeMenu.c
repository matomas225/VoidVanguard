#include "upgradeMenu.h"
#include "mainMenu.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

// Forward declarations
void draw_text(SDL_Renderer *renderer, const char *text, float x, float y,
               SDL_Color color, float scale);
float get_text_width(const char *text, float scale);
void save_coins(int coins);

// Get damage upgrade cost based on current level
int get_damage_upgrade_cost(int current_level) {
  int cost = 100 + 50 * current_level;
  return cost > 500 ? 500 : cost;
}

// Save/load upgrades
void save_upgrades(PlayerUpgrades *upgrades) {
  FILE *file = fopen("upgrades.txt", "w");
  if (file) {
    fprintf(file, "%d %d %d\n", upgrades->damage_level, upgrades->double_shots,
            upgrades->triple_shots);
    fclose(file);
  }
}

void load_upgrades(PlayerUpgrades *upgrades) {
  // Set defaults
  upgrades->damage_level = 0;
  upgrades->double_shots = 0;
  upgrades->triple_shots = 0;

  FILE *file = fopen("upgrades.txt", "r");
  if (file) {
    fscanf(file, "%d %d %d", &upgrades->damage_level, &upgrades->double_shots,
           &upgrades->triple_shots);
    fclose(file);
  }
}

void initialize_upgrade_menu(UpgradeMenu *menu) {
  menu->is_active = 0;
  menu->selected_option = UPGRADE_DAMAGE;
  menu->option_count = UPGRADE_OPTION_COUNT;
}

void update_upgrade_menu(UpgradeMenu *menu, SDL_Event *event,
                         int *show_upgrades, int *coins,
                         PlayerUpgrades *upgrades, MainMenu *main_menu) {
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
       case UPGRADE_DAMAGE: {
         int cost = 100 + 50 * upgrades->damage_level;
         if (cost > 500) cost = 500;
         if (*coins >= cost) {
           *coins -= cost;
           upgrades->damage_level++;
           save_upgrades(upgrades);
           save_coins(*coins);
           printf("Upgraded damage to level %d\n", upgrades->damage_level);
         }
         break;
       }
       case UPGRADE_DOUBLE_SHOTS:
         if (*coins >= 1000 && upgrades->double_shots == 0) {
           *coins -= 1000;
           upgrades->double_shots = 1;
           save_upgrades(upgrades);
           save_coins(*coins);
           printf("Unlocked double shots\n");
         }
         break;
       case UPGRADE_TRIPLE_SHOTS:
         if (*coins >= 2000 && upgrades->triple_shots == 0) {
           *coins -= 2000;
           upgrades->triple_shots = 1;
           save_upgrades(upgrades);
           save_coins(*coins);
           printf("Unlocked triple shots\n");
         }
         break;
      case UPGRADE_BACK:
        *show_upgrades = 0;
        menu->is_active = 0;
        main_menu->is_active = 1;
        break;
      default:
        break;
      }
      break;
    case SDLK_ESCAPE:
      *show_upgrades = 0;
      break;
    }
  }
}

void draw_upgrade_menu(UpgradeMenu *menu, SDL_Renderer *renderer, int window_w,
                        int window_h, int coins, PlayerUpgrades *upgrades) {
   // Clear screen
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);

    // Draw title
    SDL_Color title_color = {255, 255, 255, 255};
    const char *title = "UPGRADES";
    float title_width = get_text_width(title, 3.0f);
    draw_text(renderer, title, window_w / 2 - title_width / 2, 100, title_color, 3.0f);

    // Draw coins
    char coins_text[20];
    sprintf(coins_text, "COINS: %d", coins);
    float coins_width = get_text_width(coins_text, 2.0f);
    draw_text(renderer, coins_text, window_w / 2 - coins_width / 2, 150, title_color, 2.0f);

   // Draw menu options
   char options[4][50];
   int costs[4] = {0, 1000, 2000, 0};
   costs[0] = 100 + 50 * upgrades->damage_level;
   if (costs[0] > 500) costs[0] = 500;
   sprintf(options[0], "DAMAGE + (%d COINS)", costs[0]);
   sprintf(options[1], "DOUBLE SHOTS (1000 COINS)");
   sprintf(options[2], "TRIPLE SHOTS (2000 COINS)");
   sprintf(options[3], "BACK");
   for (int i = 0; i < menu->option_count; i++) {
     SDL_Color color;
     if (i == menu->selected_option) {
       color = (SDL_Color){255, 255, 0, 255};
     } else if ((i == UPGRADE_DAMAGE && coins < costs[i]) ||
                (i == UPGRADE_DOUBLE_SHOTS &&
                 (coins < costs[i] || upgrades->double_shots)) ||
                (i == UPGRADE_TRIPLE_SHOTS &&
                 (coins < costs[i] || upgrades->triple_shots))) {
       color = (SDL_Color){128, 128, 128,
                           255}; // Gray if can't afford or already have
     } else {
       color = (SDL_Color){255, 255, 255, 255};
     }
     float text_width = get_text_width(options[i], 2.0f);
     float x = window_w / 2 - text_width / 2;
     draw_text(renderer, options[i], x, 220 + i * 60, color, 2.0f);
   }

   // Draw current upgrades
   char upgrade_text[50];
   sprintf(upgrade_text, "DAMAGE LEVEL: %d", upgrades->damage_level);
   draw_text(renderer, upgrade_text, 50, window_h - 120, title_color, 1.5f);
   sprintf(upgrade_text, "DOUBLE SHOTS: %s",
           upgrades->double_shots ? "YES" : "NO");
   draw_text(renderer, upgrade_text, 50, window_h - 90, title_color, 1.5f);
   sprintf(upgrade_text, "TRIPLE SHOTS: %s",
           upgrades->triple_shots ? "YES" : "NO");
   draw_text(renderer, upgrade_text, 50, window_h - 60, title_color, 1.5f);

    // Draw instructions
    SDL_Color instr_color = {200, 200, 200, 255};
    const char *instr = "USE UP/DOWN TO SELECT, ENTER TO BUY/BACK";
    float instr_width = get_text_width(instr, 1.0f);
    draw_text(renderer, instr, window_w / 2 - instr_width / 2, window_h - 50, instr_color, 1.0f);
}
