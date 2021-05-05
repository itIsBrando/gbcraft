#include "keypad.h"

static uint __keys, __keys_prev;
static uint __since_scanned = 0;

// #include <tonc_memmap.h>

inline void key_scan()
{
    __keys_prev = __keys;
    __keys = ~REG_KEYPAD & 0x03FF;

    if(__since_scanned)
        __since_scanned--;
}


inline uint key_pressed()
{
    return __keys;
}


inline uint key_pressed_no_repeat()
{
    return (~__keys_prev) & __keys;
}


inline uint key_pressed_repeat_after(uint frames)
{
    if(__since_scanned == 0 && __keys)
        __since_scanned = frames;
    else if(__since_scanned)
        return 0;

    return __keys;
}