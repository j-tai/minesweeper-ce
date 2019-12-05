#ifndef UI_H
#define UI_H

#include "game.h"

typedef struct ui {
    game *game;
    uint8_t row, col;
    uint8_t last_row, last_col;
    /* Last seen cells */
    uint8_t *cache;
} ui;

bool ui_init(ui *u, game *g);

void ui_free(ui *u);

void ui_run(ui *u);

#endif
