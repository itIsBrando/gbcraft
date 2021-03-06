#include "level.h"
#include "tile.h"
#include "render.h"
#include "entity.h"

#include "text.h"
#include <stdlib.h>
#include <string.h>
#include "lib/keypad.h"
#include "random.h"

static BG_REGULAR *target_bg;
uint lvl_ticks;
level_t *world[4];
level_t *lvl_current = NULL;


/**
 * Bounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns the raw, stored tile at (x, y)
 */
tile_type_t lvl_get_tile_type(const level_t *lvl, uint x, uint y)
{
    if(x > 64 || y > 64)
        return TILE_NONE;
    return lvl->map[x + (y << 6)];
}


/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns the details of a tile at (x, y)
 */
inline const tile_t *lvl_get_tile(level_t *lvl, uint x, uint y)
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
inline u8 lvl_get_data(level_t *lvl, uint x, uint y)
{
    return lvl->data[x + (y << 6)];
}


/**
 * Unbounded check for a tile
 * @param x absolute tile x
 * @param y absolute tile y
 * @param v 8-bit data value
 */
inline void lvl_set_data(level_t *lvl, uint x, uint y, u8 v)
{
    lvl->data[x + (y << 6)] = v;
}


void lvl_set_tile(level_t *lvl, uint x, uint y, const tile_t *tile)
{
    uint i = x + (y << 6);
    lvl->map[i] = tile->type;
    lvl->data[i] = 0;
    tile_render(target_bg, lvl, tile, x, y);
}


/**
 * Generates a new level
 * @returns a pointer to the new level FROM THE HEAP
 * @note must be freed
 * @note no terrain is generated
 */
level_t *lvl_new(uint layer, level_t *parent)
{
    level_t *lvl = malloc(sizeof(level_t));

    if(!lvl)
        text_error("Could not allocate heap for level");
    lvl->layer = layer;
    lvl->parent = (struct level_t*)parent;
    lvl->mob_density = 0;
    lvl->ent_size = 0;
    lvl_set_world(lvl);

    memset(lvl->data, 0, LEVEL_SIZE);

    return lvl;
}


/**
 * Converts an absolute tile x coordinate to a pixel x coordinate
 * @returns x coordinate between 0-240
 */
inline uint lvl_to_pixel_x(int tx)
{
    tx *= 16;
    tx -= bg_get_scx(main_background);
    return tx;
}


/**
 * Converts an absolute tile y coordinate to a pixel y coordinate
 * @returns y coordinate between 0-160
 */
inline uint lvl_to_pixel_y(int ty)
{
    ty *= 16;
    ty -= bg_get_scy(main_background);
    return ty;
}


/**
 * Converts a screen pixel coordinate to an absolute tile coordinate
 * @param px [0-240)
 * @returns [0-64)
 */
inline uint lvl_to_tile_x(int px)
{
    px += bg_get_scx(main_background);
    if(px >= 0)
        return px >> 4;
    else
        return px / 16;
}


/**
 * Converts a screen pixel coordinate to an absolute tile coordinate
 * @param py [0-160)
 * @returns [0-64)
 */
inline uint lvl_to_tile_y(int py)
{
    py += bg_get_scy(main_background);
    if(py >= 0)
        return py >> 4;
    else
        return py / 16;
}


/**
 * Tries to get a valid spawn position for enemies
 * @param x pointer to x coordinate. Will be updated to a random position regardless of success
 * @param y pointer to y coordinate. Will be updated to a random position regardless of success
 * @returns true if in a spawnable range. if false, x and y coordinates are still modified
 */
bool lvl_try_spawn_position(level_t *lvl, uint *x, uint *y)
{
    // prevent too many mobs
    if(lvl->mob_density > 6 + lvl->layer)
        return false;

    ent_t *plr = lvl_get_player(lvl);
    *x = (rnd_random() & 63) << 4;
    *y = (rnd_random() & 63) << 4;

    int px = plr->x + bg_get_scx(main_background);
    int py = plr->y + bg_get_scy(main_background);
    
    int diffx = abs(px - *x);
    int diffy = abs(py - *y);
    int dist = diffx + diffy;
    if(dist > 95 || dist < 20)
        return false;

    const tile_type_t t = lvl_get_tile_type(lvl, *x >> 4, *y >> 4);
    
    if(!(t == TILE_GRASS || t == TILE_MUD))
        return false;

    // if ANY entity exists here, then do not proceed with spawning
    ent_t *ents[1];
    if(ent_get_all_stack(lvl, NULL, ents, *x, *y, 1))
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


static void lvl_unload(level_t *lvl)
{
    for(uint i = 0; i < lvl->ent_size; i++)
    {
        spr_free(lvl->entities[i].sprite);
        lvl->entities[i].sprite = NULL;
    }
}


/**
 * Draws and sets the target level to `newLevel`. The level must have its terrain generation before this call
 * @param newLevel MUST EXIST FOR DURATION OF BEING ACTIVE/LOADED
 */
void lvl_change_level(level_t *newLevel)
{
    if(lvl_current)
        lvl_unload(lvl_current);

    lvl_current = newLevel;
    lvl_set_world(newLevel);

    ent_t *e = lvl_current->entities;
    for(uint i = 0; i < lvl_current->ent_size; i++)
    {
        if(!e->sprite)
            e->sprite = spr_alloc(e->x, e->y, ent_get_tile(e));
        
        spr_set_size(e->sprite, SPR_SIZE_16x16);
        spr_set_priority(e->sprite, SPR_PRIORITY_HIGH);
        spr_set_pal(e->sprite, 0);
        e++;
    }

    lvl_blit();
}


/**
 * Sets the active level
 */
inline void lvl_set_world(level_t *lvl)
{
    world[lvl->layer] = lvl;
}


/**
 * @returns the loaded level
 */
inline level_t *lvl_get_current()
{
    return lvl_current;
}


/**
 * Draws the current level
 */
void lvl_blit()
{
    text_print("RENDERING WORLD", 0, 2);
    tile_render_use_recursion(false);

    for(u16 y = 0; y < 64; y++)
    {
        for(u16 x = 0; x < 64; x++)
        {
            const tile_t *tile = lvl_get_tile(lvl_current, x, y);
            tile_render(target_bg, lvl_current, tile, x, y);
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