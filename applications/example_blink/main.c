/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include <stdlib.h>
#include <led.h>
#include <timer.h>

#include <button.h>
#include "example_driver.h"

#if 0

int main(void) {
  // Ask the kernel how many LEDs are on this board.
  int num_leds;
  int err = led_count(&num_leds);
  if (err < 0) return err;

  // Blink the LEDs in a binary count pattern and scale
  // to the number of LEDs on the board.
  for (int count = 0; ; count++) {
    for (int i = 0; i < num_leds; i++) {
      if (count & (1 << i)) {
        led_on(i);
      } else {
        led_off(i);
      }
    }

    // This delay uses an underlying timer in the kernel.
    delay_ms(250);
  }
}

#endif

struct state {
  int place;
};

// Callback for button presses.
//   btn_num: The index of the button associated with the callback
//   val: 1 if pressed, 0 if depressed
static void button_callback(int btn_num,
                            int val,
                            __attribute__ ((unused)) int arg2,
                            void *ud) {
  
  struct state *state = ud;

  if (val == 0)
    return;

  led_off(state->place);

  switch (btn_num) {
  case 0:
    state->place--;
    break;
  case 1:
    state->place++;
    break;
  default:
    printf("Buton %d\n", btn_num);
  }

  led_on(state->place);
}

int main(void) {
  int err;

  struct state *state = malloc(sizeof(struct state));

  printf("Start\n");

  state->place = 0;

  err = button_subscribe(button_callback, state);
  if (err < 0) return err;

  // Enable interrupts on each button.
  int count;
  err = button_count(&count);
  if (err < 0) return err;

  for (int i = 0; i < count; i++) {
    button_enable_interrupt(i);
  }

  return 0;
}
