#include "level.h"
#include "tile.h"
#include "text.h"

#include <stdlib.h>
#include <string.h>

static BG_REGULAR *target_bg;

/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns the raw, stored tile at (x, y)
 */
tile_type_t lvl_get_tile_type(const level_t *lvl, u16 x, u16 y)
{
    return lvl->map[x + y * LEVEL_WIDTH];
}


/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns the details of a tile at (x, y)
 */
inline const tile_t *lvl_get_tile(level_t *lvl, u16 x, u16 y)
{
    return tile_get(lvl_get_tile_type(lvl, x, y));
}

void lvl_set_tile(level_t *lvl, u16 x, u16 y, const tile_t *tile)
{
    lvl->map[x + y * LEVEL_WIDTH] = tile->type;
    tile_render(target_bg, lvl, tile, x, y);
}


/**
 * Generates a new level
 * @returns a pointer to the new level FROM THE HEAP
 * @note must be freed
 */
level_t *lvl_new(u16 layer, level_t *parent)
{
    level_t *lvl = malloc(sizeof(level_t));

    if(!lvl)
        text_error("Could not allocate heap for level");
    lvl->layer = layer;
    lvl->parent = (struct level_t*)parent;


    // do map generation based on layer @todo
    memset(lvl->map, TILE_GRASS, LEVEL_SIZE);

    return lvl;
}


void lvl_blit(level_t *lvl)
{
    for(u16 y = 0; y < (LEVEL_HEIGHT >> 1); y++)
    {
        for(u16 x = 0; x < (LEVEL_WIDTH >> 1); x++)
        {
            const tile_t *tile = lvl_get_tile(lvl, x, y);
            tile_render(target_bg, lvl, tile, x, y);

        }
    }

}

/**
 * Dictates which background should be used for drawing
 */
void lvl_set_target_background(BG_REGULAR *bg)
{
    target_bg = bg;
}