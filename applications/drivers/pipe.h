#pragma once

#include <stdbool.h>

extern const unsigned int PIPE_DRIVER_NUM;

bool pipe_is_present(void);

enum pipe_register_type {
    PIPE_REGISTER_WRITE,
    PIPE_REGISTER_READ,
};

bool pipe_register(unsigned int, enum pipe_register_type);

bool pipe_read(unsigned int, void *, size_t);

bool pipe_write(unsigned int, void *, size_t);
