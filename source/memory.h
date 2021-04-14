#ifndef MEMORY_H
#define MEMORY_H

void memcpy16(vu16 *dst, const u16 *src, u16 hwsize);

void rnd_seed(int seed);
u16 rnd_random();
s16 rnd_random_bounded(s16 min, s16 max);

#endif