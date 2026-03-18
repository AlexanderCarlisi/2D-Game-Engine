#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>
// #include <winuser.h>

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

#else

#define INPUT_KEYBOARD_MAX    133

#define INPUT_KEYB_Q      'Q'   // 0x51
#define INPUT_KEYB_W      'W'   // 0x57
#define INPUT_KEYB_E      'E'   // 0x45
#define INPUT_KEYB_R      'R'   // 0x52
#define INPUT_KEYB_T      'T'   // 0x54
#define INPUT_KEYB_Y      'Y'   // 0x59
#define INPUT_KEYB_U      'U'   // 0x55
#define INPUT_KEYB_I      'I'   // 0x49
#define INPUT_KEYB_O      'O'   // 0x4F
#define INPUT_KEYB_P      'P'   // 0x50

#define INPUT_KEYB_A      'A'   // 0x41
#define INPUT_KEYB_S      'S'   // 0x53
#define INPUT_KEYB_D      'D'   // 0x44
#define INPUT_KEYB_F      'F'   // 0x46
#define INPUT_KEYB_G      'G'   // 0x47
#define INPUT_KEYB_H      'H'   // 0x48
#define INPUT_KEYB_J      'J'   // 0x4A
#define INPUT_KEYB_K      'K'   // 0x4B
#define INPUT_KEYB_L      'L'   // 0x4C

#define INPUT_KEYB_Z      'Z'   // 0x5A
#define INPUT_KEYB_X      'X'   // 0x58
#define INPUT_KEYB_C      'C'   // 0x43
#define INPUT_KEYB_V      'V'   // 0x56
#define INPUT_KEYB_B      'B'   // 0x42
#define INPUT_KEYB_N      'N'   // 0x4E
#define INPUT_KEYB_M      'M'   // 0x4D

#define INPUT_KEYB_1      '1'   // 0x31
#define INPUT_KEYB_2      '2'   // 0x32
#define INPUT_KEYB_3      '3'   // 0x33
#define INPUT_KEYB_4      '4'   // 0x34
#define INPUT_KEYB_5      '5'   // 0x35
#define INPUT_KEYB_6      '6'   // 0x36
#define INPUT_KEYB_7      '7'   // 0x37
#define INPUT_KEYB_8      '8'   // 0x38
#define INPUT_KEYB_9      '9'   // 0x39
#define INPUT_KEYB_0      '0'   // 0x30

// #define INPUT_KEYB_ESC    VK_ESCAPE  // 0x1B
// #define INPUT_KEYB_TILDE  VK_OEM_3   // 0xC0 (~ / `)
// #define INPUT_KEYB_TAB    VK_TAB     // 0x09
// #define INPUT_KEYB_CAPSL  VK_CAPITAL // 0x14
// #define INPUT_KEYB_LSHFT  VK_LSHIFT  // 0xA0
// #define INPUT_KEYB_LCTRL  VK_LCONTROL// 0xA2
// #define INPUT_KEYB_LALT   VK_LMENU   // 0xA4
// #define INPUT_KEYB_MOD    VK_LWIN    // 0x5B (Windows Key)
// #define INPUT_KEYB_SPACE  VK_SPACE   // 0x20
// #define INPUT_KEYB_RSHFT  VK_RSHIFT  // 0xA1
// #define INPUT_KEYB_RCTRL  VK_RCONTROL// 0xA3
// #define INPUT_KEYB_RALT   VK_RMENU   // 0xA5
// #define INPUT_KEYB_BACKS  VK_BACK    // 0x08
// #define INPUT_KEYB_DELET  VK_DELETE  // 0x2E
// #define INPUT_KEYB_INSER  VK_INSERT  // 0x2D
// #define INPUT_KEYB_PRTSC  VK_SNAPSHOT// 0x2C

// #define INPUT_KEYB_COMMA  VK_OEM_COMMA  // 0xBC
// #define INPUT_KEYB_PEROD  VK_OEM_PERIOD // 0xBE
// #define INPUT_KEYB_FSLSH  VK_OEM_2      // 0xBF (/ ?)
// #define INPUT_KEYB_BSLSH  VK_OEM_5      // 0xDC (\ |)
// #define INPUT_KEYB_COLON  VK_OEM_1      // 0xBA (; :)
// #define INPUT_KEYB_QUOTE  VK_OEM_7      // 0xDE (' ")
// #define INPUT_KEYB_OBRAK  VK_OEM_4      // 0xDB ([ {)
// #define INPUT_KEYB_CBRAK  VK_OEM_6      // 0xDD (] })
// #define INPUT_KEYB_MINUS  VK_OEM_MINUS  // 0xBD (- _)
// #define INPUT_KEYB_EQUAL  VK_OEM_PLUS   // 0xBB (= +)

// #define INPUT_KEYB_NUM0   VK_NUMPAD0 // 0x60
// #define INPUT_KEYB_NUM1   VK_NUMPAD1 // 0x61
// #define INPUT_KEYB_NUM2   VK_NUMPAD2 // 0x62
// #define INPUT_KEYB_NUM3   VK_NUMPAD3 // 0x63
// #define INPUT_KEYB_NUM4   VK_NUMPAD4 // 0x64
// #define INPUT_KEYB_NUM5   VK_NUMPAD5 // 0x65
// #define INPUT_KEYB_NUM6   VK_NUMPAD6 // 0x66
// #define INPUT_KEYB_NUM7   VK_NUMPAD7 // 0x67
// #define INPUT_KEYB_NUM8   VK_NUMPAD8 // 0x68
// #define INPUT_KEYB_NUM9   VK_NUMPAD9 // 0x69
// #define INPUT_KEYB_NUMLK  VK_NUMLOCK // 0x90
// #define INPUT_KEYB_NUMSL  VK_DIVIDE  // 0x6F (/)
// #define INPUT_KEYB_NUMAS  VK_MULTIPLY// 0x6A (*)
// #define INPUT_KEYB_NUMMI  VK_SUBTRACT// 0x6D (-)
// #define INPUT_KEYB_NUMPL  VK_ADD     // 0x6B (+)
// #define INPUT_KEYB_NUMEN  VK_RETURN  // 0x0D (Numpad Enter is also VK_RETURN)
// #define INPUT_KEYB_NUMDL  VK_DECIMAL // 0x6E (.)

#endif // __linux__

#endif // INPUT_H
