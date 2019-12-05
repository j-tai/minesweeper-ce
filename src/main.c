#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <debug.h>
#include <graphx.h>
#include <tice.h>

#include "deque.h"
#include "game.h"
#include "ui.h"

static void out_of_memory(void) {
    os_SetCursorPos(0, 0);
    os_PutStrFull("Out of memory.");
    /* Wait for a key press */
    while (!os_GetCSC())
        ;
}

void main(void) { /* NOLINT for 'void' return type */
    game g;
    ui u;

    /* Seed the random number generator */
    srand(rtc_Time());

    if (game_init(&g, 22, 30, 40)) {
        if (ui_init(&u, &g)) {
            gfx_Begin(); /* Initialize graphics */
            ui_run(&u);  /* Run game */
            gfx_End();   /* Finish graphics */
            ui_free(&u);
        } else {
            out_of_memory();
        }
        game_free(&g);
    } else {
        out_of_memory();
    }
}
