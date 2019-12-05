#ifndef DEQUE_H
#define DEQUE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct deque {
    uint8_t *buf;
    uint16_t start, end, cap;
} deque;

void deque_init(deque *deq);

bool deque_is_empty(deque *deq);

bool deque_push(deque *deq, uint8_t val);

uint8_t deque_pop(deque *deq);

void deque_free(deque *deq);

#endif
