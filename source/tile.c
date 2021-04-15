#include "level.h"
#include "tile.h"
#include "item.h"

#include "text.h"
#include "bg.h"

const tile_event_t tile_events[] = {
    { // grass floor
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=NULL,
    },
    { // water tile
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=NULL,
    },
    { // tree tile
        .onhurt=tile_tree_onhurt,
        .ontouch=NULL,
        .maypass=tile_no_pass,
    },
    { // wood plank
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_wood_interact
    },
    { // rock
        .onhurt=tile_stone_onhurt,
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_stone_interact,
    }
};


const tile_t tile_tile_data[] = 
{
    { // grass floor
        .type=TILE_GRASS,
        .tiling={1},              // tile base
        .indexing=TILE_INDEXING_SINGLE_8x8,// indexing mode
        .event=&tile_events[0],
    },
    { // water tile
        .type=TILE_WATER,
        .tiling={2},              // tile base
        .indexing=TILE_INDEXING_SINGLE_8x8,// indexing mode
        .event=&tile_events[1],
    },
    { // tree
        .type=TILE_TREE,
        .tiling={8},              // tile base
        .indexing=TILE_INDEXING_SINGLE_16x16,// indexing mode
        .event=&tile_events[2]
    },
    { // wood plank
        .type=TILE_WOOD,
        .tiling={3},              // tile base
        .indexing=TILE_INDEXING_TOP_BOT,// indexing mode
        .event=&tile_events[3]
    },
    { // rock
        .type=TILE_STONE,  // type
        .tiling={5},                // tile base
        .indexing=TILE_INDEXING_9PT,// indexing mode
        .event=&tile_events[4]
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
tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, u16 x, u16 y)
{
    int dx[] = {-1, 1, 0, 0, 0, 0, 0, 0};
    int dy[] = {0, 0, -1, 1, 0, 0, 0, 0};

    tile_surround_mask mask = 0;

    for(u16 i = 0; i < 8; i++)
    {
        mask <<= 1;
        if(type == lvl_get_tile_type(lvl, x + dx[i], y + dy[i]))
            mask |= 1;
    }

    return mask;
}


static void tile_render_single_8x8(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    bg_fill(bg, x, y, 2, 2, tile->tiling.center);
}


static void tile_render_single_16x16(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    u8 data[4] = {tile->tiling.topRight,
        tile->tiling.topRight + 1,
        tile->tiling.topRight + 32,
        tile->tiling.topRight + 33};
    bg_rect(bg, x, y, 2, 2, data);
}

static void tile_render_top_bot(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    tile_surround_mask mask = tile_get_surrounding((level_t *)lvl, tile->type, x >> 1, y >> 1);

    if(mask & SURROUNDING_DOWN)
    {
        u8 data[4] = {tile->tiling.topRight,
            tile->tiling.topRight + 1,
            tile->tiling.topRight,
            tile->tiling.topRight + 1};
        bg_rect(bg, x, y, 2, 2, data);
    } else
        tile_render_single_16x16(bg, lvl, tile, x, y);

    if(mask & SURROUNDING_UP)
        tile_render_top_bot(bg, lvl, tile, x, y - 1);
}


// @todo needs implmentation
static void tile_render_9pt(const BG_REGULAR *bg, const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    bg_fill(bg, x, y, 2, 2, tile->tiling.center);
    text_error("INDEXING MODE NOT SUPPORTED");
}


/**
 * @param x [0, LEVEL_WIDTH/2)
 * @param y [0, LEVEL_HEIGHT/2)
 * @todo add support for TILE_INDEXING_9PT
 */
void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, u16 x, u16 y)
{
    void (*table[])(const BG_REGULAR *, const level_t *, tile_t *, u16, u16) =
    {
        tile_render_9pt, tile_render_top_bot,
        tile_render_single_8x8, tile_render_single_16x16
    };

    table[tile->indexing](bg, lvl, (tile_t*)tile, x << 1, y << 1);
}


bool tile_no_pass(ent_t *e)
{
    return false;
}


void tile_stone_onhurt(ent_t *e)
{
    
}


void tile_tree_onhurt(ent_t *e)
{
    
}

void tile_stone_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(item->tooltype != TOOL_TYPE_PICKAXE)
        return;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_GRASS));
    item_add_to_inventory(&ITEM_STONE, &ent->player.inventory);
}


void tile_wood_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(item->tooltype == TOOL_TYPE_AXE)
    {
        lvl_set_tile(ent->level, x, y, tile_get(TILE_GRASS));
        item_add_to_inventory(&ITEM_WOOD, &ent->player.inventory);
    }
}


/**
 * Gets a `tile_t` from a type
 */
const tile_t *tile_get(tile_type_t type)
{
    for(u16 i = 0; i < sizeof(tile_tile_data) / sizeof(tile_tile_data[0]); i++)
    {
        if(type == tile_tile_data[i].type)
            return &tile_tile_data[i];
    }

    return NULL;
}