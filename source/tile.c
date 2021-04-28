#include "level.h"
#include "tile.h"
#include "item.h"
#include "player.h"
#include "entity.h"

#include "memory.h"
#include "text.h"
#include "bg.h"

static BG_REGULAR *target_bg;

const tile_event_t tile_events[] = {
    { // grass floor
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_grass_maypass,
    },
    { // water tile
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_water_maypass,
    },
    { // tree tile
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_tree_interact
    },
    { // wood plank
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_wood_interact
    },
    { // rock
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_no_pass,
        .interact=tile_stone_interact,
    },
    { // stairs
        .onhurt=NULL,
        .ontouch=tile_stair_ontouch,
        .maypass=NULL,
        .interact=NULL,
    },
};


// @todo add this
/**
 * @param _type `tile_type_t`
 * @param _tile tile index
 * @param _indexing_mode `tile_indexing_mode_t`
 * @param _event_num index in `tile_events`
 * @param _connect_to tile_type_t to visually connect to
 */
#define CREATE_TILE(_type, _tile, _indexing_mode, _event_num, _connect_to, ...) {\
 .type=_type,\
 .tiling={_tile},\
 .indexing=_indexing_mode,\
 .event=&tile_events[_event_num],\
 .connect_to=_connect_to,\
 __VA_ARGS__\
}


const tile_t tile_tile_data[] = 
{
    CREATE_TILE(TILE_GRASS, 45, TILE_INDEXING_9PT, 0, TILE_TREE),
    CREATE_TILE(TILE_WATER, 48, TILE_INDEXING_9PT, 1, TILE_NONE),
    CREATE_TILE(TILE_TREE, 7, TILE_INDEXING_SINGLE_16x16, 2, TILE_NONE),
    CREATE_TILE(TILE_WOOD, 3, TILE_INDEXING_TOP_BOT, 3, TILE_NONE),
    CREATE_TILE(TILE_STONE, 42, TILE_INDEXING_9PT, 4, TILE_NONE),
    CREATE_TILE(TILE_IRON, 65, TILE_INDEXING_SINGLE_16x16, 5, TILE_NONE),
    CREATE_TILE(TILE_GOLD, 65, TILE_INDEXING_SINGLE_16x16, 5, TILE_NONE),
    CREATE_TILE(TILE_STAIRS, 67, TILE_INDEXING_SINGLE_16x16, 6, TILE_NONE),
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
    int dx[] = {-1, 1, 0, 0, -1, 1, -1, 1};
    int dy[] = {0, 0, -1, 1, -1, -1, 1, 1};

    tile_surround_mask mask = 0;

    for(uint i = 0; i < 8; i++)
    {
        mask <<= 1;
        if(type == lvl_get_tile_type(lvl, x + dx[i], y + dy[i]))
            mask |= 1;
    }

    return mask;
}


static void tile_render_single_8x8(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    bg_fill(target_bg, x, y, 2, 2, tile->tiling.center);
}


static void tile_render_single_16x16(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    u8 data[4] = {tile->tiling.topRight,
        tile->tiling.topRight + 1,
        tile->tiling.topRight + 32,
        tile->tiling.topRight + 33};

    bg_rect(target_bg, x, y, 2, 2, data);
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
    tile_surround_mask mask = tile_get_surrounding((level_t *)lvl, tile->type, x >> 1, y >> 1)
     | tile_get_surrounding((level_t *)lvl, tile->connect_to, x >> 1, y >> 1);
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
    
    bg_rect(target_bg, x, y, 2, 2, corners);

    return mask;
}


void tile_render_nearby(tile_surround_mask mask, const level_t *lvl, const tile_t *tile, u16 x, u16 y)
{
    tile_surround_mask m = SURROUNDING_LEFT;

    if(!_use_recursion)
        return;

    for(uint i = 0; i < 4; i++)
    {
        s16 xx = x + (dir_get_x(i) << 1),
            yy = y + (dir_get_y(i) << 1);
        if(xx < 0 || yy < 0 || xx >= LEVEL_WIDTH || yy >= LEVEL_HEIGHT)
            continue;
        
        // if(mask & m)
        //     _render_9pt(lvl, tile, xx, yy);
        // else
        _render_9pt(lvl, (tile_t*)lvl_get_tile((level_t*)lvl, xx >> 1, yy >> 1), xx, yy);
        
        m >>= 1;
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
        e->player.is_swimming = true;
        return true;
    }
    else
        return false;
}


bool tile_grass_maypass(ent_t *e)
{
    if(e->type == ENT_TYPE_PLAYER) {
        e->player.is_swimming = false;
    }
    
    return true;
}


bool tile_no_pass(ent_t *e)
{
    return false;
}


void tile_stone_onhurt(ent_t *e)
{
    
}


void tile_tree_hurt(level_t *lvl, u8 dmg, u16 x, u16 y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 20)
    {
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
        ent_item_new(lvl, lvl_to_pixel_x(lvl, x), lvl_to_pixel_y(lvl, y), (item_t*)&ITEM_WOOD, 2);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }


}


void tile_tree_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(!(!item || item->tooltype == TOOL_TYPE_AXE))
        return;

    if(ent->type != ENT_TYPE_PLAYER)
        return;

    if(!plr_pay_stamina(ent, 3 + (!item)))
        return;

    u8 bonus = item ? item->level : -1;

    tile_tree_hurt(ent->level, 5 + bonus, x, y);
}


void tile_stone_hurt(level_t *lvl, u8 dmg, u16 x, u16 y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 20)
    {
        uint px = lvl_to_pixel_x(lvl, x), py = lvl_to_pixel_y(lvl, y);

        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
        ent_item_new(lvl, px, py, (item_t*)&ITEM_STONE, 2);
        if((rnd_random() & 0xF) == 0)
            ent_item_new(lvl, x, y, &ITEM_COAL, (rnd_random() & 0x1) + 1);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}


void tile_stone_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(item && item->tooltype == TOOL_TYPE_PICKAXE && plr_pay_stamina(ent, 4))
        tile_stone_hurt(ent->level, 6 + (item->level << 2), x, y);
}


void tile_wood_hurt(level_t *lvl, u8 dmg, u16 x, u16 y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 20)
    {
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
        item_add_to_inventory(&ITEM_WOOD, &lvl_get_player(lvl)->player.inventory);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}

void tile_wood_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(item && item->tooltype == TOOL_TYPE_AXE && plr_pay_stamina(ent, 4 - item->level))
    {
       tile_wood_hurt(ent->level, 5 + item->level, x, y);
    }
}

/** called when entity collides with this
 * @param x relative pixel x
 * @param y relative pixel y
 */
void tile_stair_ontouch(ent_t *e, uint x, uint y)
{
    if(e->type != ENT_TYPE_PLAYER)
        return;

    // @todo enter new level
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