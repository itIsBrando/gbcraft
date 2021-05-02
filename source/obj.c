#include "obj.h"
#include "memory.h"

#include <gba_video.h>
#include <string.h>

#define SPRITE_NUM 128
static obj_t __spr_buffer[SPRITE_NUM];
static bool __spr_free_indexes[SPRITE_NUM];
static uint highest_index = 0;


static uint spr_get_free_index()
{
	for(uint i = 0; i < SPRITE_NUM; i++) {
		if(!__spr_free_indexes[i])
		{
			__spr_free_indexes[i] = true;
			return i;
		}
	}

	return -1;
}


obj_t *spr_alloc(const u16 x, const u16 y, const u16 tile)
{
	uint index = spr_get_free_index();

	if(index > highest_index)
		highest_index = index;

	obj_t *ptr = &__spr_buffer[index];

	spr_move(ptr, x, y);
	spr_set_tile(ptr, tile);
	spr_show(ptr);
	return ptr;
}


// local routine useful to speed up spr_copy_all()
static void _spr_find_highest_index()
{
	for(uint i = 0; i < SPRITE_NUM; i++)
		if(__spr_free_indexes[i])
			highest_index = i;
}


void spr_free(obj_t *obj)
{
	spr_hide(obj);
	for(uint i = 0; i < SPRITE_NUM; i++)
	{
		if(obj == &__spr_buffer[i])
		{
			spr_copy(obj, i);
			__spr_free_indexes[i] = false;
			if(highest_index == i)
				_spr_find_highest_index();
			return;
		}
	}
}


void spr_init()
{
	memset(__spr_free_indexes, 0, sizeof(__spr_free_indexes));
	REG_DISPCNT |= OBJ_ON | OBJ_1D_MAP;
}


void spr_copy(obj_t *obj, const uint index)
{
	oam_mem[index] = *obj;
}


void spr_copy_all()
{
	// @todo only copy created sprites
	memcpy16((u16*)oam_mem, (u16*)__spr_buffer, (highest_index+1) << 2);
}


/**
 * @param obj sprite
 * @param x absolute x position (0-1FF)
 * @param y absolute y position (0-FF)
 */
void spr_move(obj_t *obj, const uint x, const uint y)
{
	obj->attr1 &= 0xFE00;
	obj->attr1 |= x & 0x01FF;

	obj->attr0 &= 0xFF00;
	obj->attr0 |= y & 0xFF;
}


inline uint spr_get_x(const obj_t *obj)
{
	return obj->attr1 & 0x01FF;
}


inline uint spr_get_y(const obj_t *obj)
{
	return obj->attr0 & 0x00FF;
}

inline uint spr_get_tile(const obj_t *obj)
{
	return obj->attr2 & 0x01FF;
}


/**
 * @param mask SPR_FLIP_HORIZONTAL, SPR_FLIP_VERTICAL
 * @see spr_flip_mask
 */
inline void spr_flip(obj_t *obj, const spr_flip_mask mask)
{
	obj->attr1 &= 0xCFFF;
	obj->attr1 |= mask;
}


/**
 * Hides a sprite
 */
inline void spr_hide(obj_t *obj)
{
	obj->attr0 &= 0xFFCF;
	obj->attr0 |= 1 << 0x9;
}


/**
 * Shows a sprite. Default is shown
 */
inline void spr_show(obj_t *obj)
{
	obj->attr0 &= ~(BIT(8)|BIT(9));
}


void spr_set_color_mode(obj_t *obj, const bool is8bpp)
{
	if(!is8bpp)
		obj->attr0 &= 0xDFFF;
	else
		obj->attr0 |= BIT(0xD);
}


void spr_set_priority(obj_t *obj, spr_priority_t priority)
{
	obj->attr2 &= ~SPR_PRIORITY_HIGHEST;
	obj->attr2 |= priority;
}


void spr_set_pal(obj_t *obj, u8 pal)
{
	obj->attr2 &= 0x0FFF;
	obj->attr2 |= (pal & 0xF) << 0xC;
}

/**
 * @param tile bits 0-9 (0-1FF)
 */
inline void spr_set_tile(obj_t *obj, u16 tile)
{
	obj->attr2 &= ~0x01FF;
	obj->attr2 |= tile & 0x01FF;
}


/**
 * Only works for sprites in 4bpp mode
 * @param pal palette number (0-3)
 */
inline void spr_set_palette(obj_t *obj, const u16 pal)
{
	obj->attr2 = (pal & 0x3) << 0xC;
}


void spr_set_size(obj_t *obj, const spr_size_mask size)
{
	const u16 sh = size >> 2, se = size & 0x3;

	obj->attr0 &= 0x3FF;
	obj->attr0 |= sh << 0xE;
	obj->attr1 &= 0x3FF;
	obj->attr1 |= se << 0xE;
}


spr_size_mask spr_get_size(const obj_t *obj)
{
	return ((obj->attr0 >> 0xE) << 2) | (obj->attr1 >> 0xE);
}


inline void spr_set_gfx_mode(obj_t *obj, spr_gfx_mode_t mode)
{
	obj->attr0 &= 0xF3FF;
	obj->attr0 |= (mode & 0x3) << 0xA;
}


#define BLEND_CNT			*(vu16*)(0x04000050)	// Alpha control
#define BLEND_ALPHA		    *(vu16*)(0x04000052)	// Fade level
// #define REG_BLDY			*(vu16*)(0x04000054)	// Blend levels


inline void blnd_set(const blnd_cnt_mask_t mask, const blnd_mode_t mode)
{
	BLEND_CNT = mask | mode;
	blnd_set_weights(28, 4);
}


inline void blnd_set_weights(uint top, uint bot)
{
	BLEND_ALPHA = (top & 0x1F) + ((bot & 0x1F) << 8);
}


inline void blnd_set_fade(uint amt)
{
	*(vu16*)(0x04000054) = amt;
}