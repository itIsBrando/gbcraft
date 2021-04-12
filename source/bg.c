#include <gba_video.h>

#include "bg.h"
#include "memory.h"
#include "text.h"


inline u16 RGB15(uint r, uint g, uint b)
{
	return r | (g << 5) | (b << 10);
}


int _dx[] = {-1, 1, 0, 0};
int _dy[] = {0, 0, -1, 1};

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


uint bg_get_ssb(const uint tx, const uint ty)
{
    return (ty >> 5) * 2;
}


void bg_move_by(BG_REGULAR *bg, const direction_t direction)
{
    bg->bgx += dir_get_x(direction);
    bg->bgy += dir_get_y(direction);
    *bg->BG_Y = bg->bgy;
    *bg->BG_X = bg->bgx;
}

/**
 * Scrolls the background to (`x`, `y`)
 */
void bg_move(BG_REGULAR *bg, const uint x, const uint y)
{
    *bg->BG_Y = y;
    *bg->BG_X = x;
    bg->bgx = x;
    bg->bgy = y;
}


/**
 * Gets the background's scroll offset in the x direction
 */
inline u16 bg_get_scx(const BG_REGULAR *bg)
{
    return bg->bgx;
}


/**
 * Gets the background's scroll offset in the y direction
 */
inline u16 bg_get_scy(const BG_REGULAR *bg)
{
    return bg->bgy;
}


/**
 * Adjust coordinates for block overflow
 */
inline void bg_adjust_coordinates(uint *tx, uint *ty)
{
    *tx &= 0x1f;
    *ty &= 0x1f;
}


/**
 * Gets the tile using absolute tile coordinates
 * @see bg_get_tile()
 */
uint bg_get_tile_absolute(const BG_REGULAR *bg, uint tx, uint ty)
{
    const uint tile = map_mem[bg->map_base + bg_get_ssb(tx, ty)][tx + (ty << 5)];
    text_uint(*bg->BG_X, 0, 4);
    return tile;
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


void bg_write_tile(const BG_REGULAR *bg, uint x, uint y, uint tile)
{
	map_mem[bg->map_base + bg_get_ssb(x, y)][x + (y << 5)] = tile;
}


void bg_write16(const BG_REGULAR *bg, uint x, uint y, uint tile)
{
    const uint TILES_PER_ROW = 32;

    bg_write_tile(bg, x, y, tile);
    bg_write_tile(bg, x+1, y, tile+1);
    bg_write_tile(bg, x, y+1, tile+TILES_PER_ROW);
    bg_write_tile(bg, x+1, y+1, tile+TILES_PER_ROW+1);
}


void bg_rect(const BG_REGULAR *bg, const uint x, const uint y, const uint w, const uint h, uint16_t *data)
{
    vu16 *ptr = map_mem[bg->map_base + bg_get_ssb(x, y)] + (y << 5) + x;

    for(uint j = y; j < y + h; j++)
    {
        for (uint i = x; i < x + w; i++)
        {
            *ptr++ = *data;
            data++;
        }

        ptr += 32 - w;
    }
    
}


void bg_fill(const BG_REGULAR *bg, const uint x, const uint y, uint w, uint h, uint tile)
{
    vu16 *ptr = map_mem[bg->map_base + bg_get_ssb(x, y)] + (y << 5) + x;
    for(uint j = y; j < y + h; j++)
    {
        for (uint i = x; i < x + w; i++)
        {
            *ptr++ = tile;
        }

        ptr += 32 - w;
    }
    
}


void bg_load_tiles(const uint8_t charbank, const uint position, const unsigned char *data, const uint size)
{
	memcpy16((vu16*)tile_mem[charbank][position],
     (u16*)data, size >> 1);
}


void bg_init(BG_REGULAR *bg, const uint8_t mapBlock, const uint8_t tileBlock, const uint8_t num)
{
    bg->width = 32;
    bg->map_base = mapBlock;
    bg->tile_base = tileBlock;
    bg->BG_CNT = regs[num][0]; // replace with 0x0400:0008 + 2*num
    bg->BG_X = regs[num][2];   // replace with 0x0400:0010 + 4*num
    bg->BG_Y = regs[num][1];   // replace with 0x0400:0012 + 4*num

    *bg->BG_CNT = BG_16_COLOR | BG_SIZE_0
     | BG_MAP_BASE(mapBlock) | BG_TILE_BASE(tileBlock);
    
    bg_move(bg, 0, 0);

	REG_DISPCNT |= BG0_ON << num;
}