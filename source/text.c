#include "text.h"
#include <gba_systemcalls.h>

static BG_REGULAR *target_background;
static u16 tile_offset;
static u16 pal_mask = 0;


void text_set_pal(u8 pal_num)
{
    pal_mask = (pal_num & 0xF) << 0xC;
}

inline void text_write_tile(u16 tile, const u16 x, const u16 y)
{
    bg_write_tile(target_background, x, y, tile | pal_mask);
}


inline void text_char(char character, const u16 x, const u16 y)
{
    u16 c = character == ' ' ? 0 : (u16)character + tile_offset - '+';
    text_write_tile(c, x, y);
}

void text_print(char *string, u16 x, u16 y)
{
    while(*string)
        text_char(*string++, x++, y);

}


void text_uint(u16 num, uint x, uint y)
{
    char buffer[6];
    u16 i = 4;
    do {
        buffer[i--] = (num % 10) + '0';
        num /= 10;
    } while(num);

    buffer[5] = 0;
    text_print(buffer + 1 + i, x, y);
}


void text_int(s16 num, u16 x, u16 y)
{
    text_char(num < 0 ? '-' : '+', x, y);
    text_uint(num < 0 ? -num : num, x + 1, y);
}


void text_init(BG_REGULAR *bg, u16 startTile)
{
    target_background = bg;
    tile_offset = startTile;
}


void text_error(char *string)
{
    bg_fill(target_background, 0, 0, 240/8, 1, 0);
    text_print(string, 0, 0);

    for (uint i = 0; i < 68; i++)
        VBlankIntrWait();
    
}