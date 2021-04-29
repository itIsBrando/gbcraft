#include "terraingen.h"
#include "tile.h"
#include "level.h"

#include "window.h"
#include "keypad.h"
#include "text.h"
#include "memory.h"
#include <stdlib.h>

typedef u16 terr_t;

static u8 w, h;
static terr_t *_values;

void setSample(u8 x, u8 y, terr_t v)
{
    _values[(x & (w - 1)) + w * (y & (h - 1))] = v;
}

terr_t getSample(u8 x, u8 y)
{
    return (_values[(x & (w - 1)) + w * (y & (h - 1))]);
}


/**
 * @param mapSize width/height of map
 */
terr_t *noise(u16 mapSize, u8 featureSize)
{
    text_print("GENERATING NOISE", 0, 0);
    w = mapSize;
    h = mapSize;
    terr_t *values = malloc(mapSize * mapSize * sizeof(terr_t));
    _values = values;

    u8 displacement = featureSize;
    u8 width = w - 1;
    // diamond step
    terr_t r = rnd_random_bounded(100, 256);
    setSample(0, 0, r);
    setSample(width, 0, r);
    setSample(0, width, r);
    setSample(width, width, r);

    while(width > 1)
    {
        u8 halfSize = width >> 1;

        // do diamond step
        for(u8 y = 0; y < h-1; y += width)
        {
            for(u8 x = 0; x < w-1; x += width)
            {
                terr_t avg = getSample(x, y)
                + getSample(x + width, y)
                + getSample(x, y + width)
                + getSample(x + width, y + width);
                avg >>= 2;
                avg += rnd_random_bounded(-displacement, displacement);
                setSample(x + halfSize, y + halfSize, avg);
            }
        }

        // do square step
        for(uint y = 0; y < h-1; y += halfSize)
        {
            for(uint x = 0; x < w-1; x += halfSize)
            {
                terr_t avg1 = getSample(x, y)
                + getSample(x + width, y)
                + getSample(x + halfSize, y + halfSize)
                + getSample(x + halfSize, y - halfSize);
                terr_t avg2 = getSample(x, y)
                + getSample(x, y + width)
                + getSample(x + halfSize, y + halfSize)
                + getSample(x - halfSize, y + halfSize);

                avg1 >>= 2;
                avg1 += rnd_random_bounded(-displacement, displacement);
                avg2 >>= 2;
                avg2 += rnd_random_bounded(-displacement, displacement);
                setSample(x + halfSize, y, avg1);
                setSample(x, y + halfSize, avg2);
            }
        }

        displacement >>= 1;
        if(displacement == 0)
            displacement = 1;

        width >>= 1;
    }

    text_print("FINISHED NOISE GENERATION", 0, 1);

    return values;
}


/**
 * Call to generate terrain for a level
 * @param lvl must be filled with layer number
 */
void gen_generate(level_t *lvl)
{
    
    win_move(win_get_0(), 0, 0, 240, 160);
    
    switch (lvl->layer)
    {
    case 0:
        gen_generate_overworld(lvl);
        gen_make_stairs_down(lvl);
        return;
    case 3:
        gen_generate_underworld(lvl);
        return;
    default:
        gen_generate_underworld(lvl);
        gen_make_stairs_down(lvl);
        return;
    }
}


/**
 * Uses diamond square to generate the underworld map
 */
void gen_generate_overworld(level_t *lvl)
{
    terr_t *n1 = noise(lvl->size, 32);

    for(uint index = 0; index < LEVEL_SIZE; index++)
    {
            u8 tile = TILE_WATER;

            if(n1[index] > 1000)
                tile = TILE_STONE;
            else if(n1[index] > 800)
                tile = TILE_TREE;
            else if(n1[index] > 50)
                tile = TILE_GRASS;
            
            lvl->map[index] = tile;
    }

    free(n1);
}

void gen_make_stairs_down(level_t *lvl)
{
    // spawn 9 staircases
    for(uint i = 0; i < 9; i++)
    {
        uint iter = 0, x, y;
        do {
            x = rnd_random() & 0x7F;
            y = rnd_random() & 0x7F;
            iter++;
        } while(lvl_get_tile_type(lvl, x, y) != TILE_STONE && iter < 10);
        lvl_set_tile(lvl, x, y, tile_get(TILE_STAIRS));
    }
}


/**
 * Uses diamond square to generate a map
 * @param lvl pointer to map of level+
 */
void gen_generate_underworld(level_t *lvl)
{
    terr_t *n1 = noise(lvl->size, 8);

    for(uint index = 0; index < LEVEL_SIZE; index++)
    {
            u8 tile = TILE_STONE;

            if(n1[index] > 1500)
                tile = TILE_IRON;
            else if(n1[index] > 50)
                tile = TILE_MUD;
            
            lvl->map[index] = tile;
    }

    free(n1);
}