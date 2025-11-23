#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

#define keycode_t uint32_t

/// @struct Trigger
/// TODO: doxy
typedef struct Trigger {
  bool pressed;
  int64_t pressed_since; // miliseconds: Set when pressed and released
} Trigger;


/// @brief return true if the keycode's held value is greater than zero. KEYBOARD
bool input_kb_pressed(keycode_t key);

/// @brief return true if the keycode's held value is greather than provided threshold. KEYBOARD
bool input_kb_held(keycode_t key, uint64_t thresholdMs);

/// @brief set to current time, if not already currently held. KEYBOARD
void input_kb_set_press(keycode_t key);

/// @brief set pressed to false. KEYBOARD
void input_kb_set_release(keycode_t key);


#ifdef __linux__

#define INPUT_KEYBOARD_MAX    133

#define INPUT_KEYB_Q      24
#define INPUT_KEYB_W      25
#define INPUT_KEYB_E      26
#define INPUT_KEYB_R      27
#define INPUT_KEYB_T      28
#define INPUT_KEYB_Y      29
#define INPUT_KEYB_U      30
#define INPUT_KEYB_I      31
#define INPUT_KEYB_O      32
#define INPUT_KEYB_P      33

#define INPUT_KEYB_A      38
#define INPUT_KEYB_S      39
#define INPUT_KEYB_D      40
#define INPUT_KEYB_F      41
#define INPUT_KEYB_G      42
#define INPUT_KEYB_H      43
#define INPUT_KEYB_J      44
#define INPUT_KEYB_K      45
#define INPUT_KEYB_L      46

#define INPUT_KEYB_Z      52
#define INPUT_KEYB_X      53
#define INPUT_KEYB_C      54
#define INPUT_KEYB_V      55
#define INPUT_KEYB_B      56
#define INPUT_KEYB_N      57
#define INPUT_KEYB_M      58

#define INPUT_KEYB_1      10
#define INPUT_KEYB_2      11
#define INPUT_KEYB_3      12
#define INPUT_KEYB_4      13
#define INPUT_KEYB_5      14
#define INPUT_KEYB_6      15
#define INPUT_KEYB_7      16
#define INPUT_KEYB_8      17
#define INPUT_KEYB_9      18
#define INPUT_KEYB_0      19

#define INPUT_KEYB_ESC    9
#define INPUT_KEYB_TILDE  49
#define INPUT_KEYB_TAB    23
#define INPUT_KEYB_CAPSL  66
#define INPUT_KEYB_LSHFT  50
#define INPUT_KEYB_LCTRL  37
#define INPUT_KEYB_LALT   64
#define INPUT_KEYB_MOD    133
#define INPUT_KEYB_SPACE  65
#define INPUT_KEYB_RSHFT  62
#define INPUT_KEYB_RCTRL  105
#define INPUT_KEYB_RALT   108
#define INPUT_KEYB_BACKS  22
#define INPUT_KEYB_DELET  119
#define INPUT_KEYB_INSER  118
#define INPUT_KEYB_PRTSC  107

#define INPUT_KEYB_COMMA  59
#define INPUT_KEYB_PEROD  60
#define INPUT_KEYB_FSLSH  61
#define INPUT_KEYB_BSLSH  22
#define INPUT_KEYB_COLON  47
#define INPUT_KEYB_QUOTE  48
#define INPUT_KEYB_OBRAK  34
#define INPUT_KEYB_CBRAK  35
#define INPUT_KEYB_MINUS  20
#define INPUT_KEYB_EQUAL  21

#define INPUT_KEYB_NUM0   90
#define INPUT_KEYB_NUM1   87
#define INPUT_KEYB_NUM2   88
#define INPUT_KEYB_NUM3   89
#define INPUT_KEYB_NUM4   83
#define INPUT_KEYB_NUM5   84
#define INPUT_KEYB_NUM6   85
#define INPUT_KEYB_NUM7   79
#define INPUT_KEYB_NUM8   80
#define INPUT_KEYB_NUM9   81
#define INPUT_KEYB_NUMLK  77
#define INPUT_KEYB_NUMSL  106
#define INPUT_KEYB_NUMAS  63
#define INPUT_KEYB_NUMMI  82
#define INPUT_KEYB_NUMPL  86
#define INPUT_KEYB_NUMEN  104
#define INPUT_KEYB_NUMDL  91

#endif // __linux__

#endif // INPUT_H
