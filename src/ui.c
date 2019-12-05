#include "ui.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <graphx.h>
#include <tice.h>

#include "game.h"

/** Pixels of offset from top/left of screen */
#define OFFSET 10
/** Width of each cell */
#define WIDTH 10

static void ui_draw_cell(ui *u, uint8_t row, uint8_t col) {
    uint16_t pos = (uint16_t)row * u->game->cols + col;
    uint8_t cell = u->game->buffer[pos];
    char buf[2];
    const char *s;
    if ((cell & CELL_DUG) && (cell & CELL_MINE)) {
        s = "x";
    } else if (cell & CELL_DUG) {
        uint8_t neighbors;
        neighbors = (cell & CELL_NEIGHBOR);
        if (neighbors == 0) {
            return; /* No text to display */
        } else {
            buf[0] = '0' + neighbors;
            buf[1] = 0;
            s = buf;
        }
    } else if (cell & CELL_FLAG) {
        s = "*";
    } else {
        s = " .";
    }
    gfx_PrintStringXY(s, OFFSET + col * WIDTH, OFFSET + row * WIDTH);
    u->cache[pos] = cell;
}

static void ui_draw_game(ui *u) {
    uint8_t r, c;
    for (r = 0; r < u->game->rows; r++) {
        for (c = 0; c < u->game->cols; c++) {
            ui_draw_cell(u, r, c);
        }
    }
}

static void ui_redraw_cell(ui *u, uint8_t row, uint8_t col) {
    uint16_t pos = (uint16_t)row * u->game->cols + col;
    if (u->cache[pos] == u->game->buffer[pos])
        return;
    gfx_SetColor(255);
    gfx_FillRectangle(OFFSET + col * WIDTH, OFFSET + row * WIDTH, 8, 8);
    gfx_SetColor(0);
    ui_draw_cell(u, row, col);
}

static void ui_redraw_game(ui *u) {
    uint8_t r, c;
    for (r = 0; r < u->game->rows; r++) {
        for (c = 0; c < u->game->cols; c++) {
            ui_redraw_cell(u, r, c);
        }
    }
}

static void ui_draw_cursor_rect(uint8_t x, uint8_t y) {
    gfx_Rectangle((OFFSET - 2) + x * WIDTH, (OFFSET - 2) + y * WIDTH, 11, 11);
}

static void ui_draw_cursor(ui *u) {
    gfx_SetColor(0);
    ui_draw_cursor_rect(u->col, u->row);
    u->last_col = u->col;
    u->last_row = u->row;
}

static void ui_redraw_cursor(ui *u) {
    /* Erase old cursor */
    gfx_SetColor(255);
    ui_draw_cursor_rect(u->last_col, u->last_row);
    ui_draw_cursor(u);
}

bool ui_init(ui *u, game *g) {
    u->game = g;
    u->cache = malloc((size_t)g->rows * g->cols);
    return true;
}

void ui_free(ui *u) { free(u->cache); }

void ui_run(ui *u) {
    sk_key_t key;
    u->row = u->col = 0;
    /* Draw screen */
    ui_draw_game(u);
    ui_draw_cursor(u);
    /* Main loop */
    while (true) {
        /* Wait for key press */
        while ((key = os_GetCSC()) == 0)
            ;
        if (u->game->cells_left == 0) {
            while (os_GetCSC() != sk_Del)
                ;
            break;
        }
        if (key == sk_Del) {
            break; /* End game */
        } else if (key == sk_2nd) {
            if (!game_dig(u->game, u->row, u->col)) {
                /* The game is lost */
                ui_redraw_game(u);
                while (os_GetCSC() != sk_Del)
                    ;
                break; /* Exit the game */
            }
            /* Redraw the game */
            ui_redraw_game(u);
        } else if (key == sk_Alpha) {
            game_flag(u->game, u->row, u->col);
            /* Only need to redraw the current cell */
            ui_redraw_cell(u, u->row, u->col);
        } else if (key == sk_Up) {
            if (u->row != 0)
                u->row--;
            ui_redraw_cursor(u);
        } else if (key == sk_Down) {
            if (u->row != u->game->rows - 1)
                u->row++;
            ui_redraw_cursor(u);
        } else if (key == sk_Left) {
            if (u->col != 0)
                u->col--;
            ui_redraw_cursor(u);
        } else if (key == sk_Right) {
            if (u->col != u->game->cols - 1)
                u->col++;
            ui_redraw_cursor(u);
        }
    }
}
