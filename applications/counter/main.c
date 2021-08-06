/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <button.h>

#include "dots_display.h"

struct state {
  char digit;
};

static void button_callback(int btn_num,
                            int val,
                            __attribute__ ((unused)) int arg2,
                            void *ud) {
  
  struct state *state = ud;

  if (val == 0)
    return;

  switch (btn_num) {
  case 0:
    state->digit--;
    break;
  case 1:
    state->digit++;
    break;
  case 2:
    state->digit = '0';
    break;
  default:
    return;
  }

  if (!dots_display_print(state->digit)) {
    printf("Dots display error\n");
  }
}

int main(void) {
  int err;

  struct state *state = malloc(sizeof(struct state));

  state->digit = '0';

  if (!dots_display_is_present()) {
    printf("Required dots_display!\n");
    return 0;
  }

  dots_display_print(state->digit);

  err = button_subscribe(button_callback, state);
  if (err < 0) return err;

  int count;
  err = button_count(&count);
  if (err < 0) return err;

  for (int i = 0; i < count; i++) {
    button_enable_interrupt(i);
  }

  return 0;
}
