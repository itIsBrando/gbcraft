#include "memory.h"


void memcpy16(vu16 *dst, const u16 *src, u16 hwsize)
{
	for(u16 i = 0; i < hwsize; i++)
	{
		*dst = *src;
		dst++, src++;
	}
}


void memset16(vu16 *dst, const u16 c, u16 hwsize)
{
	for(u16 i = 0; i < hwsize; i++)
		*dst++ = c;
}

static int _rnd_seed;

void rnd_seed(int seed)
{
	_rnd_seed = seed;
}

/**
 * @note code borrowed from Tonc's documentation
 * @returns random number between [0, 0x8000)
 */
u16 rnd_random()
{
	_rnd_seed *= 1664525;
	_rnd_seed += 1013904223;
	return (_rnd_seed >> 16) & 0x7FFF;
}


/**
 * Generates a random number
 * @note from Tonc
 * @warning (max - min) < 0x8000
 */
inline s16 rnd_random_bounded(s16 min, s16 max)
{
	return (rnd_random()*(max-min)>>15)+min;
}