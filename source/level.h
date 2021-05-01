#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"

extern uint lvl_ticks;
extern level_t *world[4];
extern level_t *lvl_current;

level_t *lvl_new(u16 layer, level_t *parent);
void lvl_set_target_background(BG_REGULAR *bg);

ent_t *lvl_get_player(const level_t *lvl);

tile_type_t lvl_get_tile_type(const level_t *lvl, uint x, uint y);
const tile_t *lvl_get_tile(level_t *lvl, uint x, uint y);

u8 lvl_get_data(level_t *lvl, uint x, uint y);
void lvl_set_data(level_t *lvl, uint x, uint y, u8 v);

void lvl_set_tile(level_t *lvl, uint x, uint y, const tile_t *tile);

uint lvl_to_pixel_x(uint tx);
uint lvl_to_pixel_y(uint ty);
uint lvl_to_tile_x(uint px);
uint lvl_to_tile_y(uint py);

bool lvl_try_spawn_position(level_t *lvl, uint *x, uint *y);
void lvl_try_spawn(level_t *level, uint tries);


void lvl_blit();

void lvl_change_level(level_t *newLevel);
level_t *lvl_get_current();

#endif