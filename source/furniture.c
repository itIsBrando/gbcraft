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
    {
        .result=&ITEM_FLOOR_WOOD,
        .amount=4,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_WOOD, 2),
        },
    },
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
    { // door
        .result=&ITEM_DOOR,
        .costs_num=1,
        .costs = {
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
    { // furnace
        .result=&ITEM_FURNACE,
        .costs_num=1,
        .costs = {
            CREATE_COST(ITEM_TYPE_STONE, 20),
        }
    },
};


const recipe_t FURNACE_RECIPES[] = {
    {
        .result=&ITEM_IRON,
        .amount=5,
        .costs_num=2,
        .costs = {
            CREATE_COST(ITEM_TYPE_IRON_ORE, 5),
            CREATE_COST(ITEM_TYPE_COAL, 1),
        }
    },
    {
        .result=&ITEM_GOLD,
        .amount=5,
        .costs_num=2,
        .costs = {
            CREATE_COST(ITEM_TYPE_GOLD_ORE, 5),
            CREATE_COST(ITEM_TYPE_COAL, 1),
        }
    }
};


void ent_furniture_update(ent_t *e)
{
    if(ent_is_on_screen(e))
        spr_move(e->sprite, e->x, e->y);
}


void ent_furniture_init(ent_t *e)
{
    e->furniture.inventory.size = 0;
    e->furniture.inventory.parent = e;
}


bool ent_furniture_interact(ent_t *f, ent_t *plr, s8 dmg)
{
    switch (f->furniture.type)
    {
    case FURNITURE_TYPE_CRAFTING:
        mnu_open_crafting(plr, CRAFTING_RECIPES, sizeof(CRAFTING_RECIPES) / sizeof(CRAFTING_RECIPES[0]));
        break;
    case FURNITURE_TYPE_CHEST:
        mnu_open_chest(f, plr);
        break;
    case FURNITURE_TYPE_FURNACE:
        mnu_open_crafting(plr, FURNACE_RECIPES, sizeof(FURNACE_RECIPES) / sizeof(FURNACE_RECIPES[0]));
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
const u8 _fur_spr[] = {53, 49, 45};

inline u8 ent_furniture_get_tile(const ent_t *e)
{
    return _fur_spr[e->furniture.type];
}

void ent_furniture_set_tile(ent_t *e) {
    spr_set_tile(e->sprite, ent_furniture_get_tile(e));
}