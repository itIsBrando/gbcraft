#include "level.h"
#include "tile.h"
#include "item.h"
#include "player.h"
#include "entity.h"
#include "terraingen.h"
#include "menu.h"
#include "lighting.h"

#include "memory.h"
#include "text.h"
#include "bg.h"

static BG_REGULAR *target_bg;

const tile_event_t tile_events[] = {
    { // grass floor
        .onhurt=NULL,
        .ontouch=NULL,
        .maypass=tile_grass_maypass,
        .interact=tile_grass_interact,
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
    { // ore

    },
    { // stairs down
        .onhurt=NULL,
        .ontouch=tile_stair_down_ontouch,
        .maypass=NULL,
        .interact=NULL,
    },
    { // stairs up
        .ontouch=tile_stair_up_ontouch,
    },
    { // door closed
        .interact=tile_door_closed_interact,
        .maypass=tile_no_pass,
    },
    { // door open
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


void tile_grass_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(!item)
        return;

    if(item->tooltype != TOOL_TYPE_HOE || lvl_get_tile_type(ent->level, x, y) != TILE_GRASS)
        return;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_MUD));
    if((rnd_random() & 0x7) == 0)
        ent_item_new(ent->level, lvl_to_pixel_x(x), lvl_to_pixel_y(y), (item_t*)&ITEM_SEED, 1);

}


void tile_tree_hurt(level_t *lvl, u8 dmg, u16 x, u16 y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 20)
    {
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
        x = lvl_to_pixel_x(x);
        y =  lvl_to_pixel_y(y);
        ent_item_new(lvl, x, y, (item_t*)&ITEM_WOOD, 2);
        if((rnd_random() & 0x3) == 0)
            ent_item_new(lvl, x, y, (item_t*)&ITEM_SAPLING, 2);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}


void tile_tree_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    if(!item || item->tooltype != TOOL_TYPE_AXE || ent->type != ENT_TYPE_PLAYER)
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
        uint px = lvl_to_pixel_x(x), py = lvl_to_pixel_y(y);

        lvl_set_tile(lvl, x, y, tile_get(lvl->layer > 0 ? TILE_MUD : TILE_GRASS));

        ent_item_new(lvl, px, py, (item_t*)&ITEM_STONE, 2);
        if((rnd_random() & 0xF) == 0)
            ent_item_new(lvl, px, py, &ITEM_COAL, (rnd_random() & 0x1) + 1);
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


bool tile_stair_up_ontouch(ent_t *e, uint x, uint y)
{
    if(e->type != ENT_TYPE_PLAYER)
        return false;

    if(e->player.on_stairs > 0)
        return false;

    mnu_load_level();

    level_t *curLevel = e->level;
    level_t *newLevel = world[curLevel->layer-1];
    plr_move_to(e, x, y);
    
    if(!curLevel->layer) // @todo safeguard against this. The window will block viewport
        return false;
    
    e = ent_change_level(e, newLevel);

    lvl_change_level(newLevel);

    if(!newLevel->layer) {
        lt_hide();
    }

	mnu_draw_hotbar(e);

    e->player.on_stairs = 65;

    return true;
}


/** called when entity collides with this. This is a stairway down
 * @param e player entity
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns true if the player changed levels
 */
bool tile_stair_down_ontouch(ent_t *e, uint x, uint y)
{
    if(e->type != ENT_TYPE_PLAYER)
        return false;
    
    if(e->player.on_stairs > 0)
        return false;

    mnu_load_level();

    // @todo enter new level
    level_t *curLevel = e->level;
    uint curLayer = curLevel->layer;
    level_t *newLevel;

    plr_move_to(e, x, y);

    if(world[curLayer+1])
        newLevel = world[curLayer+1];
    else {
        // generate new level if necessary
        newLevel = lvl_new(curLayer + 1, curLevel);
        gen_generate(newLevel);

        for(uint i = 0; i < 8; i++)
            lvl_set_tile(newLevel, x + dir_get_x(i), y + dir_get_y(i), tile_get(TILE_MUD));

        lvl_set_tile(newLevel, x, y, tile_get(TILE_STAIR_UP));
    }
   
    e = ent_change_level(e, newLevel);

    lvl_change_level(newLevel);
    lt_show(newLevel);

    e->player.on_stairs = 65;

	mnu_draw_hotbar(e);
    return true;
}


void tile_door_closed_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    // @todo add door_hurt
    if(item && item->tooltype == TOOL_TYPE_AXE)
        return;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_DOOR_OPEN));
}


void tile_door_open_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    // @todo add door_hurt (can be the same for opened and closed)
    if(item && item->tooltype == TOOL_TYPE_AXE)
        return;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_DOOR_CLOSED));
}


void tile_wheat_interact(ent_t *ent, item_t *item, u16 x, u16 y)
{
    level_t *lvl = ent->level;
    uint px = lvl_to_pixel_x(x), py = lvl_to_pixel_y(y);

    lvl_set_tile(lvl, x, y, tile_get(TILE_MUD));
    ent_item_new(lvl, px, py, &ITEM_WHEAT, (rnd_random() & 0x1) + 1);
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