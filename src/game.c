#include "game.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <tice.h>

/*
 * Cell format: 0bFMD0NNNN
 * F = whether the cell is flagged.
 * M = whether the cell has a mine.
 * D = whether the cell has been dug up.
 * NNNN = number of neighbouring mines.
 * F and D must never be set at the same time.
 * M and D must never be set at the same time.
 * NNNN may not be set if M is set.
 */

bool game_init(game *g, uint8_t rows, uint8_t cols, uint8_t mines) {
    size_t len = (size_t)rows * cols;
    g->rows = rows;
    g->cols = cols;
    g->mines_left = mines;
    g->buffer = malloc(len);
    if (g->buffer == NULL)
        return false;
    memset(g->buffer, 0, len);
    return true;
}

void game_free(game *g) { free(g->buffer); }

static bool is_adjacent(uint16_t pos1, uint16_t pos2, uint8_t cols) {
    uint8_t diff;
    if (pos1 < pos2) {
        diff = pos2 - pos1;
    } else {
        diff = pos1 - pos2;
    }
    return diff == 0 || diff == 1 || diff == (cols - 1) || diff == cols ||
           diff == (cols + 1);
}

static void game_generate(game *g, uint8_t row, uint8_t col) {
    uint8_t cols = g->cols;
    uint16_t area = (uint16_t)g->rows * cols;
    uint16_t chosen_pos = (uint16_t)row * cols + col;
    uint8_t i;
    for (i = 0; i < g->mines_left; i++) {
        int8_t dr, dc;
        uint16_t pos;
        do {
            pos = random() % area;
        } while ((g->buffer[pos] & CELL_MINE) != 0 &&
                 !is_adjacent(chosen_pos, pos, cols));
        g->buffer[pos] |= CELL_MINE;
        /* Update neighbors; need to be careful not to go out of bounds */
        for (dr = (pos < cols ? 0 : -cols); dr <= (pos >= area - cols ? 0 : cols);
             dr += cols) {
            for (dc = (pos % cols == 0 ? 0 : -1);
                 dc <= (pos % cols == cols - 1 ? 0 : 1); dc++) {
                g->buffer[pos + dr + dc]++;
            }
        }
    }
    g->initialized = true;
}

bool game_dig(game *g, uint8_t row, uint8_t col) {
    uint8_t *cell;
    cell = game_get(g, row, col);
    /* Tried to dig a cell with a mine; the game is lost */
    if ((*cell & CELL_MINE) != 0) {
        return false;
    }
    if (!g->initialized) {
        game_generate(g, row, col);
    }
    *cell |= CELL_DUG;
    g->cells_left--;
    return true;
}

void game_flag(game *g, uint8_t row, uint8_t col) {
    uint8_t *cell;
    cell = game_get(g, row, col);
    /* Cannot flag a dug cell */
    if ((*cell & CELL_DUG) == 0) {
        /* Toggle flag */
        *cell ^= CELL_FLAG;
    }
}
