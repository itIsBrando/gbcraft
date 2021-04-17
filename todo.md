# Todo List


### Features
- [ ] Add slime entity
- [ ] Add real player tracking to zombie
- [ ] Add player side sprite
- [ ] Add player back sprite
- [ ] Add tree sprite
- [ ] Add tree tile
- [ ] Add crafting table
- [ ] Add chest
- [ ] Add *powerglove*
- [ ] Require block breaking to use multiple hits
- [ ] Add stamina to hotbar
- [ ] Subtract from stamina upon using a tool
- [ ] Replenish stamina over time
- [ ] Add pointer to player entity in `level_t`
- [ ] Add vblank interrupt to copy `obj_t` to OAM
- [ ] Add `interrupt.h`
- [ ] Allow `bg_write_tile()` to accept coordinates greater than 32 for regular backgrounds
- [ ] Add `hotbar.h`
- [ ] Add `ent_remove(level_t *, ent_t *)`
- [ ] Move tile rending functions into a new file, `render.c`/`.h`
- [ ] Add tile drops
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
- [ ] Add indication that a tile is being destroyed
- [ ] Add inner-corner tiles for `9pt indexing`
- [x] Hide entities that leave visible screen


### Bug
- [ ] Confirm functional collision with other entities
- [ ] Out-of-bounds placing tiles is weird
- [ ] Entities are visible above the hotbar
- [ ] Grass does not connect to tree
- [ ] Cursor position does not line up with tile placement
- [ ] **Crafting table does not open**
- [ ] Game crashes opening inventory menu with no items
- [ ] `bg.h` does not support regular backgrounds with sizes larger than 32x32
- [x] Fix weird cursor positioning