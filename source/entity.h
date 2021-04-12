#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"

typedef struct {
    uint health;
    uint max_health;
    // item_t *activeItem;
    // inventory_t inventory;
    uint invulnerableTime;
} player_t;


void ent_player_update(ent_t *plr);
void ent_add(level_t *lvl, ent_type_t type, uint x, uint y);

#endif