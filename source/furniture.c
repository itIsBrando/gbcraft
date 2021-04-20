#include "menu.h"
#include "entity.h"
#include "item.h"

#include "obj.h"
#include "memory.h"


const recipe_t CRAFTING_RECIPES[] = {
    { // stone axe crafting
        .result=&ITEM_STONE_AXE,
        .costs_num=2,
        .costs = {
            {
                .item_type=ITEM_TYPE_STONE,
                .required_amount=6,
            },
            {
                .item_type=ITEM_TYPE_WOOD,
                .required_amount=8,
            }
        }
    },
    { // stone pickaxe
        .result=&ITEM_STONE_PICKAXE,
        .costs_num=2,
        .costs = {
            {
                .item_type=ITEM_TYPE_STONE,
                .required_amount=6,
            },
            {
                .item_type=ITEM_TYPE_WOOD,
                .required_amount=10,
            }
        }
    },
    { // stone sword
        .result=&ITEM_STONE_SWORD,
        .costs_num=2,
        .costs = {
            {
                .item_type=ITEM_TYPE_STONE,
                .required_amount=6,
            },
            {
                .item_type=ITEM_TYPE_WOOD,
                .required_amount=10,
            }
        }
    },
};


void ent_furniture_update(ent_t *e)
{

    ent_draw(e);
}



void ent_furniture_interact(ent_t *f, ent_t *plr, s8 dmg)
{
    if(f->furniture == FURNITURE_TYPE_CRAFTING)
    {
        mnu_open_crafting(plr);
    }
}


bool ent_furniture_maypass(ent_t *f, ent_t *e)
{
    return false;
}