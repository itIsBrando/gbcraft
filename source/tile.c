#include "level.h"
#include "tile.h"
#include "text.h"
#include "bg.h"


const tile_t tile_tile_data[] = 
{
    { // grass floor
        .type=TILE_GRASS,
        .tiling={1},              // tile base
        .indexing=TILE_INDEXING_SINGLE_8x8,// indexing mode
        .onhurt=NULL, // onhurt
        .ontouch=NULL, // ontouch
        .maypass=NULL, // maypass
    },
    { // rock
        .type=TILE_ROCK,  // type
        .tiling={3},                // tile base
        .indexing=TILE_INDEXING_9PT,// indexing mode
        .onhurt=tile_rock_onhurt, // onhurt
        .ontouch=NULL, // ontouch
        .maypass=tile_no_pass, // maypass
    },
    { // water tile
        .type=TILE_WATER,
        .tiling={2},              // tile base
        .indexing=TILE_INDEXING_SINGLE_8x8,// indexing mode
        .onhurt=NULL, // onhurt
        .ontouch=NULL, // ontouch
        .maypass=NULL, // maypass
    },
    { // tree floor
        .type=TILE_TREE,
        .tiling={8},              // tile base
        .indexing=TILE_INDEXING_SINGLE_16x16,// indexing mode
        .onhurt=tile_tree_onhurt, // onhurt
        .ontouch=NULL, // ontouch
        .maypass=tile_no_pass, // maypass
    },
    { // wood plank
        .type=TILE_WOOD,
        .tiling={3},              // tile base
        .indexing=TILE_INDEXING_TOP_BOT,// indexing mode
        .onhurt=NULL, // onhurt
        .ontouch=NULL, // ontouch
        .maypass=tile_no_pass, // maypass
    },
};



/**
 * Gets a mask of the tiles surrounding this one
 * @param level to look at
 * @param type tile to check for matching nearby
 * @param x absolute x
 * @param y absolute y
 * @todo add support for diagnol
 */
tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, uint x, uint y)
{
    int dx[] = {-1, 1, 0, 0, 0, 0, 0, 0};
    int dy[] = {0, 0, -1, 1, 0, 0, 0, 0};

    tile_surround_mask mask = 0;

    for(uint i = 0; i < 8; i++)
    {
        mask <<= 1;
        if(type == lvl_get_tile_type(lvl, x + dx[i], y + dy[i]))
            mask |= 1;
    }

    return mask;
}


static void tile_render_single_8x8(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, uint x, uint y)
{
    bg_fill(bg, x << 1, y << 1, 2, 2, tile->tiling.center);
}


static void tile_render_single_16x16(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, uint x, uint y)
{
    u16 data[4] = {tile->tiling.topRight,
        tile->tiling.topRight + 1,
        tile->tiling.topRight + 32,
        tile->tiling.topRight + 33};
    bg_rect(bg, x << 1, y << 1, 2, 2, data);
}

static void tile_render_top_bot(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, uint x, uint y)
{
    tile_surround_mask mask = tile_get_surrounding((level_t *)lvl, tile->type, x, y);

    if(mask & DIRECTION_DOWN)
    {
        u16 data[4] = {tile->tiling.topRight,
            tile->tiling.topRight + 1,
            tile->tiling.topRight,
            tile->tiling.topRight + 1};
        bg_rect(bg, x << 1, y << 1, 2, 2, data);
    } else
        tile_render_single_16x16(bg, lvl, tile, x, y);

    if(mask & DIRECTION_UP)
        tile_render_top_bot(bg, lvl, tile, x, y - 1);
}


// @todo needs implmentation
static void tile_render_9pt(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, uint x, uint y)
{
    bg_fill(bg, x, y, 2, 2, 0);
    text_error("INDEXING MODE NOT SUPPORTED");
}


/**
 * @todo add support for TILE_INDEXING_9PT
 */
void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, uint x, uint y)
{
    void (*table[])(const BG_REGULAR *, const level_t *, tile_t *, uint, uint) =
    {
        tile_render_9pt, tile_render_top_bot,
        tile_render_single_8x8, tile_render_single_16x16
    };

    table[tile->indexing](bg, lvl, (tile_t*)tile, x, y);
}


bool tile_no_pass(ent_t *e)
{
    return false;
}


void tile_rock_onhurt(ent_t *e)
{
    
}


void tile_tree_onhurt(ent_t *e)
{
    
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