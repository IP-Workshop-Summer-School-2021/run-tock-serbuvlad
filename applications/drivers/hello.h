#pragma once

#include <stdbool.h>
#include <stdint.h>

extern const unsigned int HELLO_DRIVER_NUM;

bool hello_is_present(void);

bool hello(void);

bool hello_up(uint32_t);
bool hello_down(uint32_t);

bool hello_set(uint32_t);
bool hello_get(uint32_t *);

static inline
void hello_incr(void) {
    hello_up(1);
};

static inline
void hello_decr(void) {
    hello_down(1);
}