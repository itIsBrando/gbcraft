#ifndef MEMORY_H
#define MEMORY_H

#include "defines.h"

void rnd_seed(int seed);
int rnd_random();
int rnd_random_bounded(int min, int max);

#endif