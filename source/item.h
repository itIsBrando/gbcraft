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

extern const item_t ALL_ITEMS[];
/**
 * Removes an item from the item's inventory
 * @param item item that must be part of an inventory
 * @returns false if item could not be removed
 */
bool item_remove_from_inventory(item_t *item);



/**
 * Checks to see if an item can be used for attacking
 * @returns true if the item can attack another entity
 */
bool item_can_attack(const item_t *item);


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
 */
void item_add_to_inventory(const item_t *item, inventory_t *inv);

/**
 * Checks for an item in an inventory, very precise
 * @param item Item type and tooltype to match in inventory
 * @returns NULL if not found
 * @note more precise than `item_get_from_inventory()`
 */
item_t *item_get_from_inventory_matching(const item_t *item, const inventory_t *inv);

/**
 * Checks for an item in an inventory
 * @param type item type to check
 * @param inv inventory to search in
 * @returns NULL if not found
 * @warning cannot discern the difference between tools and furniture
 */
item_t *item_get_from_inventory(const item_type_t type, const inventory_t *inv);


/**
 * Updates an entity's position if it is on screen
 */
void item_change_count(item_t *item, const s8 change);

bool item_can_attack_all(item_t *item, ent_t *ent);

bool item_stone_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y);
bool item_tool_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y);
bool item_wood_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y);
bool item_furniture_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y);

#endif