#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"

level_t *lvl_new(u16 layer, level_t *parent);
void lvl_set_target_background(BG_REGULAR *bg);

ent_t *lvl_get_player(const level_t *lvl);

tile_type_t lvl_get_tile_type(const level_t *lvl, u16 x, u16 y);
const tile_t *lvl_get_tile(level_t *lvl, u16 x, u16 y);

u8 lvl_get_data(level_t *lvl, u16 x, u16 y);
void lvl_set_data(level_t *lvl, u16 x, u16 y, u8 v);

void lvl_set_tile(level_t *lvl, u16 x, u16 y, const tile_t *tile);


void lvl_blit(level_t *lvl);

#endif