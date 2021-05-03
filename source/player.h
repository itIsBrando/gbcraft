#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"


void plr_move_by(ent_t *player, const direction_t direction, const uint dist);

void player_set_hotbar_pos(ent_t *p, int index);
void player_change_hotbar_pos(ent_t *p, int a);
#endif