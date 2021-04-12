#include "text.h"

static BG_REGULAR *target_background;
static uint tile_offset;


void text_print(char *string, uint x, uint y)
{
    while(*string) {
        const uint tile = (uint)(*string++) + tile_offset - '.';
        bg_write_tile(target_background, x++, y, tile);
    }
}


void text_uint(uint num, uint x, uint y)
{
    char buffer[6];
    uint i = 4;
    do {
        buffer[i--] = (num % 10) + '0';
        num /= 10;
    } while(num);

    buffer[5] = 0;
    text_print(buffer + 1 + i, x, y);
}


void text_init(BG_REGULAR *bg, uint startTile)
{
    target_background = bg;
    tile_offset = startTile;
}


void text_error(char *string)
{
    bg_fill(target_background, 0, 0, 320/8, 160/8, 0);
    text_print(string, 0, 0);
}