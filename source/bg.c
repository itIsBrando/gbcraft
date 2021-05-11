#include <gba_video.h>

#include "bg.h"
#include "lib/mem.h"
#include "text.h"


inline u16 RGB15(u16 r, u16 g, u16 b)
{
	return r | (g << 5) | (b << 10);
}


int _dx[] = {-1, 1, 0, 0, -1, 1, -1, 1, 0};
int _dy[] = {0, 0, -1, 1, -1, -1, 1, 1, 0};

inline int dir_get_x(direction_t direction)
{
    return _dx[direction];
}

inline int dir_get_y(direction_t direction)
{
    return _dy[direction];
}



static vu16 *regs[][3] = {
    {&REG_BG0CNT, &REG_BG0VOFS, &REG_BG0HOFS},
    {&REG_BG1CNT, &REG_BG1VOFS, &REG_BG1HOFS},
    {&REG_BG2CNT, &REG_BG2VOFS, &REG_BG2HOFS},
    {&REG_BG3CNT, &REG_BG3VOFS, &REG_BG3HOFS},
};


inline uint bg_get_quadrant(const uint tx, const uint ty)
{
    return 0;//(ty >> 5) * 2;
}

void bg_move_by(BG_REGULAR *bg, const direction_t direction)
{
    bg_move(bg,
        bg->bgx + dir_get_x(direction),
        bg->bgy + dir_get_y(direction)
    );
}

#define toFixed(n) ((n) << 8)

/**
 * Scrolls the background to (`x`, `y`)
 */
void bg_move(BG_REGULAR *bg, const uint x, const uint y)
{
    if(bg->is_affine)
    {
        *bg->x.aff = toFixed(x);
        *bg->y.aff = toFixed(y);
    } else {
        *bg->x.reg = x;
        *bg->y.reg = y;
    }

    bg->bgy = y;
    bg->bgx = x;
}


/**
 * Gets the background's scroll offset in the x direction
 */
inline int bg_get_scx(const BG_REGULAR *bg)
{
    return bg->bgx;
}


/**
 * Gets the background's scroll offset in the y direction
 */
inline int bg_get_scy(const BG_REGULAR *bg)
{
    return bg->bgy;
}


/**
 * Adjust coordinates for overflow on affine backgrounds
 */
inline void bg_clamp_coordinates(const BG_REGULAR *bg, uint *tx, uint *ty)
{
    *tx &= (1 << bg->width_bits)-1;
    *ty &= (1 << bg->width_bits)-1;
}


/**
 * Gets the tile using absolute tile coordinates
 * @see bg_get_tile()
 * @todo add support for large BG_REGULAR maps
 */
uint bg_get_tile_absolute(const BG_REGULAR *bg, uint tx, uint ty)
{
    return map_mem[bg->map_base + bg_get_quadrant(tx, ty)][tx + (ty << 5)];
}


/**
 * Gets the tile using coordinates relative to the screen's viewport
 * @param tx coordinate = tx + bgXoffset
 * @param ty coordinate = ty + bgYoffset
 */
uint bg_get_tile(const BG_REGULAR *bg, uint tx, uint ty)
{
    return bg_get_tile_absolute(bg, tx + (bg_get_scx(bg) >> 3), ty + (bg_get_scy(bg) >> 3));
}


void bg_write_tile(const BG_REGULAR *bg, uint x, uint y, u16 tile)
{
    if(bg->is_affine)
    {
        // bg_clamp_coordinates(bg, &x, &y);
        vu16 *ptr = map_mem[bg->map_base] + (x >> 1) + ((y >> 1) << bg->width_bits);
        if(y & 1) ptr += 1 << bg->width_bits >> 1;
        if(x & 1)
            tile = (tile << 8) | (*ptr & 0x00FF);
        else
            tile = (tile & 0x00FF) | (*ptr & 0xFF00);
        *ptr = tile;
    } else {
	    map_mem[bg->map_base + bg_get_quadrant(x, y)][x + (y << 5)] = tile;
    }
}


void bg_rect(const BG_REGULAR *bg, uint x, uint y, const uint w, const uint h, void *data)
{
    vu16 *ptr;
    uint i, j;
    // @todo AFFINE BG ODD SIZES DO NOT WORK
    if(bg->is_affine)
    {
        x >>= 1;
        y >>= 1;
        ptr = map_mem[bg->map_base] + (y << bg->width_bits) + x;
        for(j = 0; j < h; j++)
        {
            for (i = 0; i < w >> 1; i++)
            {
                *ptr++ = *(u16*)data;
                data += 2;
            }
            ptr += ((1 << bg->width_bits) - w) >> 1;
        }
    } else {
        ptr = map_mem[bg->map_base + bg_get_quadrant(x, y)] + (y << 5) + x;

        for(j = y; j < y + h; j++)
        {
            for (i = x; i < x + w; i++)
            {
                *ptr++ = *(u16*)data;
                data += 2;
            }

            ptr += 32 - w;
        }
    }
    
}


void bg_fill(const BG_REGULAR *bg, uint x, uint y, uint w, uint h, u16 tile)
{
    vu16 *ptr;
    uint i, j;

    if(bg->is_affine)
    {
        tile &= 0x00FF;
        tile |= tile << 8; // convert tile to 16bit
        x >>= 1;
        y >>= 1;
        ptr = map_mem[bg->map_base] + (u32)(y << bg->width_bits) + x;
        for(j = 0; j < h; j++)
        {
            for(i = 0; i < w >> 1; i++)
            {
                *ptr++ = tile;
            }
            ptr += ((1 << bg->width_bits) - w) >> 1;
        }
    } else {
        ptr = map_mem[bg->map_base + bg_get_quadrant(x, y)] + (y << 5) + x;
        for(j = y; j < y + h; j++)
        {
            for(i = x; i < x + w; i++)
            {
                *ptr++ = tile;
            }

            ptr += 32 - w;
        }
    }
    
}


void bg_load_tiles(const uint8_t charbank, const uint position, const unsigned char *data, const uint size, const bool is8bpp)
{
	memcpy16((u16*)tile_mem[charbank][position << is8bpp],
     (u16*)data, size);
}


void bg_set_size(BG_REGULAR *bg, bg_map_size_t size)
{
    *bg->BG_CNT = (*bg->BG_CNT & 0x3FFF) | (size << 0xE);
    const u8 reg_widths[] = {5, 6, 5, 6};
    const u8 aff_widths[] = {4, 5, 6, 7};
    bg->width_bits = bg->is_affine ? aff_widths[size] : reg_widths[size];
}


void bg_set_priority(BG_REGULAR *bg, bg_priority_t p)
{
    *bg->BG_CNT &= 0xFFFC;
    *bg->BG_CNT |= p & 0x3;
}


void bg_affine_init(AFFINE_BG *bg, const uint8_t mapBlock, const uint8_t tileBlock, const uint8_t num)
{
    bg->is_affine = true;

    bg->map_base = mapBlock;
    bg->tile_base = tileBlock;
    bg->map_number = num;

    bg->BG_CNT = (vu16*)(0x04000008 + (num << 1));
    // initialize BG scroll registers
    const uint off = (0x10 * (num - 2));
    bg->x.aff = (vs32*)(0x04000028 + off);
    bg->y.aff = (vs32*)(0x0400002C + off);

    *bg->BG_CNT = BG_256_COLOR | BG_SIZE_0 /*| BG_WRAP*/
     | BG_MAP_BASE(mapBlock) | BG_TILE_BASE(tileBlock);

    bg_move(bg, 0, 0);
    bg_show(bg);

    bg_set_size(bg, BG_SIZE_AFF_16x16);
}


void bg_init(BG_REGULAR *bg, const uint8_t mapBlock, const uint8_t tileBlock, const uint8_t num)
{
    bg->map_base = mapBlock;
    bg->tile_base = tileBlock;
    bg->BG_CNT = regs[num][0]; // replace with 0x0400:0008 + 2*num
    bg->x.reg = regs[num][2];   // replace with 0x0400:0010 + 4*num
    bg->y.reg = regs[num][1];   // replace with 0x0400:0012 + 4*num
    bg->map_number = num;
    bg->is_affine = false;

    *bg->BG_CNT = BG_16_COLOR
        | BG_MAP_BASE(mapBlock) | BG_TILE_BASE(tileBlock);
    
    bg_move(bg, 0, 0);
    bg_show(bg);

    bg_set_size(bg, BG_SIZE_REG_32x32);
}


/**
 * Visually shows a background
 */
inline void bg_show(const BG_REGULAR *bg)
{
	REG_DISPCNT |= BG0_ON << bg->map_number;
}


/**
 * Visually hides a background
 */
inline void bg_hide(const BG_REGULAR *bg)
{
    REG_DISPCNT &= ~(BG0_ON << bg->map_number);
}