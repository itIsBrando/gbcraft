#include "level.h"
#include "tile.h"
#include "entity.h"

#include "text.h"
#include <stdlib.h>
#include <string.h>
#include "keypad.h"
#include "memory.h"

static BG_REGULAR *target_bg;
uint lvl_ticks;


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
    lvl->mob_density = 0;

    // do map generation based on layer @todo
    memset(lvl->data, 0, LEVEL_SIZE);

    return lvl;
}


/**
 * Converts an absolute tile x coordinate to a pixel x coordinate
 * @returns x coordinate between 0-320
 */
inline u16 lvl_to_pixel_x(level_t *lvl, u16 tx)
{
    tx <<= 4;
    tx -= bg_get_scx(main_background);
    return tx;
}


/**
 * Converts an absolute tile y coordinate to a pixel y coordinate
 * @returns y coordinate between 0-240
 */
inline u16 lvl_to_pixel_y(level_t *lvl, u16 ty)
{
    ty <<= 4;
    ty -= bg_get_scy(main_background);
    return ty;
}


/**
 * Tries to get a valid spawn position for enemies
 * @param x pointer to x coordinate. Will be updated to a random position
 * @param y pointer to y coordinate. Will be updated to a random position
 * @returns true if in a spawnable range. if false, x and y coordinates are still modified
 */
bool lvl_try_spawn_position(level_t *lvl, uint *x, uint *y)
{
    // prevent too many mobs
    if(lvl->mob_density > 6 + lvl->layer)
        return false;

    ent_t *plr = lvl_get_player(lvl);
    *x = rnd_random_bounded(0, LEVEL_WIDTH) << 4;
    *y = rnd_random_bounded(0, LEVEL_HEIGHT) << 4;

    int px = plr->x + bg_get_scx(main_background);
    int py = plr->y + bg_get_scy(main_background);
    
    if(abs(px - *x) + abs(py - *y) > 80)
        return false;

    const tile_type_t t = lvl_get_tile_type(lvl, *x >> 4, *y >> 4);
    if(t != TILE_GRASS)
        return false;

    // if ANY entity exists here, then do not proceed with spawning
    ent_t *ents[1];
    if(ent_get_all_stack(lvl, ents, *x, *y, 1))
        return false;
    
    lvl->mob_density++;

    return true;
}

/**
 * Tries to spawn enemies
 * @param tries number of spawns to try
 */
void lvl_try_spawn(level_t *level, uint tries)
{
    uint x, y;
    for(uint i = 0; i < tries; i++)
    {
        // if we cannot find a spawn location, then do not add
        if(!lvl_try_spawn_position(level, &x, &y))
            continue;
        // center coordinates based on top left rather than screen
        x -= bg_get_scx(main_background);
        y -= bg_get_scy(main_background);

        if((rnd_random() & 0x3) == 0)
        {
		    ent_add(level, ENT_TYPE_ZOMBIE, x, y);
        } else {
            ent_add(level, ENT_TYPE_SLIME, x, y);
        }
    }
}


void lvl_blit(level_t *lvl)
{
    text_print("RENDERING WORLD", 0, 2);
    tile_render_use_recursion(false);

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