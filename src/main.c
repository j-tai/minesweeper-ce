#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <graphx.h>
#include <tice.h>

#include "game.h"
#include "ui.h"

void main(void) { /* NOLINT for 'void' return type */
    game g;

    /* Seed the random number generator */
    srand(rtc_Time());

    if (game_init(&g, 22, 30, 40)) {
        ui_run(&g);
        /* Free game data */
        game_free(&g);
    } else {
        os_SetCursorPos(0, 0);
        os_PutStrFull("Out of memory.");
    }
}
