#ifndef TILE_H
#define TILE_H

#include "types.h"


typedef enum {
    SURROUNDING_LEFT = 1 << 7,
    SURROUNDING_RIGHT = 1 << 6,
    SURROUNDING_UP = 1 << 5,
    SURROUNDING_DOWN = 1 << 4,
    SURROUNDING_LEFT_UP = 1 << 3, 
    SURROUNDING_RIGHT_UP = 1 << 2,
    SURROUNDING_LEFT_DOWN = 1 << 1,
    SURROUNDING_RIGHT_DOWN = 1 << 0,
} tile_surround_mask;


extern const tile_t tile_tile_data[];

tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, uint x, uint y);
const tile_t *tile_get(tile_type_t type);

void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, uint x, uint y);

void tile_tree_onhurt(ent_t *e);
bool tile_no_pass(ent_t *e);
void tile_rock_onhurt(ent_t *e);


#endif