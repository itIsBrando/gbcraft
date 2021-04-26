#ifndef MENU_H
#define MENU_H

#include "types.h"

void mnu_show_inventory(ent_t *player);

void mnu_draw_hotbar(ent_t *player);
void mnu_draw_item(item_t *item, uint x, uint y);

void mnu_free_item_list(obj_t **icons, u8 size);
obj_t **mnu_draw_item_list(item_t *items, u8 size, uint tx, uint ty);
void mnu_open_crafting(ent_t *plr);
void mnu_open_chest(ent_t *e, ent_t *plr);
uint mnu_open_main();


#endif