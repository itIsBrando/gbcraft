#include "level.h"
#include "tile.h"
#include "text.h"

#include <stdlib.h>
#include <string.h>
#include "keypad.h"

static BG_REGULAR *target_bg;


static inline u16 lvl_get_size(const level_t *lvl)
{
    return lvl->size;
}

/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns the raw, stored tile at (x, y)
 */
tile_type_t lvl_get_tile_type(const level_t *lvl, u16 x, u16 y)
{
    return lvl->map[x + y * lvl_get_size(lvl)];
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

/**
 * @returns the GBA's controllable player
 */
inline ent_t *lvl_get_player(const level_t *lvl)
{
    return lvl->player;
}

/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns the data of the tile at (x, y)
 */
inline u8 lvl_get_data(level_t *lvl, u16 x, u16 y)
{
    return lvl->data[x + y * lvl->size];
}


/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @param v 8-bit data value
 */
inline void lvl_set_data(level_t *lvl, u16 x, u16 y, u8 v)
{
    lvl->data[x + y * lvl->size] = v;
}


void lvl_set_tile(level_t *lvl, u16 x, u16 y, const tile_t *tile)
{
    u16 i = x + y * lvl_get_size(lvl);
    lvl->map[i] = tile->type;
    lvl->data[i] = 0;
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
    lvl->size = 64;

    // do map generation based on layer @todo
    memset(lvl->data, 0, LEVEL_SIZE);

    return lvl;
}


void lvl_blit(level_t *lvl)
{
    text_print("RENDERING WORLD", 0, 2);
    // tile_render_use_recursion(false);

    for(u16 y = 0; y < lvl->size; y++)
    {
        for(u16 x = 0; x < lvl->size-1; x++)
        {
            const tile_t *tile = lvl_get_tile(lvl, x, y);
            tile_render(target_bg, lvl, tile, x, y);
        }
    }

    tile_render_use_recursion(true);
}

/**
 * Dictates which background should be used for drawing
 */
void lvl_set_target_background(BG_REGULAR *bg)
{
    target_bg = bg;
}