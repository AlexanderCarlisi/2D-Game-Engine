#include "input.h"
#include <stdint.h>
#include "e_time.h"

static struct Trigger keyboard_map[INPUT_KEYBOARD_MAX];

bool input_kb_pressed(keycode_t key) {
  return (key < INPUT_KEYBOARD_MAX) ? keyboard_map[key].pressed : false;
}

bool input_kb_held(keycode_t key, uint64_t thresholdMs) {
  if (key >= INPUT_KEYBOARD_MAX) return false;
  return (etime_ms() - keyboard_map[key].pressed_since) >= thresholdMs;
}

void input_kb_set_press(keycode_t key) {
  if (key < INPUT_KEYBOARD_MAX && !keyboard_map[key].pressed) {
    keyboard_map[key].pressed = true;
    keyboard_map[key].pressed_since = etime_ms();
  }
}

void input_kb_set_release(keycode_t key) {
  if (key < INPUT_KEYBOARD_MAX && keyboard_map[key].pressed) {
    keyboard_map[key].pressed = false;
    keyboard_map[key].pressed_since = etime_ms();
  }
}
