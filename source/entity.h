#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"

direction_t dir_get(const s16 dx, const s16 dy);


const bounding_rect_t *ent_get_bounding_rect(const ent_t *ent);

bool ent_can_move(ent_t *ent, const direction_t direction);
bool ent_move(ent_t *ent, const direction_t direction);
bool ent_move_by(ent_t *ent, s16 dx, s16 dy); // @todo remove

void ent_hide_all(level_t *level);
void ent_show_all(level_t *level);
void ent_draw(const ent_t *ent);

bool ent_is_on_screen(const ent_t *ent);
ent_t *ent_add(level_t *lvl, ent_type_t type, u16 x, u16 y);
void ent_remove(level_t *lvl, ent_t *ent);
ent_t **ent_get_all(level_t *lvl, u16 x, u16 y, u8 *outputSize);

void ent_player_update(ent_t *plr);
void ent_slime_update(ent_t *s);
void ent_zombie_update(ent_t *zombie);
void ent_furniture_update(ent_t *e);

void ent_furniture_onhurt(ent_t *f, ent_t *plr);

bool ent_furniture_maypass(ent_t *f, ent_t *e);

void ent_player_set_active_item(ent_t *plr, item_t *item);
#endif