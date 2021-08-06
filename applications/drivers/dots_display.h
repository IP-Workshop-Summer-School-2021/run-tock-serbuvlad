#pragma once

#include <stdbool.h>
#include <stdint.h>

extern const unsigned int DOTS_DISPLAY_DRIVER_NUM;

bool dots_display_is_present(void);

bool dots_display_print(char digit);
