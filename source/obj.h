#ifndef OBJ_H
#define OBJ_H

#include <gba_sprites.h>

#include "defines.h"

typedef struct {
	u16 attr0;
	u16 attr1;
	u16 attr2;
	u16 dummy;
} __attribute__((packed, aligned(4))) obj_t;

typedef enum {
	SPR_FLIP_NONE = 0,
    SPR_FLIP_HORIZONTAL = BIT(0xC),
    SPR_FLIP_VERTICAL = BIT(0xD)
} spr_flip_mask;


typedef enum {
	SPR_SIZE_8x16 =  0b1000,
	SPR_SIZE_8x32 =  0b1001,
	SPR_SIZE_16x32 = 0b1010,
	SPR_SIZE_32x64 = 0b1011,

	SPR_SIZE_16x8 =  0b0100,
	SPR_SIZE_32x8 =  0b0101,
	SPR_SIZE_32x16 = 0b0110,
	SPR_SIZE_64x32 = 0b0111,

	SPR_SIZE_8x8 = 	 0b0000,
	SPR_SIZE_16x16 = 0b0001,
	SPR_SIZE_32x32 = 0b0010,
	SPR_SIZE_64x64 = 0b0011,
	
} spr_size_mask;

/**
 * @param x initial x position
 * @param y initial x position
 * @param tile initial tile
 * @note must be free with `spr_free`
 * @returns pointer to your new sprite. Most uninitalized too
 */
obj_t *spr_alloc(const uint x, const uint y, const uint tile);
void spr_free(obj_t *obj);

/**
 * Allows your program to utilize this sprite library
 */
void spr_init();

void spr_copy(obj_t *, const uint8_t index);
void spr_move(obj_t *, const uint8_t, const uint8_t);

void spr_set_tile(obj_t *obj, const uint tile);
void spr_flip(obj_t *obj, const spr_flip_mask mask); // only compatible with regular sprites

/**
 * Sets the size of the sprite (SPR_SIZE_8x8, SPRITE_SIZE_16x8...)
 * @param size spr_size_mask
 */
void spr_set_size(obj_t *obj, const spr_size_mask size);
spr_size_mask spr_get_size(const obj_t *obj);

#endif