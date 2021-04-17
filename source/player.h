#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"


bool plr_pay_stamina(ent_t *plr, s8 amt);

void plr_move_by(ent_t *player, const direction_t direction);

#endif