#include "random.h"


static int _rnd_seed;

/**
 * Sets the RNG seed
 */
void rnd_seed(int seed)
{
	_rnd_seed = seed;
}

/**
 * @note code borrowed from Tonc's documentation
 * @returns random number between [0, 0x8000)
 */
int rnd_random()
{
	_rnd_seed *= 1664525;
	_rnd_seed += 1013904223;
	return (_rnd_seed >> 16) & 0x7FFF;
}


/**
 * Generates a random number
 * @param min inclusive
 * @param max exclusive
 * @note from Tonc
 * @warning (max - min) < 0x8000
 */
inline int rnd_random_bounded(int min, int max)
{
	return (rnd_random()*(max-min)>>15)+min;
}