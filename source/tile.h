#ifndef TILE_H
#define TILE_H

#include "types.h"

extern const tile_t tile_tile_data[];

tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, u16 x, u16 y);
const tile_t *tile_get(tile_type_t type);

bool tile_water_maypass(ent_t *e);
bool tile_grass_maypass(ent_t *e);
bool tile_no_pass(ent_t *e);

bool tile_stair_down_ontouch(ent_t *e, uint x, uint y);
bool tile_stair_up_ontouch(ent_t *e, uint x, uint y);

bool tile_tree_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_stone_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_wood_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_door_closed_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_door_open_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_grass_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_iron_interact(ent_t *ent, item_t *item, uint x, uint y);
bool tile_floor_interact(ent_t *ent, item_t *item, uint x, uint y);

bool tile_wheat_interact(ent_t *ent, item_t *item, uint x, uint y);
void tile_mud_tick(level_t *, uint, uint);
void tile_sapling_tick(level_t *lvl, uint x, uint y);
void tile_seed_tick(level_t *lvl, uint x, uint y);

#endif