#include "render.h"
#include "tile.h"
#include "level.h"

#include "bg.h"


static BG_REGULAR *target_bg;


static void tile_render_single_8x8(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    bg_fill(target_bg, x, y, 2, 2, tile->tiling.center);

    tile_render_nearby(SURROUNDING_DOWN | SURROUNDING_LEFT | SURROUNDING_UP | SURROUNDING_RIGHT, lvl, tile, x, y);
}


static void tile_render_single_16x16(const level_t *lvl, tile_t *tile, u16 x, u16 y)
{
    u8 data[4] = {tile->tiling.topRight,
        tile->tiling.topRight + 1,
        tile->tiling.topRight + 32,
        tile->tiling.topRight + 33};

    bg_rect(target_bg, x, y, 2, 2, data);

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

