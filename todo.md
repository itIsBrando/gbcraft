# Todo List


### Features
- [ ] Add real player tracking to zombie
- [ ] Add player side sprite
- [ ] Add chest
- [ ] Add vblank interrupt to copy `obj_t` to OAM
- [ ] Add `interrupt.h`
- [ ] Allow `bg_write_tile()` to accept coordinates greater than 32 for regular backgrounds
- [ ] Move tile rending functions into a new file, `render.c`/`.h`
- [ ] Add tile drops
- [ ] Create sub-classes for entity creation. ex: `ent_slime_add`
- [ ] Add slime tile
- [x] Add `hotbar.h`
- [x] Add slime entity
- [x] Subtract from stamina upon using a tool
- [x] Add player back sprite
- [x] Require block breaking to use multiple hits
- [x] Add stamina to hotbar
- [x] Replenish stamina over time
- [x] Add tree sprite
- [x] Add tree tile
- [x] Add crafting table
- [x] Add *powerglove*
- [x] Add pointer to player entity in `level_t`
- [x] Add `ent_remove(level_t *, ent_t *)`
- [x] Allow item crafting
- [x] Add `recipe_t`
- [x] Add terrain generation
- [x] Add supporting for `9pt indexing`
- [x] Add player's health to the hotbar
- [x] Add stone
- [x] Add axe
- [x] Add inventory GUI
- [x] Add the player's active item to the hotbar
- [x] prevent duplicates in inventory
- [x] Add `zombie_t`
- [x] Add `inventory_t`
- [x] Add `item_t`
- [x] Support large GBA mapping mode
- [x] change bg map to *affine* mapping
- [x] Add bounding rectangles for entities


### Polish
- [ ] Move & cleanup player's cursor sprite handling
- [ ] Prevent user from placing block on the tile that it occupies
- [ ] Animate player
- [ ] Prevent furniture from being able to be placed in the water
- [ ] Add indication that a tile is being destroyed
- [ ] Add inner-corner tiles for `9pt indexing`
- [ ] Create a modified version of `ent_get_all` that uses the stack instead of the heap
- [x] Allow `A` button for exiting inventory
- [x] Hide entities that leave visible screen


### Bug
- [ ] Confirm functional collision with other entities
- [ ] Out-of-bounds placing tiles is weird
- [ ] Blocks can be placed on top of entities
- [ ] Entities are visible above the hotbar
- [ ] Grass does not connect to tree
- [ ] Entities are drawn above the hotbar
- [ ] Cursor position does not line up with tile placement
- [ ] Game crashes opening inventory menu with no items
- [ ] `bg.h` does not support regular backgrounds with sizes larger than 32x32
- [x] **Crafting table does not open**
- [x] Fix weird cursor positioning