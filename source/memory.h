#ifndef MEMORY_H
#define MEMORY_H

#include "defines.h"

void memcpy16(vu16 *dst, const u16 *src, u16 hwsize);
void memset16(vu16 *dst, const u16 c, u16 hwsize);

void rnd_seed(int seed);
int rnd_random();
int rnd_random_bounded(int min, int max);

#endif