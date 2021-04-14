#ifndef ITEM_H
#define ITEM_H

#include "types.h"

#define ITEM_WOOD   ALL_ITEMS[0]
#define ITEM_STONE  ALL_ITEMS[1]

extern const item_t ALL_ITEMS[];
/**
 * Removes an item from the item's inventory
 * @param item item that must be part of an inventory
 * @returns false if item could not be removed
 */
bool item_remove_from_inventory(item_t *item);


/**
 * Adds an item to an inventory
 * @param item does not need to outlive `inv`
 * @param inv inventory to add item to
 */
void item_add_to_inventory(item_t *item, inventory_t *inv);

/**
 * Checks for an item in an inventory
 * @param type item type to check
 * @param inv inventory to search in
 * @returns NULL if not found
 */
item_t *item_get_from_inventory(const item_type_t type, const inventory_t *inv);


/**
 * Updates an entity's position if it is on screen
 */
void item_change_count(item_t *item, const s8 change);


bool item_wood_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y);

#endif