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

typedef enum {
    SPR_PRIORITY_LOWEST = 0,
    SPR_PRIORITY_LOW = BIT(0xA),
    SPR_PRIORITY_HIGH = BIT(0xB),
    SPR_PRIORITY_HIGHEST = BIT(0xA) | BIT(0xB),
} spr_priority_t;


typedef enum {
	BLEND_BOT_OBJ=BIT(0xC),
	BLEND_BOT_BG3=BIT(0xB),
	BLEND_BOT_BG2=BIT(0xA),
	BLEND_BOT_BG1=BIT(0x9),
	BLEND_BOT_BG0=BIT(0x8),

	BLEND_TOP_OBJ=BIT(0x4),
	BLEND_TOP_BG3=BIT(0x3),
	BLEND_TOP_BG2=BIT(0x2),
	BLEND_TOP_BG1=BIT(0x1),
	BLEND_TOP_BG0=BIT(0x0),

	BLEND_MODE_0=0,			/** no blending. */
	BLEND_MODE_1=0b01<<6,	/** weighted blending. @see `REG_BNDALPHA` */
	BLEND_MODE_2=0b10<<6,	/** white @see REG_BLDY` */
	BLEND_MODE_3=0b11<<6,	/** black @see REG_BLDY` */
} blnd_cnt_mask_t;


typedef enum {
	SPR_GFX_NONE,
	SPR_GFX_ALPHA_BLENDING,
	SPR_GFX_WINDOW
} spr_gfx_mode_t;


typedef blnd_cnt_mask_t blnd_mode_t; /** `BLEND_MODE_...` */


/**
 * @param x initial x position
 * @param y initial x position
 * @param tile initial tile
 * @note must be free with `spr_free`
 * @returns pointer to your new sprite. Most uninitalized too
 */
obj_t *spr_alloc(const u16 x, const u16 y, const u16 tile);
void spr_free(obj_t *obj);

/**
 * Allows your program to utilize this sprite library
 */
void spr_init();

void spr_copy(obj_t *, const uint index);

/**
 * Copies all sprites to OAM
 */
void spr_copy_all();

void spr_move(obj_t *, const uint, const uint);


/**
 * Sets paletting mode. Defaults to 4BPP
 * @param is8bpp true to use 256colors, false to use 16colors
 */
void spr_set_color_mode(obj_t *obj, const bool is8bpp);


/**
 * Sets the priority of a sprite.
 * @param priority `spr_priority_t`. SPR_PRIORITY_...
 */
void spr_set_priority(obj_t *obj, spr_priority_t priority);


/**
 * Sets the palette number for a sprite in 4BPP. Default is 0
 * @param pal 0x00-0x0F
 */
void spr_set_pal(obj_t *obj, u8 pal);

void spr_set_tile(obj_t *obj, u16 tile);


/**
 * Sets special graphics mode for a sprite. Default is `SPR_GFX_NONE`
 * @param mode SPR_GFX_...
 */
void spr_set_gfx_mode(obj_t *obj, spr_gfx_mode_t mode);

void spr_flip(obj_t *obj, const spr_flip_mask mask); // only compatible with regular sprites

void spr_hide(obj_t *obj);
void spr_show(obj_t *obj);

/**
 * Sets the size of the sprite (SPR_SIZE_8x8, SPRITE_SIZE_16x8...)
 * @param size spr_size_mask
 */
void spr_set_size(obj_t *obj, const spr_size_mask size);
spr_size_mask spr_get_size(const obj_t *obj);
uint spr_get_x(const obj_t *obj);
uint spr_get_y(const obj_t *obj);
uint spr_get_tile(const obj_t *obj);



/**
 * @param mask layers to blend (BLEND_TOP_... | BLEND_BOT...)
 * @param mode blending mode to use (BLEND_MODE_...)
 */
void blnd_set(const blnd_cnt_mask_t mask, const blnd_mode_t mode);


/**
 * Only for mode 1
 * @param top top layer weight [0-1F] [0-31]
 * @param bot bottom layer weight [0-1F] [0-31]
 */
void blnd_set_weights(uint top, uint bot);

/**
 * Sets darkening/lightening affect for modes 2 & 3
 * @param amt [0-1f] [0-31]
 */
void blnd_set_fade(uint amt);

#endif