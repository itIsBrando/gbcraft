#include "level.h"
#include "tile.h"
#include "item.h"
#include "player.h"
#include "entity.h"
#include "menu.h"

#include "memory.h"
#include "text.h"
#include "bg.h"

static BG_REGULAR *target_bg;

const tile_event_t tile_events[] = {
    [0]={ // grass floor
        .ontouch=NULL,
        .maypass=tile_grass_maypass,
        .interact=tile_grass_interact,
    },
    [1]={ // water tile
        .ontouch=NULL,
        .maypass=NULL,
        .maypass=tile_water_maypass,
    },
    [2]={ // tree tile
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_tree_interact
    },
    [3]={ // wood plank
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_wood_interact
    },
    [4]={ // rock
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_stone_interact,
    },
    [5]={ // ore
        .maypass=tile_no_pass,
        .interact=tile_iron_interact,
    },
    [6]={ // stairs down
        .ontouch=tile_stair_down_ontouch,
        .maypass=NULL,
        .interact=NULL,
    },
    [7]={ // stairs up
        .ontouch=tile_stair_up_ontouch,
    },
    [8]={ // door closed
        .interact=tile_door_closed_interact,
        .maypass=tile_no_pass,
    },
    [9]={ // door open
        .interact=tile_door_open_interact,
    },
    [10]={ // mud
        .onrandomtick=tile_mud_tick,
    },
    [11]={ // sapling
        .onrandomtick=tile_sapling_tick,
    },
    [12]={ // seed
        .onrandomtick=tile_seed_tick,
    },
    [13]={ // fullly grown wheat
        .interact=tile_wheat_interact,
    }
};


// @todo add this
/**
 * @param _type `tile_type_t`
 * @param _tile tile index
 * @param _indexing_mode `tile_indexing_mode_t`
 * @param _event_num index in `tile_events`
 */
#define CREATE_TILE(_type, _tile, _indexing_mode, _event_num, ...) {\
 .type=_type,\
 .tiling={_tile},\
 .indexing=_indexing_mode,\
 .event=&tile_events[_event_num],\
 __VA_ARGS__\
}

// order irrelevant
const tile_t tile_tile_data[] = 
{
    CREATE_TILE(TILE_GRASS, 45, TILE_INDEXING_9PT, 0),
    CREATE_TILE(TILE_WATER, 48, TILE_INDEXING_9PT, 1),
    CREATE_TILE(TILE_TREE, 7, TILE_INDEXING_SINGLE_16x16, 2),
    CREATE_TILE(TILE_WOOD, 3, TILE_INDEXING_TOP_BOT, 3),
    CREATE_TILE(TILE_STONE, 42, TILE_INDEXING_9PT, 4),
    CREATE_TILE(TILE_IRON, 65, TILE_INDEXING_SINGLE_16x16, 5),
    CREATE_TILE(TILE_GOLD, 65, TILE_INDEXING_SINGLE_16x16, 5),
    CREATE_TILE(TILE_STAIR_DOWN, 129, TILE_INDEXING_SINGLE_16x16, 6),
    CREATE_TILE(TILE_STAIR_UP, 131, TILE_INDEXING_SINGLE_16x16, 7),
    CREATE_TILE(TILE_MUD, 34, TILE_INDEXING_SINGLE_8x8, 10),
    CREATE_TILE(TILE_DOOR_CLOSED, 71, TILE_INDEXING_SINGLE_16x16, 8),
    CREATE_TILE(TILE_DOOR_OPEN, 69, TILE_INDEXING_SINGLE_16x16, 9),
    CREATE_TILE(TILE_FLOOR_WOOD, 1, TILE_INDEXING_SINGLE_8x8, 0),
    CREATE_TILE(TILE_SAPLING, 67, TILE_INDEXING_SINGLE_16x16, 11),
    CREATE_TILE(TILE_SEEDED_MUD, 2, TILE_INDEXING_SINGLE_8x8, 12),
    CREATE_TILE(TILE_WHEAT_MUD, 33, TILE_INDEXING_SINGLE_8x8, 13),
};



/**
 * Gets a mask of the tiles surrounding this one
 * @param level to look at
 * @param type tile to check for matching nearby
 * @param x absolute x
 * @param y absolute y
 */
tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, u16 x, u16 y)
{
    tile_surround_mask mask = 0;

    for(uint i = 0; i < 8; i++)
    {
        mask <<= 1;
        if(type == lvl_get_tile_type(lvl, x + dir_get_x(i), y + dir_get_y(i)))
            mask |= 1;
    }

    return mask;
}


static void tile_render_single_8x8(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    bg_fill(target_bg, x, y, 2, 2, tile->tiling.center);

    // @todo maybe make a call to `tile_get_surrounding instead??
    tile_render_nearby(SURROUNDING_DOWN | SURROUNDING_LEFT | SURROUNDING_UP | SURROUNDING_RIGHT, lvl, tile, x, y);
}


static void tile_render_single_16x16(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    u8 data[4] = {tile->tiling.topRight,
        tile->tiling.topRight + 1,
        tile->tiling.topRight + 32,
        tile->tiling.topRight + 33};

    bg_rect(target_bg, x, y, 2, 2, data);

    // @todo maybe make a call to `tile_get_surrounding instead??
    tile_render_nearby(SURROUNDING_DOWN | SURROUNDING_LEFT | SURROUNDING_UP | SURROUNDING_RIGHT, lvl, tile, x, y);
}


static void tile_render_top_bot(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    tile_surround_mask mask = tile_get_surrounding((level_t *)lvl, tile->type, x >> 1, y >> 1);

    if(mask & SURROUNDING_DOWN)
    {
        u8 data[4] = {tile->tiling.topRight,
            tile->tiling.topRight + 1,
            tile->tiling.topRight,
            tile->tiling.topRight + 1};
        bg_rect(target_bg, x, y, 2, 2, data);
    } else
        tile_render_single_16x16(lvl, tile, x, y);

    if(mask & SURROUNDING_UP)
        tile_render_top_bot(lvl, tile, x, y - 1);

    tile_render_nearby(mask, lvl, tile, x, y);
}

static bool _use_recursion;


static tile_surround_mask _render_9pt(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    tile_surround_mask mask = tile_get_surrounding((level_t *)lvl, tile->type, x >> 1, y >> 1);
    //| tile_get_surrounding((level_t *)lvl, tile->connect_to, x >> 1, y >> 1);
    const u8 t = tile->tiling.center;

    if(tile->indexing != TILE_INDEXING_9PT)
        return mask;

    bool l = mask & SURROUNDING_LEFT;
    bool r = mask & SURROUNDING_RIGHT;
    bool u = mask & SURROUNDING_UP;
    bool d = mask & SURROUNDING_DOWN;

    bool dl = mask & SURROUNDING_LEFT_DOWN;
    bool ul = mask & SURROUNDING_LEFT_UP;
    bool dr = mask & SURROUNDING_RIGHT_DOWN;
    bool ur = mask & SURROUNDING_RIGHT_UP;

    u8 corners[] = {t - 33, t - 31, t + 31, t + 33};
    
    // draw top left tile
    if(l && u)
        corners[0] = t;
    else if(l)
        corners[0] = t - 32; // top tile
    else if(u)
        corners[0] = t - 1;
    
    // top right tile
    if(r && u)
        corners[1] = t; // middle tile
    else if(r)
        corners[1] = t - 32; // top tile
    else if(u)
        corners[1] = t + 1; // right tile

    // bottom left
    if(l && d)
        corners[2] = t; // middle tile
    else if(l)
        corners[2] = t + 32; // bottom tile
    else if(d)
        corners[2] = t - 1; // left tile
    
    // bottom right
    if(r && d)
        corners[3] = t; // middle tile
    else if(r)
        corners[3] = t + 32; // bottom tile
    else if(d)
        corners[3] = t + 1; // left tile

    if(!dl && d && l)
        corners[2] = t + 63; // down left

    if(!dr && d && r)
        corners[3] = t + 64; // down right

    if(!ul && u && l)
        corners[0] = t + 65; // up left
        
    if(!ur && u && r)
        corners[1] = t + 96; // up right
    
    bg_rect(target_bg, x, y, 2, 2, corners);

    return mask;
}


void tile_render_nearby(tile_surround_mask mask, const level_t *lvl, const tile_t *tile, u16 x, u16 y)
{

    if(!_use_recursion)
        return;

    for(uint i = 0; i < 8; i++)
    {
        int xx = x + (dir_get_x(i) << 1),
            yy = y + (dir_get_y(i) << 1);
        if(xx < 0 || yy < 0 || xx >= LEVEL_WIDTH || yy >= LEVEL_HEIGHT)
            continue;
        
        const tile_t *t = lvl_get_tile((level_t*)lvl, xx >> 1, yy >> 1);

        if(t->indexing == TILE_INDEXING_9PT)
            _render_9pt(lvl, (tile_t*)t, xx, yy);
        
    }
}


static void tile_render_9pt(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    tile_surround_mask mask = _render_9pt(lvl, tile, x, y);

    tile_render_nearby(mask, lvl, tile, x, y);
}


void tile_render_use_recursion(bool b)
{
    _use_recursion = b;
}


/**
 * @param x [0, level.size).
 * @param y [0, level.size). Absolute tile coordinate
 * @todo add support for TILE_INDEXING_9PT
 */
void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, u16 x, u16 y)
{
    void (*table[])(const level_t *, tile_t *, u16, u16) =
    {
        tile_render_9pt, tile_render_top_bot,
        tile_render_single_8x8, tile_render_single_16x16
    };

    target_bg = (BG_REGULAR*)bg;

    table[tile->indexing](lvl, (tile_t*)tile, x << 1, y << 1);
}


bool tile_water_maypass(ent_t *e)
{
    if(e->type == ENT_TYPE_PLAYER) {
        plr_set_swim(e, true);
        return true;
    }
    else
        return false;
}


bool tile_no_pass(ent_t *e)
{
    return false;
}


void tile_wheat_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    level_t *lvl = ent->level;
    const uint px = lvl_to_pixel_x(x), py = lvl_to_pixel_y(y);

    lvl_set_tile(lvl, x, y, tile_get(TILE_MUD));
    ent_item_new(lvl, px, py, &ITEM_WHEAT, 1);
    ent_item_new(lvl, px, py, &ITEM_SEED, (rnd_random() & 0x3));
}


/**
 * @param x tile x
 * @param y tile y
 */
void tile_mud_tick(level_t *lvl, uint x, uint y)
{
    // only if we are on the surface
    if(!lvl->layer)
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
}


/**
 * @param x tile x
 * @param y tile y
 */
void tile_sapling_tick(level_t *lvl, uint x, uint y)
{
    uint data = lvl_get_data(lvl, x, y) + 1;

    if(data > 5)
        lvl_set_tile(lvl, x, y, tile_get(TILE_TREE));
    else
        lvl_set_data(lvl, x, y, data);
}


/**
 * @param x tile x
 * @param y tile y
 */
void tile_seed_tick(level_t *lvl, uint x, uint y)
{
    uint data = lvl_get_data(lvl, x, y) + 2;

    if(data > 5)
        lvl_set_tile(lvl, x, y, tile_get(TILE_WHEAT_MUD));
    else
        lvl_set_data(lvl, x, y, data);
}


/**
 * Gets a `tile_t` from a type
 */
const tile_t *tile_get(tile_type_t type)
{
    for(uint i = 0; i < sizeof(tile_tile_data) / sizeof(tile_tile_data[0]); i++)
    {
        if(type == tile_tile_data[i].type)
            return &tile_tile_data[i];
    }

    return NULL;
}