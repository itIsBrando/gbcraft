#ifndef TILE_H
#define TILE_H

#include "types.h"

extern const tile_t tile_tile_data[];

void tile_render_use_recursion(bool b);
void tile_render_nearby(tile_surround_mask mask, const level_t *lvl, const tile_t *tile, u16 x, u16 y);

tile_surround_mask tile_get_surrounding(level_t *lvl, tile_type_t type, u16 x, u16 y);
const tile_t *tile_get(tile_type_t type);

void tile_render(const BG_REGULAR *bg, const level_t *lvl, const tile_t *tile, u16 x, u16 y);

bool tile_water_maypass(ent_t *e);
bool tile_grass_maypass(ent_t *e);
bool tile_no_pass(ent_t *e);

void tile_stair_down_ontouch(ent_t *e, uint x, uint y);
void tile_stair_up_ontouch(ent_t *e, uint x, uint y);

void tile_tree_interact(ent_t *ent, item_t *item, u16 x, u16 y);
void tile_stone_interact(ent_t *ent, item_t *item, u16 x, u16 y);
void tile_wood_interact(ent_t *ent, item_t *item, u16 x, u16 y);
void tile_door_closed_interact(ent_t *ent, item_t *item, u16 x, u16 y);
void tile_door_open_interact(ent_t *ent, item_t *item, u16 x, u16 y);

#endif