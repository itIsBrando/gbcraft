#include "level.h"
#include "tile.h"
#include "item.h"
#include "player.h"
#include "entity.h"
#include "menu.h"

#include "random.h"
#include "text.h"
#include "bg.h"


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
    },
    [14]={ // floor
        .interact=tile_floor_interact,
    },
};


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
    CREATE_TILE(TILE_FLOOR_WOOD, 1, TILE_INDEXING_SINGLE_8x8, 14),
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


bool tile_wheat_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    level_t *lvl = ent->level;
    const uint px = lvl_to_pixel_x(x), py = lvl_to_pixel_y(y);

    lvl_set_tile(lvl, x, y, tile_get(TILE_MUD));
    ent_item_new(lvl, px, py, &ITEM_WHEAT, 1);
    ent_item_new(lvl, px, py, &ITEM_SEED, (rnd_random() & 0x3));
    
    return true;
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