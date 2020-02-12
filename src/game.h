#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>

#define CELL_FLAG 0x80
#define CELL_MINE 0x40
#define CELL_DUG 0x20
#define CELL_NEIGHBOR 0x0F

#define game_get(g, row, col) (&(g)->buffer[row * (g)->cols + col])

typedef struct game {
    uint8_t rows;
    uint8_t cols;
    uint8_t *buffer;
    /** Number of mines left to be flagged */
    uint8_t mines_left;
    /** Number of cells left to be dug up */
    uint16_t cells_left;
    struct {
        /** Whether the mines have been placed */
        bool initialized : 1;
        /** Whether the game is over */
        bool game_over : 1;
        /**
         * Whether the game was won. This field is uninitialized if and only if
         * game_over is false.
         */
        bool win : 1;
    } flags;
    /** Real-time clock value or time in seconds, depending on flags */
    uint16_t time;
} game;

bool game_init(game *g, uint8_t rows, uint8_t cols, uint8_t mines);

void game_free(game *g);

void game_dig(game *g, uint8_t row, uint8_t col);

void game_flag(game *g, uint8_t row, uint8_t col);

uint32_t game_time(const game *g);

#endif
