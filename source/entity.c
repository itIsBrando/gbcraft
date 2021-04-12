#include "entity.h"
#include "level.h"

#include <stdlib.h>


/**
 * @param lvl level to inspect
 * @param x absolute x
 * @param y absolute y
 * @returns NULL if none, or an array of pointers to entities allocated on the heap
 */
ent_t **ent_get_all(level_t *lvl, uint x, uint y, uint *outputSize)
{
    ent_t *buffer[50];
    
    for(uint i = 0; i < lvl->ent_size; i++)
    {
        ent_t *e = &lvl->entities[i];

        if(e->x >= x && e->y >= y && e->x + 16 <= y && e->y + 16 <= y)
            buffer[*outputSize++] = e;
    }

    return malloc(*outputSize * sizeof(ent_t));
}

/**
 * @param ent entity to move
 * @param direction direction to move entity
 */
void ent_move_by(ent_t *ent, direction_t direction)
{
    uint x = ent->x + dir_get_x(direction), y = ent->y + dir_get_y(direction);

    uint tx = x >> 4;
    uint ty = y >> 4;
    if(lvl_get_tile((level_t*)ent->level, tx, ty)->maypass(ent))
        ent->x = x, ent->y = y;
}


static ent_event_t events[] = {
    { // player
        .onhurt=NULL,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=ent_player_update
    },
    { // slime
        .onhurt=NULL,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=NULL
    }
};

static uint __tiles[] = {
    1,  // player
        // slime
};


void ent_add(level_t *lvl, ent_type_t type, uint x, uint y)
{
    ent_t *ent = &lvl->entities[lvl->ent_size++];

    ent->type = type;
    ent->x = x;
    ent->y = y;
    ent->events = events[type];
    ent->level = lvl;
    ent->sprite = spr_alloc(x, y, __tiles[type]);

	spr_set_size(ent->sprite, SPR_SIZE_16x16);
}