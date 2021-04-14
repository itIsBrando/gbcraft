#include "terraingen.h"

#include "memory.h"


/**
 * Uses perlin noise to generate a map
 */
void gen_generate(level_t *lvl)
{
    u16 x, y;
    for(y = 0; y < LEVEL_HEIGHT; y++)
    {
        for(x = 0; x < LEVEL_WIDTH; x++)
        {
            lvl->map[x + LEVEL_WIDTH * y] = rnd_random() & 0x1;
        }
    }
}