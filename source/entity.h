#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"

direction_t dir_get(const s16 dx, const s16 dy);


const bounding_rect_t *ent_get_bounding_rect(const ent_t *ent);
uint ent_get_tile(const ent_t *e);

bool ent_can_move(ent_t *ent, const direction_t direction);
bool ent_move(ent_t *ent, const direction_t direction);
void ent_apply_knockback(ent_t *e);

void ent_hide_all(level_t *level);
void ent_show_all(level_t *level);
void ent_draw(const ent_t *ent);

bool ent_is_on_screen(const ent_t *ent);
ent_t *ent_add(level_t *lvl, ent_type_t type, u16 x, u16 y);
ent_t *ent_change_level(ent_t *e, level_t *newLevel);
void ent_remove(level_t *lvl, ent_t *ent);

/**
 * Removes an entity from the level
 * @param e entity
 */
void ent_kill(ent_t *e);

void plr_move_to(ent_t *plr, uint x, uint y);

ent_t **ent_get_all(level_t *lvl, u16 x, u16 y, u8 *outputSize);
uint ent_get_all_stack(level_t *lvl, ent_t **buffer, u16 x, u16 y, u8 maxSize);

void ent_furniture_set_tile(ent_t *e);
u8 ent_furniture_get_tile(const ent_t *e);
uint ent_item_get_tile(const ent_t *e);

void ent_load_events(ent_t *e);

void ent_slime_init(ent_t *e);
void ent_zombie_init(ent_t *e);
void ent_player_init(ent_t *e);
void ent_furniture_init(ent_t *e);

void ent_player_update(ent_t *plr);
void ent_slime_update(ent_t *s);
void ent_zombie_update(ent_t *zombie);
void ent_furniture_update(ent_t *e);
void ent_item_update(ent_t *e);

bool ent_furniture_interact(ent_t *, ent_t *, s8);
void ent_player_interact(const ent_t *plr); // not a standard event
bool ent_slime_hurt(ent_t *, ent_t *, s8);
bool ent_zombie_hurt(ent_t *e, ent_t *atker, s8 damage);

void ent_player_onrelocate(ent_t *eOld, ent_t *eNew);

bool ent_furniture_maypass(ent_t *f, ent_t *e);

void ent_item_ontouch(ent_t *e, ent_t *other, u16 x, u16 y);

void ent_item_new(level_t *level, u16 x, u16 y, const item_t *item, u8 cnt);

void ent_player_set_active_item(ent_t *plr, item_t *item);
bool ent_player_heal(ent_t *e, uint by);

#endif