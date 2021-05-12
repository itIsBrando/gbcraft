#ifndef RENDER_H
#define RENDER_H

#include "types.h"


void tile_render_use_recursion(bool b);
void tile_render_nearby(const level_t *lvl, const tile_t *tile, u16 x, u16 y);

void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, u16 x, u16 y);


#endif