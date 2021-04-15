#ifndef MENU_H
#define MENU_H

#include "types.h"

void mnu_show_inventory(ent_t *player);

void mnu_draw_hotbar(ent_t *player);
void mnu_draw_item(item_t *item, u16 x, u16 y);

#endif