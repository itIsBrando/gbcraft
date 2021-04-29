#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include "types.h"

void gen_generate_overworld(level_t *lvl);
void gen_generate_underworld(level_t *lvl);

void gen_generate(level_t *lvl);
void gen_make_stairs_down(level_t *lvl);

#endif