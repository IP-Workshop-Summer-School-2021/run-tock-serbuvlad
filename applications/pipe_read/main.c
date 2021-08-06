/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include <inttypes.h>

#include <timer.h>

#include "pipe.h"


int main(void) {
  char buf[6];

  delay_ms(2000);


  printf("Register read %d\n", pipe_register(10001, PIPE_REGISTER_READ));

  printf("Read %d\n", pipe_read(10001, buf, 6));

  printf("%6s\n", buf);

  return 0;
}
