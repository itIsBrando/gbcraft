#include "obj.h"
#include "memory.h"

#include <gba_video.h>
#include <string.h>


static obj_t __spr_buffer[128];
static bool __spr_free_indexes[128];

inline static uint spr_get_free_index()
{
	for(uint i = 0; i < sizeof(__spr_free_indexes) / sizeof(__spr_free_indexes[0]); i++)
		if(!__spr_free_indexes[i])
		{
			__spr_free_indexes[i] = true;
			return i;
		}
	return NULL;
}


obj_t *spr_alloc(const uint x, const uint y, const uint tile)
{
	obj_t *ptr = &__spr_buffer[spr_get_free_index()];

	spr_move(ptr, x, y);
	spr_set_tile(ptr, tile);
	return ptr;
}


void spr_free(obj_t *obj)
{
	for(uint i = 0; i < sizeof(__spr_free_indexes) / sizeof(__spr_free_indexes[0]); i++)
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


/**
 * @param obj sprite
 * @param x absolute x position
 * @param y absolute y position
 */
void spr_move(obj_t *obj, const uint8_t x, const uint8_t y)
{
	obj->attr1 &= 0xFE00;
	obj->attr1 |= x;

	obj->attr0 &= 0xFE00;
	obj->attr0 |= y;
}


inline uint spr_get_x(const obj_t *obj)
{
	return obj->attr1 & 0x01FF;
}


inline uint spr_get_y(const obj_t *obj)
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
 * @param tile bits 0-9
 */
inline void spr_set_tile(obj_t *obj, const uint tile)
{
	obj->attr2 &= ~0x01FF;
	obj->attr2 |= tile & 0x01FF;
}


/**
 * Only works for sprites in 4bpp mode
 * @param pal palette number (0-3)
 */
inline void spr_set_palette(obj_t *obj, const uint pal)
{
	obj->attr2 = (pal & 0x3) << 0xC;
}


void spr_set_size(obj_t *obj, const spr_size_mask size)
{
	const uint sh = size >> 2, se = size & 0x3;

	obj->attr0 &= 0x3FF;
	obj->attr0 |= sh << 0xE;
	obj->attr1 &= 0x3FF;
	obj->attr1 |= se << 0xE;
}


spr_size_mask spr_get_size(const obj_t *obj)
{
	return ((obj->attr0 >> 0xE) << 2) | (obj->attr1 >> 0xE);
}
