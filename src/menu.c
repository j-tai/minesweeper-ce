#include "menu.h"

#include <stdbool.h>
#include <stdio.h>

#include <tice.h>

#include "game.h"

/* Screen layout: */
/*    0    5   10   15   20   25  */
/*   ++----+----+----+----+----++ */
/* 0 |                          | */
/* 1 |      MINESWEEPER-CE      | */
/* 2 |                          | */
/* 3 |  Difficulty: <Very Hard> | */
/* 4 |  Rows:       000         | */
/* 5 |  Columns:    000         | */
/* 6 |  Mines:      000         | */
/* 7 |                          | */
/* 8 |         OK [2nd]         | */
/* 9 |                          | */
/*   +--------------------------+ */

struct difficulty {
    const char *name;
    uint8_t rows, cols, mines;
};

const struct difficulty DIFFICULTIES[] = {
    {"Very Easy", 10, 10, 10},  /* 10.0% mines */
    {"  Easy   ", 12, 15, 20},  /* 11.1% mines */
    {" Normal  ", 16, 20, 38},  /* 11.9% mines */
    {"  Hard   ", 20, 25, 65},  /* 13.0% mines */
    {"Very Hard", 22, 30, 100}, /* 15.2% mines */
    {" Extreme ", 22, 30, 112}, /* 17.0% mines */
};

#define NUM_DIFFICULTIES (sizeof(DIFFICULTIES) / sizeof(*DIFFICULTIES))

void menu_init(menu *m) { m->difficulty = 2; }

bool menu_init_game(menu *m, game *g) {
    const struct difficulty *diff = &DIFFICULTIES[m->difficulty];
    return game_init(g, diff->rows, diff->cols, diff->mines);
}

bool menu_run(menu *m) {
    sk_key_t key;
    /* Draw screen */
    os_ClrHomeFull();
    os_SetCursorPos(1, 6);
    os_PutStrFull("MINESWEEPER-CE");
    os_SetCursorPos(3, 1);
    os_PutStrFull("Difficulty: \xcf         \x05");
    os_SetCursorPos(4, 1);
    os_PutStrFull("Rows:");
    os_SetCursorPos(5, 1);
    os_PutStrFull("Columns:");
    os_SetCursorPos(6, 1);
    os_PutStrFull("Mines:");
    os_SetCursorPos(8, 9);
    os_PutStrFull("OK \3012nd]"); /* '\301' = '[' in TI-ASCII */
    while (true) {
        const struct difficulty *diff;
        char buf[4];
        /* Update display */
        diff = &DIFFICULTIES[m->difficulty];
        os_SetCursorPos(3, 14);
        os_PutStrFull(diff->name);
        sprintf(buf, "%3u", (int)diff->rows);
        os_SetCursorPos(4, 14);
        os_PutStrFull(buf);
        sprintf(buf, "%3u", (int)diff->cols);
        os_SetCursorPos(5, 14);
        os_PutStrFull(buf);
        sprintf(buf, "%3u", (int)diff->mines);
        os_SetCursorPos(6, 14);
        os_PutStrFull(buf);
        /* Wait for keypress */
    check_key:
        while ((key = os_GetCSC()) == 0)
            ;
        switch (key) {
        case sk_Left:
            if (m->difficulty != 0)
                m->difficulty--;
            break;
        case sk_Right:
            if (m->difficulty != NUM_DIFFICULTIES - 1)
                m->difficulty++;
            break;
        case sk_2nd:
        case sk_Enter:
            return true;
        case sk_Del:
            return false;
        default:
            goto check_key; /* Get the next key; skip the redrawing code */
        }
    }
}
