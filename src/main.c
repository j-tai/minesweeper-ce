#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <debug.h>
#include <graphx.h>
#include <tice.h>

#include "deque.h"
#include "game.h"
#include "menu.h"
#include "ui.h"

static void out_of_memory(void) {
    os_SetCursorPos(0, 0);
    os_PutStrFull("Out of memory.");
    /* Wait for a key press */
    while (!os_GetCSC())
        ;
}

void main(void) { /* NOLINT for 'void' return type */
    menu m;
    game g;
    ui u;

    menu_init(&m);
    if (!menu_run(&m))
        return; /* User exited */

    /* Seed the random number generator */
    srand(rtc_Time());

    if (!menu_init_game(&m, &g)) {
        out_of_memory();
        return;
    }

    if (!ui_init(&u, &g)) {
        game_free(&g);
        out_of_memory();
        return;
    }

    gfx_Begin(); /* Initialize graphics */
    ui_run(&u);  /* Run game */
    gfx_End();   /* Finish graphics */
    ui_free(&u);
    game_free(&g);
}
