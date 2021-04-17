#include "entity.h"
#include "level.h"
#include "bg.h"

#include <stdlib.h>
#include <string.h>


/**
 * @param lvl level to inspect
 * @param x absolute x
 * @param y absolute y
 * @returns NULL if none, or an array of pointers to entities allocated on the heap
 */
ent_t **ent_get_all(level_t *lvl, u16 x, u16 y, u8 *outputSize)
{
    ent_t *buffer[50];
    
    for(u16 i = 0; i < lvl->ent_size; i++)
    {
        ent_t *e = &lvl->entities[i];

        if(e->x >= x && e->y >= y && e->x + 16 <= y && e->y + 16 <= y)
            buffer[*outputSize++] = e;
    }

    u16 size = *outputSize * sizeof(ent_t);
    ent_t **out = malloc(size);

    memcpy(out, buffer, size);

    return out;
}


/**
 * @param ent entity to move
 * @param direction direction to move entity
 */
void ent_move_by(ent_t *ent, direction_t direction)
{
    u16 x = ent->x + dir_get_x(direction), y = ent->y + dir_get_y(direction);

    u16 tx = x >> 4;
    u16 ty = y >> 4;
    if(lvl_get_tile((level_t*)ent->level, tx, ty)->event->maypass(ent))
        ent->x = x, ent->y = y;
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
        .onupdate=NULL
    },
    { // zombie
        .onhurt=NULL,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=ent_zombie_update
    },
    { // slime
        .onhurt=NULL,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=NULL
    },
    { // furniture
        .onhurt=ent_furniture_onhurt,
        .doDamage=NULL,
        .ontouch=NULL,
        .ondeath=NULL,
        .onupdate=ent_furniture_update
    }
};

/**
 * Order matters. Based on ent_type_t
 */
static u16 __tiles[] = {
    1,  // player
    0,  // slime
    1,  // zombie
    0,  // furniture (set externally. see item_furniture_interact)
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

    if(type == ENT_TYPE_PLAYER)
    {
        ent->player.inventory.parent = ent;
    }

	spr_set_size(ent->sprite, SPR_SIZE_16x16);
    return ent;
}


void ent_remove(level_t *lvl, ent_t *ent)
{
    spr_free(ent->sprite);
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
    if(direction != -1)
    {
        x += dir_get_x(direction) * 2;
        y += dir_get_y(direction) * 2; 
    }
    x += rect->sx;
    y += rect->sy;

    const u16 w = 16 - rect->sx - rect->ex,
     h = 16 - rect->sy - rect->ey;

    uint16_t dx[] = {0, 1, 0, 1};
    uint16_t dy[] = {0, 0, 1, 1};

    ent->dir = direction;

    for(u16 i = 0; i < 4; i++)
    {
        u16 tx = (x + dx[i] * w) >> 4;
        u16 ty = (y + dy[i] * h) >> 4;
        const tile_t *tile = lvl_get_tile(ent->level, tx, ty);
        bool (*maypass)(ent_t*) = tile->event->maypass;
        void (*ontouch)(ent_t*) = tile->event->ontouch;
        
        if(ontouch)
            ontouch(ent);

        if(maypass && !maypass(ent))
            return false;
    }

    return true;
}


void ent_move(ent_t *ent, const direction_t direction)
{
    if(ent_can_move(ent, direction))
    {
        ent->x += dir_get_x(direction);
        ent->y += dir_get_y(direction);
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