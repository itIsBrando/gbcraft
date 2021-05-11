#ifndef ITEM_H
#define ITEM_H

#include "types.h"

#define ITEM_WOOD           ALL_ITEMS[0]
#define ITEM_STONE          ALL_ITEMS[1]
#define ITEM_STONE_AXE      ALL_ITEMS[2]
#define ITEM_STONE_PICKAXE  ALL_ITEMS[3]
#define ITEM_STONE_SWORD    ALL_ITEMS[4]
#define ITEM_PICKUP         ALL_ITEMS[5]
#define ITEM_BENCH          ALL_ITEMS[6]
#define ITEM_WOOD_AXE       ALL_ITEMS[7]
#define ITEM_WOOD_PICKAXE   ALL_ITEMS[8]
#define ITEM_WOOD_SWORD     ALL_ITEMS[9]
#define ITEM_CHEST          ALL_ITEMS[10]
#define ITEM_IRON_ORE       ALL_ITEMS[11]
#define ITEM_GOLD_ORE       ALL_ITEMS[12]
#define ITEM_IRON           ALL_ITEMS[13]
#define ITEM_GOLD           ALL_ITEMS[14]
#define ITEM_FURNACE        ALL_ITEMS[15]
#define ITEM_COAL           ALL_ITEMS[16]
#define ITEM_DOOR           ALL_ITEMS[17]
#define ITEM_FLOOR_WOOD     ALL_ITEMS[18]
#define ITEM_WOOD_HOE       ALL_ITEMS[19]
#define ITEM_SEED           ALL_ITEMS[20]
#define ITEM_SAPLING        ALL_ITEMS[21]
#define ITEM_WHEAT          ALL_ITEMS[22]
#define ITEM_BREAD          ALL_ITEMS[23]
#define ITEM_ANVIL          ALL_ITEMS[24]
#define ITEM_IRON_AXE       ALL_ITEMS[25]
#define ITEM_IRON_PICKAXE   ALL_ITEMS[26]
#define ITEM_IRON_SWORD     ALL_ITEMS[27]
#define ITEM_APPLE          ALL_ITEMS[28]
#define ITEM_LANTERN        ALL_ITEMS[29]
#define ITEM_SLIME          ALL_ITEMS[30]
#define ITEM_GOLDEN_APPLE   ALL_ITEMS[31]

extern const item_t ALL_ITEMS[];
/**
 * Removes an item from the item's inventory
 * @param item item that must be part of an inventory
 * @returns false if item could not be removed
 */
bool item_remove_from_inventory(item_t *item);


/**
 * Looks up the name of an item based on its type and other properties
 * @note used in loading save data bc pointers decay
 * @todo very inefficient. improve!!!
 * @returns pointer to string
 */
const char *item_lookup_name(item_t *item);

/**
 * @param type ITEM_TYPE_...
 * @param data used for identifying tools (tooltype) or furniture (furniture_t). Use -1 to ignore
 * @returns the data `item_t` based on the type of item, or NULL
 */
const item_t *item_get_from_type(item_type_t type, s16 data);


/**
 * Adds an item to an inventory
 * @param item does not need to outlive `inv`
 * @param inv inventory to add item to
 * @returns pointer to the newly added item
 */
item_t *item_add_to_inventory(const item_t *item, inventory_t *inv);

/**
 * Checks for an item in an inventory, very precise
 * @param item Item type and tooltype to match in inventory
 * @returns NULL if not found
 * @note more precise than `item_get_from_inventory()`
 */
item_t *item_get_from_inventory_matching(const item_t *item, const inventory_t *inv);


/**
 * @param item item to find in `list`
 * @param list array of pointers to item_t
 * @param size number of elements in `list`
 * @see item_get_from_inventory_matching
 */
item_t *item_get_perfect_match(const item_t *item, const item_t *list, const uint size);

/**
 * Checks for an item in an inventory
 * @param type item type to check
 * @param inv inventory to search in
 * @returns NULL if not found
 * @warning cannot discern the difference between tools and furniture
 */
item_t *item_get_from_inventory(const item_type_t type, const inventory_t *inv);


/**
 * Creates a sprite that shows the graphical representation of an item
 * @param item item used for the icon
 * @param x pixel x coordinate
 * @param y pixel y coordinate
 */
obj_t *item_new_icon(item_t *item , uint x, uint y);


/**
 * Same as `item_new_icon()` but does not create a new sprite
 * @param obj sprite
 * @param item item
 */
void item_set_icon(obj_t *obj, const item_t *item);

/**
 * Updates an entity's position if it is on screen
 */
void item_change_count(item_t *item, const s8 change);

bool item_can_attack_all(item_t *item, ent_t *ent);

bool item_stone_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_tool_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_wood_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_furniture_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_door_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_floor_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);

bool item_sapling_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_seed_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
bool item_food_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y);
#endif