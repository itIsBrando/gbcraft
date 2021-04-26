#include "menu.h"
#include "entity.h"
#include "item.h"

#include "obj.h"
#include "text.h"
#include "memory.h"

#define CREATE_COST(itemType, amt) {\
 .item_type=(itemType),\
 .required_amount=amt,\
}


// sizes changes here need to be reflected in `furniture.h`
const recipe_t CRAFTING_RECIPES[] = {
    { // wood axe crafting
        .result=&ITEM_WOOD_AXE,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_WOOD, 5),
        }
    },
    { // wood pickaxe
        .result=&ITEM_WOOD_PICKAXE,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_WOOD, 5)
        }
    },
    { // wood sword
        .result=&ITEM_WOOD_SWORD,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_WOOD, 5)
        }
    },
    { // stone axe
        .result=&ITEM_STONE_AXE,
        .costs_num=2,
        .costs = {
            CREATE_COST(ITEM_TYPE_STONE, 5),
            CREATE_COST(ITEM_TYPE_WOOD, 5),
        }
    },
    { // stone pickaxe
        .result=&ITEM_STONE_PICKAXE,
        .costs_num=2,
        .costs = {
            CREATE_COST(ITEM_TYPE_STONE, 5),
            CREATE_COST(ITEM_TYPE_WOOD, 5)
        }
    },
    { // stone sword
        .result=&ITEM_STONE_SWORD,
        .costs_num=2,
        .costs = {
            CREATE_COST(ITEM_TYPE_STONE, 5),
            CREATE_COST(ITEM_TYPE_WOOD, 5),
        }
    },
    { // chest
        .result=&ITEM_CHEST,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_WOOD, 20),
        }
    },
    {
        .result=&ITEM_FURNITURE,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_STONE, 20),
        }
    },
};


void ent_furniture_update(ent_t *e)
{
    ent_draw(e);
}



bool ent_furniture_interact(ent_t *f, ent_t *plr, s8 dmg)
{
    switch (f->furniture.type)
    {
    case FURNITURE_TYPE_CRAFTING:
        mnu_open_crafting(plr);
        break;
    case FURNITURE_TYPE_CHEST:
        mnu_open_chest(f, plr);
        break;
    case FURNITURE_TYPE_FURNACE:
        mnu_open_crafting(plr);
        break;
    default:
        text_error("Unknown furniture type");
        break;
    }

    return false;
}


bool ent_furniture_maypass(ent_t *f, ent_t *e)
{
    return false;
}


// sprite indexes for 16x16 furniture sprites
static const u8 _fur_spr[] = {53, 49};

void ent_furniture_set_tile(ent_t *e) {
    spr_set_tile(e->sprite, _fur_spr[e->furniture.type]);
}