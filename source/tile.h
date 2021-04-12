#ifndef TILE_H
#define TILE_H

#include "types.h"

extern const tile_t tile_tile_data[];

tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, uint x, uint y);
const tile_t *tile_get(tile_type_t type);

void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, uint x, uint y);

void tile_tree_onhurt(ent_t *e);
bool tile_no_pass(ent_t *e);
void tile_rock_onhurt(ent_t *e);


#endif