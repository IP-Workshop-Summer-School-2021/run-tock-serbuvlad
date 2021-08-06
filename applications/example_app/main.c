/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include "hello.h"
#include <inttypes.h>


int main(void) {
  //printf ("Hello World!\r\n");
  //example_driver_action ();

  if (hello_is_present()) {
    printf("Hello is present\n");
  } else {
    printf("Hello is not present\n");
    return 1;
  }

  hello();

  hello_set(5);

  hello_incr();
  hello_incr();
  hello_decr();

  uint32_t n;

  hello_get(&n);

  printf("%"PRIu32"\n", n);

  return 0;
}
