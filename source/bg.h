#ifndef BG_H
#define BG_H

#include "defines.h"

u16 RGB15(uint r, uint g, uint b);

/**
 * @param bg pointer to structure to fill
 * @param mapBlock VRAM block location of the map
 * @param tileBlock VRAM block location of the tileset
 * @param num background number 0-3
 */
void bg_init(BG_REGULAR *bg, const uint8_t mapBlock, const uint8_t tileBlock, const uint8_t num);


/**
 * Fills the rectangle from (x, y) to (w, h) with `tile`
 */
void bg_fill(const BG_REGULAR *bg, const uint x, const uint y, uint w, uint h, uint tile);


/**
 * Writes a tile at (x, y)
 * @param bg background to draw onto
 * @param x unclipped tile x position
 * @param x unclipped tile y position
 * @param tile tile
 */
void bg_write_tile(const BG_REGULAR *bg, uint x, uint y, uint tile);


/**
 * Writes a 16x16 tile at (x, y)
 * @param bg background to draw onto
 * @param x unclipped tile x position
 * @param x unclipped tile y position
 * @param tile tile
 */
void bg_write16(const BG_REGULAR *bg, uint x, uint y, uint tile);

uint bg_get_tile_absolute(const BG_REGULAR *bg, uint tx, uint ty);
uint bg_get_tile(const BG_REGULAR *bg, uint tx, uint ty);


/**
 * Copies data to the background
 * @param bg background struct
 * @param x
 * @param y
 * @param w width of data
 * @param h height of data
 * @param data u16 pointer to tile data
 */
void bg_rect(const BG_REGULAR *bg, const uint x, const uint y, const uint w, const uint h, uint16_t *data);


void bg_move(BG_REGULAR *bg, const uint x, const uint y);
u16 bg_get_scx(const BG_REGULAR *bg);
u16 bg_get_scy(const BG_REGULAR *bg);


/**
 * @param charbank tile bank to load tiles into (0-31?)
 * @param position starting tilenumber in the charbank
 * @param data pointer to tile data
 * @param size size of tile data in bytes
 */
void bg_load_tiles(const uint8_t charbank, const uint position, const unsigned char *data, const uint size);

#endif