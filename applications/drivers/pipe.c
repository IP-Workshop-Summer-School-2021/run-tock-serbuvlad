#include "pipe.h"

#include "tock.h"

#include <timer.h>

const unsigned int PIPE_DRIVER_NUM = 0xa0002;

bool pipe_is_present(void)
{
    syscall_return_t res = command(PIPE_DRIVER_NUM, 0, 0, 0);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool pipe_register(unsigned int pipeid, enum pipe_register_type prt)
{
    syscall_return_t res = command(PIPE_DRIVER_NUM, 1, pipeid, prt);

    return res.type == TOCK_SYSCALL_SUCCESS;
}

bool pipe_write(unsigned int pipeid, void *buf, size_t len)
{
    syscall_return_t ret;
    do {

        allow_ro_return_t res = allow_readonly(PIPE_DRIVER_NUM, pipeid, buf, len);

        if (!res.success)
            return false;

        ret = command(PIPE_DRIVER_NUM, 2, pipeid, PIPE_REGISTER_WRITE);

        if (ret.type != TOCK_SYSCALL_SUCCESS_U32)
            return false;

        if (ret.data[0] == 0)
            delay_ms(10);
    } while(ret.data[0] == 0);

    return true;
}

bool pipe_read(unsigned int pipeid, void *buf, size_t len)
{
    syscall_return_t ret;
    do {
        allow_rw_return_t res = allow_readwrite(PIPE_DRIVER_NUM, pipeid, buf, len);

            if (!res.success)
            return false;

        ret = command(PIPE_DRIVER_NUM, 2, pipeid, PIPE_REGISTER_WRITE);

        if (ret.type != TOCK_SYSCALL_SUCCESS_U32)
            return false;

        if (ret.data[0] == 0)
            delay_ms(10);
    } while(ret.data[0] == 0);

    return true;
}
