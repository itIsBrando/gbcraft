#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"


const bounding_rect_t *ent_get_bounding_rect(const ent_t *ent);

bool ent_can_move(ent_t *ent, const direction_t direction);
void ent_move(ent_t *ent, const direction_t direction);


void ent_draw(const ent_t *ent);

bool ent_is_on_screen(const ent_t *ent);
ent_t *ent_add(level_t *lvl, ent_type_t type, u16 x, u16 y);

void ent_player_update(ent_t *plr);
void ent_zombie_update(ent_t *zombie);

void ent_player_set_active_item(ent_t *plr, item_t *item);
#endif