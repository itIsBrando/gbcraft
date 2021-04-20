#include "obj.h"
#include "memory.h"

#include <gba_video.h>
#include <string.h>


static obj_t __spr_buffer[128];
static bool __spr_free_indexes[128];

inline static u16 spr_get_free_index()
{
	for(u16 i = 0; i < sizeof(__spr_free_indexes) / sizeof(__spr_free_indexes[0]); i++)
		if(!__spr_free_indexes[i])
		{
			__spr_free_indexes[i] = true;
			return i;
		}
	return -1;
}


obj_t *spr_alloc(const u16 x, const u16 y, const u16 tile)
{
	obj_t *ptr = &__spr_buffer[spr_get_free_index()];

	spr_move(ptr, x, y);
	spr_set_tile(ptr, tile);
	spr_show(ptr);
	return ptr;
}


void spr_free(obj_t *obj)
{
	spr_hide(obj);
	for(u16 i = 0; i < sizeof(__spr_free_indexes) / sizeof(__spr_free_indexes[0]); i++)
	{
		if(obj == &__spr_buffer[i])
			__spr_free_indexes[i] = false;
	}
}

void spr_init()
{
	memset(__spr_free_indexes, 0, sizeof(__spr_free_indexes));
	REG_DISPCNT |= OBJ_ON | OBJ_1D_MAP;
}

void spr_copy(obj_t *obj, const uint8_t index)
{
	memcpy16((vu16*)(&oam_mem[index]), (u16*)obj, 4);
}

void spr_copy_all()
{
	// @todo only copy created sprites
	memcpy16((vu16*)oam_mem, (u16*)__spr_buffer, 64 << 2);
}


/**
 * @param obj sprite
 * @param x absolute x position
 * @param y absolute y position
 */
void spr_move(obj_t *obj, const u16 x, const u8 y)
{
	obj->attr1 &= 0xFE00;
	obj->attr1 |= x & 0x01FF;

	obj->attr0 &= 0xFF00;
	obj->attr0 |= y;
}


inline u16 spr_get_x(const obj_t *obj)
{
	return obj->attr1 & 0x01FF;
}


inline u16 spr_get_y(const obj_t *obj)
{
	return obj->attr0 & 0x00FF;
}


/**
 * @param mask SPR_FLIP_HORIZONTAL, SPR_FLIP_VERTICAL
 * @see spr_flip_mask
 */
inline void spr_flip(obj_t *obj, const spr_flip_mask mask)
{
	obj->attr1 &= 0xC0FF;
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
 * Shows a sprite
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
 * @param tile bits 0-9
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
