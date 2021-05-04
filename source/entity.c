#include "entity.h"
#include "level.h"
#include "player.h"

#include "bg.h"
#include "obj.h"

#include <stdlib.h>
#include <string.h>
#include "text.h"

inline u8 abss(u8 a, u8 b)
{
    return a > b ? a - b : b - a;
}


/**
 * @param lvl level to inspect
 * @param ignoredEntity This entity will not appear in `buffer`. Can be NULL
 * @param buffer buffer of entities to fill
 * @param x relative pixel x
 * @param y relative pixel y
 * @param maxSize max number of entities that the buffer can hold
 * @returns NULL if none, or an array of pointers to entities allocated on the heap
 */
uint ent_get_all_stack(level_t *lvl, const ent_t *ignoredEntity, ent_t **buffer, u16 x, u16 y, u8 maxSize)
{
    uint index = 0;

        ent_t *e = lvl->entities;
    for(uint i = 0; i < lvl->ent_size; i++)
    {
        if(e == ignoredEntity)
        {
            e++;
            continue;
        }
        if((x >= e->x) && (x <= e->x + 16) && (y >= e->y) && (y <= e->y + 16))
        {
            buffer[index++] = e;
            if(index >= maxSize)
                return maxSize;
        }
        e++;
    }
    
    return index;
}


/**
 * @param lvl level to inspect
 * @param x relative pixel x
 * @param y relative pixel y
 * @returns NULL if none, or an array of pointers to entities allocated on the heap
 * @warning very slow!!! prefer `ent_get_all_stack`
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
            (*outputSize)++;
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
static const ent_event_t events[] = {
    { // player
        .init=ent_player_init,
        .onrelocate=ent_player_onrelocate,
        .onhurt=NULL,
        .ontouch=ent_mob_ontouch,
        .ondeath=NULL,
        .onupdate=ent_player_update
    },
    { // slime
        .init=ent_slime_init,
        .onhurt=ent_slime_hurt,
        .ontouch=NULL,
        .ondeath=NULL,
        .maypass=NULL,
        .onupdate=ent_slime_update
    },
    { // zombie
        .init=ent_zombie_init,
        .onhurt=ent_zombie_hurt,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=ent_zombie_update
    },
    { // furniture
        .init=ent_furniture_init,
        .onhurt=ent_furniture_interact,
        .maypass=ent_furniture_maypass,
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
    0,  // item entity (set externally. See `ent_item_new()`)
};

uint ent_get_tile(const ent_t *e) {
    if(e->type == ENT_TYPE_FURNITURE)
        return ent_furniture_get_tile(e);
    else if(e->type == ENT_TYPE_ITEM_ENTITY)
        return ent_item_get_tile(e);
    else
        return __tiles[e->type];
}


/**
 * Holds the sizes of each entity.
 * Order matters based on  ent_type_t
 */
static const bounding_rect_t __rects[] = {
    {3, 2, 3, 1}, // player
    {2, 2, 1, 3}, // slime
    {3, 2, 2, 0}, // zombie
    {3, 2, 4, 1}, // furniture
    {0, 8, 0, 8}, // item
};

/**
 * Sets the entity's events pointer based on its type
 */
void ent_load_events(ent_t *e)
{
    e->events = &events[e->type];
}


/**
 * @returns NULL if unable to create
 */
ent_t *ent_add(level_t *lvl, ent_type_t type, u16 x, u16 y)
{
    if(lvl->ent_size >= 50)
        return NULL;
    
    ent_t *ent = &lvl->entities[lvl->ent_size++];

    memset(ent, 0, sizeof(ent_t));
    ent->type = type;
    ent->x = x;
    ent->y = y;
    ent->level = lvl;
    ent->sprite = spr_alloc(x, y, 0);
    ent_load_events(ent);

    if(ent->events->init) {
        ent->events->init(ent);
    }

    spr_set_tile(ent->sprite, ent_get_tile(ent));
	spr_set_size(ent->sprite, SPR_SIZE_16x16);
    spr_set_priority(ent->sprite, SPR_PRIORITY_HIGH);
    spr_set_pal(ent->sprite, 0);
    return ent;
}


/**
 * Moves an entity from one layer to another.
 * @note does NOT remove `e` from other level
 * @returns new location of entity
 */
ent_t *ent_change_level(ent_t *e, level_t *newLevel)
{
    ent_t *ent = &newLevel->entities[newLevel->ent_size++];
    obj_t spr = *e->sprite;
    memcpy(ent, e, sizeof(ent_t));

    ent->level = newLevel;
    if(e->type == ENT_TYPE_PLAYER)
    {
        newLevel->player = ent;
        e->player.removed = true;
    }
    
    ent->sprite = spr_alloc(spr_get_x(&spr), spr_get_y(&spr), spr_get_tile(&spr));
    spr_set_size(ent->sprite, SPR_SIZE_16x16);

    if(e->events->onrelocate)
        e->events->onrelocate(e, ent);

    return ent;
}


void ent_remove(level_t *lvl, ent_t *ent)
{
    int index = -1;
    for(uint i = 0; i < lvl->ent_size; i++)
    {
        if(&lvl->entities[i] == ent) {
            index = i;
            break;
        }
    }

    if(ent->type == ENT_TYPE_SLIME || ent->type == ENT_TYPE_ZOMBIE)
        lvl->mob_density--;

    spr_free(ent->sprite);

    // @todo report error
    if(index == -1)
        return;

    // if this entity is at the end of the table, ignore
    if(index == lvl->ent_size - 1) {
        lvl->ent_size--;
        return;
    }

    for(uint i = index; i < lvl->ent_size; i++)
    {
        lvl->entities[i] = lvl->entities[i + 1];
    }
    // memcpy(&lvl->entities[index], &lvl->entities[index + 1], (lvl->ent_size - index) * sizeof(ent_t));

    lvl->ent_size--;
}


void ent_kill(ent_t *e)
{
    if(e->events->ondeath)
        e->events->ondeath(e);

    ent_remove(e->level, e);
}


inline const bounding_rect_t *ent_get_bounding_rect(const ent_t *ent)
{
    return &__rects[ent->type];
}


/**
 * @param ent entity to check for valid movement
 * @param direction direction to move
 * @param dist Number of pixels to check for movement
 */
bool ent_can_move(ent_t *ent, const direction_t direction, uint dist)
{
    const bounding_rect_t *rect = ent_get_bounding_rect(ent);
    uint x = ent->x + bg_get_scx(main_background);
    uint y = ent->y + bg_get_scy(main_background);
    int cx = dir_get_x(direction);
    int cy = dir_get_y(direction);

    x += cx * dist + cx;
    y += cy * dist + cy;
    
    x += rect->sx;
    y += rect->sy;

    const uint w = 16 - rect->sx - rect->ex,
     h = 16 - rect->sy - rect->ey;

    const uint dx[] = {0, w, 0, w};
    const uint dy[] = {0, 0, h, h};

    ent->dir = direction;

    for(uint i = 0; i < 4; i++)
    {
        uint tx = (x + dx[i]) >> 4;
        uint ty = (y + dy[i]) >> 4;
        const tile_t *tile = lvl_get_tile(ent->level, tx, ty);
        const tile_event_t *events = tile->event;

        if(events->maypass && !events->maypass(ent))
            return false;
        
        // @todo fix this. The stairways down requires this
        //   function to return after switching levels
        if(events->ontouch) {
            if(events->ontouch(ent, tx, ty))
                return false;
        }
    }

    // make sure that we don't collide into any solid entities
    uint px = ent->x + 8;
    uint py = ent->y + 8;
    px += dir_get_x(direction) << 3, py += dir_get_y(direction) << 3;
    
    ent_t *e[3];
    uint s = ent_get_all_stack(ent->level, ent, e, px, py, 3);

    for(uint i = 0; i < s; i++)
    {
        const ent_event_t *events = e[i]->events;
        if(events->maypass && !events->maypass(e[i], ent))
            return false;

        if(events->ontouch && events->ontouch(e[i], ent, px, py))
            return false;
    }

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


/**
 * @param ent
 * @param direction direction_t
 * @param dist number of pixels to move in the direction dictated by `direction` parameter
 */
bool ent_move(ent_t *ent, const direction_t direction, const uint dist)
{
    if(ent_can_move(ent, direction, dist))
    {
        ent->x += dir_get_x(direction) * dist;
        ent->y += dir_get_y(direction) * dist;
        return true;
    }

    return false;
}


inline void _dec_magnitude(s8 *a)
{
    if(*a)
        *a < 0 ? (*a)++ : (*a)--;
}

inline u8 _get_magnitude(int a)
{
    return a < 0 ? -a : a;
}


void ent_apply_knockback(ent_t *e)
{
    direction_t d = dir_get(e->xKnockback, e->yKnockback);
    const int xMag = _get_magnitude(e->xKnockback);
    const int yMag = _get_magnitude(e->yKnockback);
    int dist = xMag != 0 ? xMag : yMag;

    ent_move(e, d, dist);
    
    _dec_magnitude(&e->xKnockback);
    _dec_magnitude(&e->yKnockback);
}


void plr_apply_knockback(ent_t *e)
{
    if(e->xKnockback == 0 && e->yKnockback == 0)
        return;

    const int dist = e->xKnockback ? e->xKnockback : e->yKnockback;
    direction_t d = e->dir;

    plr_move_by(e, dir_get(e->xKnockback, e->yKnockback), abs(dist));

    e->dir = d; // restore direction

    _dec_magnitude(&e->xKnockback);
    _dec_magnitude(&e->yKnockback);
}


/**
 * Does damage to the player
 * @param e mob
 * @param other player
 */
bool ent_mob_ontouch(ent_t *e, ent_t *other, u16 x, u16 y)
{
    if(other->type != ENT_TYPE_SLIME)
        return false;
    
    plr_hurt(other, e, e->level->layer + 1);

    return true;
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
 * @param level pointer to level with entities
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
    const int x = (s16)ent->x, y = (s16)ent->y;
    return x > -16 &&  y > -16 && x <= 240 && y <= 160;
}