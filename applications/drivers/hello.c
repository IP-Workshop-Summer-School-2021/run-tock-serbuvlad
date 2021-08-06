#include "hello.h"

#include "tock.h"

const unsigned int HELLO_DRIVER_NUM = 0xa0000;

bool hello_is_present(void)
{
    syscall_return_t res = command(HELLO_DRIVER_NUM, 0, 0, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool hello(void)
{
    syscall_return_t res = command(HELLO_DRIVER_NUM, 1, 0, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}


bool hello_up(uint32_t delta)
{
    syscall_return_t res = command(HELLO_DRIVER_NUM, 2, delta, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool hello_down(uint32_t delta)
{
    syscall_return_t res = command(HELLO_DRIVER_NUM, 3, delta, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool hello_set(uint32_t val)
{
    syscall_return_t res = command(HELLO_DRIVER_NUM, 4, val, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool hello_get(uint32_t *val)
{
    syscall_return_t res = command(HELLO_DRIVER_NUM, 5, 0, 0);

    if (res.type == TOCK_SYSCALL_SUCCESS_U32) {
        *val = res.data[0];
        return true;
    } else {
        return false;
    }
}
