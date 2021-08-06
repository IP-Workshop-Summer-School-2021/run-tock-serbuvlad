/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include <inttypes.h>

#include <timer.h>

#include "pipe.h"


int main(void) {
  char buf[] = "LOGIN\0";

  printf("Register write %d\n", pipe_register(10001, PIPE_REGISTER_WRITE));
  printf("Register write %d\n", pipe_register(10002, PIPE_REGISTER_WRITE));

  printf("Write %d\n", pipe_write(10001, buf, 6));

  return 0;
}
