#include "keypad.h"

static u16 __keys, __keys_prev;

// #include <tonc_memmap.h>

inline void key_scan()
{
    __keys_prev = __keys;
    __keys = ~REG_KEYPAD & 0x03FF;
}


inline u16 key_pressed()
{
    return __keys;
}


inline u16 key_pressed_no_repeat()
{
    return (~__keys_prev) & __keys;
}
