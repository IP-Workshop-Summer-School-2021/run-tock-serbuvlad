#include "dots_display.h"

#include "tock.h"

const unsigned int DOTS_DISPLAY_DRIVER_NUM = 0xa0001;

bool dots_display_is_present(void)
{
    syscall_return_t res = command(DOTS_DISPLAY_DRIVER_NUM, 0, 0, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool dots_display_print(char digit)
{
    syscall_return_t res = command(DOTS_DISPLAY_DRIVER_NUM, 1, digit, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

