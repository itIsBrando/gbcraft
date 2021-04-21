#include "entity.h"
#include "level.h"
#include "bg.h"

#include <stdlib.h>
#include <string.h>
#include "text.h"

inline u8 abss(u8 a, u8 b)
{
    return a > b ? a - b : b - a;
}


/**
 * @param lvl level to inspect
 * @param x relative pixel x
 * @param y relative pixel y
 * @returns NULL if none, or an array of pointers to entities allocated on the heap
 */
ent_t **ent_get_all(level_t *lvl, u16 x, u16 y, u8 *outputSize)
{
    ent_t *buffer[50];
    *outputSize = 0;

    for(u16 i = 0; i < lvl->ent_size; i++)
    {
        ent_t *e = &lvl->entities[i];

        if((x >= e->x) && (x <= e->x + 16) && (y >= e->y) && (y <= e->y + 16)) {
            buffer[*outputSize] = e;
            outputSize[0]++;
        }
    }
    
    if(!*outputSize)
        return NULL;

    u16 size = *outputSize * sizeof(ent_t);
    ent_t **out = malloc(size);

    memcpy(out, buffer, size);

    return out;
}


// order matters. Based on `ent_type_t`
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
        .onupdate=ent_slime_update
    },
    { // zombie
        .onhurt=NULL,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=ent_zombie_update
    },
    { // furniture
        .onhurt=ent_furniture_onhurt,
        .maypass=ent_furniture_maypass,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=ent_furniture_update
    },
    { // item entity
        .onhurt=NULL,
        .maypass=NULL,
        .doDamage=NULL,
        .ontouch=ent_item_ontouch,
        .ondeath=NULL,
        .onupdate=ent_item_update,
    },
};


/**
 * Order matters. Based on ent_type_t
 */
static u16 __tiles[] = {
    1,  // player
    0,  // slime
    1,  // zombie
    0,  // furniture (set externally. see item_furniture_interact)
    0,  // item entity (set exeternally. See `ent_item_new()`)
};


/**
 * Holds the sizes of each entity.
 * Order matters based on  ent_type_t
 */
static const bounding_rect_t __rects[] = {
    {3, 2, 3, 0}, // player
    {0, 0, 0, 0}, // slime
    {3, 2, 2, 0}, // zombie
    {3, 2, 4, 1}, // furniture
    {0, 8, 0, 8}, // item
};


ent_t *ent_add(level_t *lvl, ent_type_t type, u16 x, u16 y)
{
    ent_t *ent = &lvl->entities[lvl->ent_size++];

    ent->type = type;
    ent->x = x;
    ent->y = y;
    ent->events = &events[type];
    ent->level = lvl;
    ent->sprite = spr_alloc(x, y, __tiles[type]);

    if(type == ENT_TYPE_PLAYER) {
        ent->player.inventory.parent = ent;
    }

	spr_set_size(ent->sprite, SPR_SIZE_16x16);
    return ent;
}


void ent_remove(level_t *lvl, ent_t *ent)
{
    u16 index = -1;
    for(u16 i = 0; i < lvl->ent_size; i++)
    {
        if(&lvl->entities[i] == ent) {
            index = i;
            break;
        }
    }

    spr_free(ent->sprite);

    // @todo report error
    if(index == -1)
        return;

    // if this entity is at the end of the table, ignore
    if(index == lvl->ent_size - 1) {
        lvl->ent_size--;
        return;
    }

    memcpy(&lvl->entities[index], &lvl->entities[index + 1], (lvl->ent_size - index) * sizeof(ent_t));

    lvl->ent_size--;
}


inline const bounding_rect_t *ent_get_bounding_rect(const ent_t *ent)
{
    return &__rects[ent->type];
}


/**
 * @param ent entity to check for valid movement
 * @param direction direction to move
 */
bool ent_can_move(ent_t *ent, const direction_t direction)
{
    const bounding_rect_t *rect = ent_get_bounding_rect(ent);
    u16 x = ent->x + bg_get_scx(main_background);
    u16 y = ent->y + bg_get_scy(main_background);

    x += dir_get_x(direction) * 2;
    y += dir_get_y(direction) * 2; 
    
    x += rect->sx;
    y += rect->sy;

    const u16 w = 16 - rect->sx - rect->ex,
     h = 16 - rect->sy - rect->ey;

    const u16 dx[] = {0, w, 0, w};
    const u16 dy[] = {0, 0, h, h};

    ent->dir = direction;

    for(u16 i = 0; i < 4; i++)
    {
        u16 tx = (x + dx[i]) >> 4;
        u16 ty = (y + dy[i]) >> 4;
        const tile_t *tile = lvl_get_tile(ent->level, tx, ty);
        bool (*maypass)(ent_t*) = tile->event->maypass;
        void (*ontouch)(ent_t*) = tile->event->ontouch;
        
        if(ontouch)
            ontouch(ent);

        if(maypass && !maypass(ent))
            return false;
    }

    // make sure that we don't collide into any solid entities
    u8 s;
    u16 px = ent->x + 8;
    u16 py = ent->y + 8;
    px += dir_get_x(direction) << 3, py += dir_get_y(direction) << 3;
    
    ent_t **e = ent_get_all(ent->level, px, py, &s);

    for(u16 i = 0; i < s; i++)
    {
        if(e[i]->events->maypass && !e[i]->events->maypass(e[i], ent))
        {
            free(e);
            return false;
        }
    }

    free(e);

    return true;
}

/**
 * @note both dx and dy != 0
 */
direction_t dir_get(const s16 dx, const s16 dy)
{   if(dx == 0 && dy == 0)
        return DIRECTION_NONE;
    
    if(dx == 0)
        return dy < 0 ? DIRECTION_UP : DIRECTION_DOWN;
    else if(dy == 0)
        return dx < 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
    
    // diagonal
    if(dx < 0 && dy < 0)
        return DIRECTION_LEFT_UP;
    else if(dx > 0 && dy < 0)
        return DIRECTION_RIGHT_UP;
    else if(dx < 0 && dy > 0)
        return DIRECTION_LEFT_DOWN;
    else if(dx > 0 && dy > 0)
        return DIRECTION_RIGHT_DOWN;
    
    return -1;
}


bool ent_move(ent_t *ent, const direction_t direction)
{
    if(ent_can_move(ent, direction))
    {
        ent->x += dir_get_x(direction);
        ent->y += dir_get_y(direction);
        return true;
    }

    return false;
}


/**
 * @param ent entity to move
 * @param dx x displacement
 * @param dy y displacement
 * @returns true if entity was moved
 */
bool ent_move_by(ent_t *ent, s16 dx, s16 dy)
{
    u16 oldx = ent->x, oldy = ent->y;

    ent->x += dx << 1;
    ent->y += dy << 1;

    if(!ent_can_move(ent, DIRECTION_NONE))
    {
        ent->x = oldx;
        ent->y = oldy;
        return false;
    } else {
        ent->x = oldx + dx;
        ent->y = oldy + dy;
        return true;
    }

}


void ent_draw(const ent_t *ent)
{
    spr_move(ent->sprite, ent->x, ent->y);
    if(ent_is_on_screen(ent)) {
        spr_show(ent->sprite);
    } else {
        spr_hide(ent->sprite);
    }
}


/**
 * Hides all of the entities on the level
 */
void ent_hide_all(level_t *level)
{
    for(u16 i = 0; i < level->ent_size; i++)
    {
        spr_hide(level->entities[i].sprite);
    }
    spr_copy_all();
}


/**
 * Shows all of the entities that are on screen
 */
void ent_show_all(level_t *level)
{
    for(u16 i = 0; i < level->ent_size; i++)
        ent_draw(&level->entities[i]);
}


/**
 * @returns true if the sprite is visible on screen
 */
bool ent_is_on_screen(const ent_t *ent)
{
    return ent->x <= 240 && ent->y <= 160;
}