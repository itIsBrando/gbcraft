#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"


bool plr_pay_stamina(ent_t *plr, s8 amt);

void plr_move_by(ent_t *player, const direction_t direction);

void player_set_hotbar_pos(ent_t *p, uint index);
void player_change_hotbar_pos(ent_t *p, int a);
#endif