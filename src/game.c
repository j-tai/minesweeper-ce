#include "game.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>
#include <tice.h>

#include "deque.h"

/*
 * Cell format: 0bFMD0NNNN
 * F = whether the cell is flagged.
 * M = whether the cell has a mine.
 * D = whether the cell has been dug up.
 * NNNN = number of neighbouring mines.
 * F and D must never be set at the same time.
 * If M and D are both set, then the digging of this cell caused the
 * game over.
 * NNNN may not be set if M is set.
 */

bool game_init(game *g, uint8_t rows, uint8_t cols, uint8_t mines) {
    size_t len = (size_t)rows * cols;
    g->rows = rows;
    g->cols = cols;
    g->buffer = malloc(len);
    if (g->buffer == NULL)
        return false;
    memset(g->buffer, 0, len);
    g->mines_left = mines;
    g->cells_left = len - mines;
    g->initialized = false;
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
        } while ((g->buffer[pos] & CELL_MINE) != 0 ||
                 is_adjacent(chosen_pos, pos, cols));
        g->buffer[pos] |= CELL_MINE;
        /* Update neighbors; need to be careful not to go out of bounds */
        for (dr = (pos < cols ? 0 : -cols);
             dr <= (pos >= area - cols ? 0 : cols); dr += cols) {
            for (dc = (pos % cols == 0 ? 0 : -1);
                 dc <= (pos % cols == cols - 1 ? 0 : 1); dc++) {
                g->buffer[pos + dr + dc]++;
            }
        }
    }
    g->initialized = true;
}

void game_dig_bfs(game *g, uint8_t row, uint8_t col) {
    deque deq;
    deque_init(&deq);
    if (!(deque_push(&deq, row) && deque_push(&deq, col))) {
        /* Failed to allocate; just abort */
        deque_free(&deq);
        return;
    }
    while (!deque_is_empty(&deq)) {
        int8_t dr, dc;
        row = deque_pop(&deq);
        col = deque_pop(&deq);
        for (dr = (row == 0 ? 0 : -1); dr <= (row == g->rows - 1 ? 0 : 1);
             dr++) {
            for (dc = (col == 0 ? 0 : -1); dc <= (col == g->cols - 1 ? 0 : 1);
                 dc++) {
                uint8_t nr = row + dr, nc = col + dc;
                uint8_t *cell = game_get(g, nr, nc);
                /* Don't touch any flagged cells */
                if (*cell & CELL_FLAG)
                    continue;
                if ((*cell & CELL_DUG) == 0)
                    g->cells_left--;
                if (*cell != 0) {
                    *cell |= CELL_DUG;
                    continue;
                }
                *cell |= CELL_DUG;
                if (!(deque_push(&deq, nr) && deque_push(&deq, nc)))
                    break; /* Allocation failed; abort */
            }
        }
    }
    deque_free(&deq);
}

bool game_dig(game *g, uint8_t row, uint8_t col) {
    uint8_t *cell = game_get(g, row, col);
    /* Return if the cell has already been dug or has a flag */
    if ((*cell & CELL_DUG) || (*cell & CELL_FLAG))
        return true;
    /* Tried to dig a cell with a mine; the game is lost */
    if ((*cell & CELL_MINE) != 0) {
        *cell |= CELL_DUG;
        return false;
    }
    if (!g->initialized) {
        game_generate(g, row, col);
    }
    *cell |= CELL_DUG;
    /* Run a BFS if the cell is 0 */
    if ((*cell & CELL_NEIGHBOR) == 0) {
        game_dig_bfs(g, row, col);
    }
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
