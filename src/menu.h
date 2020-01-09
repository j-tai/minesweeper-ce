#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#include "game.h"

typedef struct menu {
    uint8_t difficulty;
} menu;

void menu_init(menu *m);

bool menu_init_game(menu *m, game *g);

/**
 * Runs the menu. Returns true if the user started the game, and false if the
 * user cancelled.
 */
bool menu_run(menu *m);

#endif
