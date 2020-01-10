#include "deque.h"

#include <stdlib.h>
#include <string.h>

void deque_init(deque *deq) {
    deq->buf = NULL;
    deq->start = 0;
    deq->end = 0;
    deq->cap = 0;
}

bool deque_is_empty(deque *deq) { return deq->start == deq->end; }

bool deque_push(deque *deq, uint8_t val) {
    uint16_t end_plus_1 = deq->end + 1;
    if (end_plus_1 == deq->cap)
        end_plus_1 = 0;
    if (deq->cap == 0 || end_plus_1 == deq->start) {
        /* Need to reallocate */
        uint16_t old_cap = deq->cap;
        uint8_t *new_buf;
        if (old_cap == 0)
            deq->cap = 16;
        else
            deq->cap <<= 1;
        new_buf = malloc(deq->cap);
        if (new_buf == NULL)
            return false;
        if (deq->start > deq->end) {
            /* Wraps around */
            memcpy(new_buf, deq->buf + deq->start, old_cap - deq->start);
            memcpy(new_buf + (old_cap - deq->start), deq->buf, deq->end);
            deq->end += old_cap - deq->start;
        } else {
            /* Does not wrap around */
            memcpy(new_buf, deq->buf + deq->start, deq->end - deq->start);
            deq->end -= deq->start;
        }
        free(deq->buf);
        deq->buf = new_buf;
        deq->start = 0;
        end_plus_1 = deq->end + 1;
    }
    deq->buf[deq->end] = val;
    deq->end = end_plus_1;
    return true;
}

uint8_t deque_pop(deque *deq) {
    uint8_t res = deq->buf[deq->start++];
    if (deq->start == deq->cap)
        deq->start = 0;
    return res;
}

void deque_free(deque *deq) { free(deq->buf); }
