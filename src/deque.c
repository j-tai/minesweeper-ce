#include "deque.h"

#include <stdlib.h>
#include <string.h>

#include <debug.h>

static void deque_print(deque *deq) {
    dbg_sprintf(dbgout, "start=%d, end=%d, cap=%d\n", (int)deq->start,
                (int)deq->end, (int)deq->cap);
}

void deque_init(deque *deq) {
    deq->buf = NULL;
    deq->start = 0;
    deq->end = 0;
    deq->cap = 0;
}

bool deque_is_empty(deque *deq) { return deq->start == deq->end; }

bool deque_push(deque *deq, uint8_t val) {
    uint16_t end_plus_1 = deq->end + 1;
    deque_print(deq);
    dbg_sprintf(dbgerr, "deque_push(%d)\n", (int)val);
    if (end_plus_1 == deq->cap)
        end_plus_1 = 0;
    if (deq->cap == 0 || end_plus_1 == deq->start) {
        /* Need to reallocate */
        uint16_t old_cap = deq->cap;
        uint8_t *new_buf;
        dbg_sprintf(dbgout, "Must reallocate...\n");
        if (old_cap == 0)
            deq->cap = 16;
        else
            deq->cap <<= 1;
        dbg_sprintf(dbgout, "New capacity: %d\n", (int) deq->cap);
        new_buf = malloc(deq->cap);
        if (new_buf == NULL)
            return false;
        if (deq->start > deq->end) {
            dbg_sprintf(dbgout, "Wraparound...\n");
            /* Wraps around */
            memcpy(new_buf, deq->buf + deq->start, old_cap - deq->start);
            memcpy(new_buf + (old_cap - deq->start), deq->buf, deq->end);
            deq->end += old_cap - deq->start;
        } else {
            dbg_sprintf(dbgout, "No wraparound...\n");
            /* Does not wrap around */
            memcpy(new_buf, deq->buf + deq->start, deq->end - deq->start);
            deq->end -= deq->start;
        }
        free(deq->buf);
        deq->buf = new_buf;
        deq->start = 0;
        deque_print(deq);
        end_plus_1 = deq->end + 1;
    }
    deq->buf[deq->end] = val;
    deq->end = end_plus_1;
    deque_print(deq);
    return true;
}

uint8_t deque_pop(deque *deq) {
    uint8_t res = deq->buf[deq->start++];
    dbg_sprintf(dbgerr, "deque_pop()\n");
    if (deq->start == deq->cap)
        deq->start = 0;
    dbg_sprintf(dbgout, "... = %d\n", (int) res);
    return res;
}

void deque_free(deque *deq) { free(deq->buf); }
