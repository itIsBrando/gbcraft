#ifndef BG_H
#define BG_H

#include "types.h"


#define TILE_FLIP_HORIZONTAL(t) ((t) | BIT(0xA)) /** flips an 8-bit tile @warning only for regular bgs!! */
#define TILE_FLIP_VERTICAL(t) ((t) | BIT(0xB)) /** flips an 8-bit tile @warning only for regular bgs!! */

u16 RGB15(u16 r, u16 g, u16 b);

int dir_get_x(direction_t direction); // @todo move elsewhere
int dir_get_y(direction_t direction);
typedef BG_REGULAR AFFINE_BG;

/**
 * @param bg pointer to structure to fill
 * @param mapBlock VRAM block location of the map [0-32)
 * @param tileBlock VRAM block location of the tileset [0,4)
 * @param num background number 0-3
 */
void bg_init(BG_REGULAR *bg, const uint8_t mapBlock, const uint8_t tileBlock, const uint8_t num);


/**
 * @param bg pointer to structure to fill
 * @param mapBlock VRAM block location of the map [0, 32)
 * @param tileBlock VRAM block location of the tileset [0,4)
 * @param num must be 2 or 3
 * @note Sets the size to the smallest possible value
 */
void bg_affine_init(AFFINE_BG *bg, const uint8_t mapBlock, const uint8_t tileBlock, const uint8_t num);


void bg_clamp_coordinates(const BG_REGULAR *bg, uint *tx, uint *ty);
void bg_show(const BG_REGULAR *bg);
void bg_hide(const BG_REGULAR *bg);

/**
 * Fills the rectangle from (x, y) to (w, h) with `tile`
 */
void bg_fill(const BG_REGULAR *bg, uint x, uint y, uint w, uint h, u16 tile);


/**
 * Writes a tile at (x, y)
 * @param bg background to draw onto
 * @param x unclipped tile x position
 * @param x unclipped tile y position
 * @param tile tile
 */
void bg_write_tile(const BG_REGULAR *bg, uint x, uint y, u16 tile);

void bg_move_by(BG_REGULAR *bg, const direction_t direction);

uint bg_get_tile_absolute(const BG_REGULAR *bg, uint tx, uint ty);
uint bg_get_tile(const BG_REGULAR *bg, uint tx, uint ty);

/**
 * Sets the size of a background. Default is 32x32
 * @param size BG_SIZE_REG_... or BG_SIZE_AFF_...
 * @see bg_map_size_t
 */
void bg_set_size(BG_REGULAR *bg, bg_map_size_t size);


/**
 * Sets the priority of the background. Higher numbers drawn first
 * @param p BG_PRIORITY_...
 */
void bg_set_priority(BG_REGULAR *bg, bg_priority_t p);

/**
 * Copies data to the background
 * @param bg background struct
 * @param x
 * @param y
 * @param w width of data
 * @param h height of data
 * @param data u16 pointer to tile data. If affine, then it must be u8 pointer
 */
void bg_rect(const BG_REGULAR *bg, uint x, uint y, const uint w, const uint h, void *data);


void bg_move(BG_REGULAR *bg, const uint x, const uint y);
int bg_get_scx(const BG_REGULAR *bg);
int bg_get_scy(const BG_REGULAR *bg);


/**
 * @param charbank tile bank to load tiles into (0-31?)
 * @param position starting tilenumber in the charbank
 * @param data pointer to tile data
 * @param size size of tile data in bytes
 * @param is8bpp determine whether tiles are 8bpp or 4bpp
 */
void bg_load_tiles(
    const uint8_t charbank, const uint position,
    const unsigned char *data, const uint size, const bool is8bpp
);

#endif