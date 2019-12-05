#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <graphx.h>
#include <tice.h>

#include "game.h"
#include "ui.h"

static void out_of_memory(void) {
    os_SetCursorPos(0, 0);
    os_PutStrFull("Out of memory.");
    /* Wait for a key press */
    while (!os_GetCSC());
}

void main(void) { /* NOLINT for 'void' return type */
    game g;
    ui u;

    /* Seed the random number generator */
    srand(rtc_Time());

    if (game_init(&g, 22, 30, 40)) {
        if (ui_init(&u, &g)) {
            ui_run(&u);
            ui_free(&u);
        } else {
            out_of_memory();
        }
        game_free(&g);
    } else {
        out_of_memory();
    }
}
